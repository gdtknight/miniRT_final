# Data Model: BVH Tree Visualization Improvements

**Feature**: 015-bvh-viz-improvements  
**Date**: 2026-01-12  
**Phase**: Phase 1 - Data Model Design

## Overview

This document defines all data structures, entities, and state models for the unified object identification and debounced keyboard rendering features.

---

## Entity Definitions

### 1. Object Identifier

**Purpose**: Unique string identifier for each scene object, stored directly in object structs

**Structure Modifications**:

```c
/* objects.h - Add char id[8] field to each object type */

typedef struct s_sphere
{
    t_vec3  center;
    double  diameter;
    t_color color;
    double  radius;
    double  radius_squared;
    char    id[8];          // NEW: Object identifier "sp-N\0"
}   t_sphere;

typedef struct s_plane
{
    t_vec3  point;
    t_vec3  normal;
    t_color color;
    char    id[8];          // NEW: Object identifier "pl-N\0"
}   t_plane;

typedef struct s_cylinder
{
    t_vec3  center;
    t_vec3  axis;
    double  diameter;
    double  height;
    t_color color;
    double  radius;
    double  radius_squared;
    double  half_height;
    char    id[8];          // NEW: Object identifier "cy-N\0"
}   t_cylinder;
```

**Format Specification**:
- Pattern: `<type>-<number>` (lowercase type, hyphen, sequential number)
- Type codes: `sp` (sphere), `pl` (plane), `cy` (cylinder)
- Number range: 1-999 (supports up to 999 objects per type)
- Storage: 8 bytes (max "cy-999\0" = 7 chars + null terminator)
- Examples: "sp-1", "sp-2", "pl-1", "cy-3"

**Validation Rules**:
- Type prefix MUST be exactly 2 lowercase letters
- Separator MUST be single hyphen '-'
- Number MUST be >= 1, no leading zeros
- Total length MUST be <= 7 characters (excluding null terminator)

**Lifecycle**:
- **Created**: During scene file parsing (parse_objects.c)
- **Accessed**: HUD rendering, BVH visualization, debug output
- **Modified**: Never (immutable after creation)
- **Destroyed**: With parent object on scene cleanup

---

### 2. Debounce Timer State Machine

**Purpose**: Manages keyboard input debouncing with explicit state transitions

**Enum Definition**:

```c
/* debounce.h - Timer state machine states */

typedef enum e_timer_state
{
    TIMER_IDLE,         // No pending input, timer not active
    TIMER_PENDING,      // Input received, waiting for delay
    TIMER_RENDERING     // Rendering in progress, queue new inputs
}   t_timer_state;
```

**State Transition Events**:

```c
typedef enum e_timer_event
{
    EVENT_KEYBOARD_INPUT,   // User pressed keyboard key
    EVENT_TIMER_EXPIRED,    // 1-second delay elapsed
    EVENT_RENDER_START,     // Rendering operation began
    EVENT_RENDER_COMPLETE,  // Rendering operation finished
    EVENT_MOUSE_ACTION,     // User performed mouse action
    EVENT_VIEW_SWITCH       // User switched HUD/BVH view
}   t_timer_event;
```

**Transition Table**:

| Current State | Event | Next State | Actions |
|--------------|-------|------------|---------|
| IDLE | KEYBOARD_INPUT | PENDING | Start timer, enqueue input |
| PENDING | KEYBOARD_INPUT | PENDING | Reset timer, enqueue input |
| PENDING | TIMER_EXPIRED | RENDERING | Trigger render, log transition |
| PENDING | MOUSE_ACTION | IDLE | Cancel timer, clear queue |
| PENDING | VIEW_SWITCH | IDLE | Cancel timer, clear queue |
| RENDERING | KEYBOARD_INPUT | RENDERING | Enqueue input (or reject if full) |
| RENDERING | RENDER_COMPLETE (queue empty) | IDLE | Log transition |
| RENDERING | RENDER_COMPLETE (queue not empty) | PENDING | Start timer, log transition |
| RENDERING | MOUSE_ACTION | IDLE | Abort render, clear queue |

**Invalid Transitions**: All other combinations log error and maintain current state

---

### 3. Timer Slot

**Purpose**: Individual timer instance within the pre-allocated pool

**Structure Definition**:

```c
/* debounce.h - Timer slot in pool */

typedef struct s_timer_slot
{
    int             in_use;         // 1 if slot allocated, 0 if free
    t_timer_state   state;          // Current state machine state
    struct timeval  start_time;     // When timer started (for PENDING state)
    void            *context;       // Owner context (e.g., HUD, BVH vis)
    t_input_queue   input_queue;    // Input buffer for this timer
}   t_timer_slot;
```

**Field Semantics**:
- `in_use`: Slot allocation flag (0 = free, 1 = allocated)
- `state`: Current state in state machine (IDLE/PENDING/RENDERING)
- `start_time`: Timestamp when PENDING state entered (from gettimeofday())
- `context`: Pointer to component using this timer (for logging/debugging)
- `input_queue`: Per-timer input queue (8 slots, see below)

**Lifecycle**:
- **Allocated**: When component requests timer via `timer_pool_acquire()`
- **Used**: State transitions, timer checks, input queuing
- **Released**: When component no longer needs timer via `timer_pool_release()`
- **Reset**: On scene reload to clear stale references

---

### 4. Timer Pool

**Purpose**: Pre-allocated array of timer slots with allocation management

**Structure Definition**:

```c
/* debounce.h - Global timer pool */

typedef struct s_timer_pool
{
    t_timer_slot    slots[4];       // Fixed array of 4 timer slots
    int             active_count;   // Number of currently allocated slots
}   t_timer_pool;
```

**Capacity**: 4 slots (sufficient for HUD, BVH viz, camera, spare)

**Allocation Strategy**:
- Linear search for first free slot (in_use == 0)
- Return slot index on success, -1 on pool full
- Caller handles error (log warning, fall back to immediate rendering)

**Operations**:

```c
// Initialize pool (called once at program start)
void timer_pool_init(t_timer_pool *pool);

// Acquire timer slot, returns index or -1 if full
int timer_pool_acquire(t_timer_pool *pool, void *context);

// Release timer slot by index
void timer_pool_release(t_timer_pool *pool, int slot_index);

// Reset all slots (on scene reload)
void timer_pool_reset(t_timer_pool *pool);

// Get slot by index (for state operations)
t_timer_slot *timer_pool_get(t_timer_pool *pool, int slot_index);
```

**Global Instance**:

```c
// debounce.c - Single global timer pool
static t_timer_pool g_timer_pool;
```

---

### 5. Input Queue

**Purpose**: Fixed-capacity circular buffer for keyboard inputs during rendering

**Structure Definition**:

```c
/* debounce.h - Input queue (per timer slot) */

typedef struct s_input_queue
{
    int     keys[8];        // Circular buffer of key codes
    int     head;           // Next write position (0-7)
    int     tail;           // Next read position (0-7)
    int     count;          // Current number of items (0-8)
}   t_input_queue;
```

**Capacity**: 8 key codes (fixed array)

**Circular Buffer Logic**:
- `head`: Index where next enqueue writes (wraps at 8)
- `tail`: Index where next dequeue reads (wraps at 8)
- `count`: Number of items currently in queue (0-8)
- Empty: `count == 0`
- Full: `count == 8`

**Operations**:

```c
// Initialize empty queue
void queue_init(t_input_queue *q);

// Enqueue key, returns 0 on success, -1 if full (rejects input)
int queue_enqueue(t_input_queue *q, int keycode);

// Dequeue key, returns keycode on success, -1 if empty
int queue_dequeue(t_input_queue *q);

// Check if queue is full
int queue_is_full(t_input_queue *q);

// Check if queue is empty
int queue_is_empty(t_input_queue *q);

// Clear all items (reset to empty)
void queue_clear(t_input_queue *q);
```

**Overflow Behavior**: When full, `queue_enqueue()` returns -1 and input is discarded

---

### 6. Progress Bar State

**Purpose**: Visual representation of debounce countdown progress

**Structure Definition**:

```c
/* debounce.h - Progress bar rendering state */

typedef struct s_progress_bar
{
    int     x;              // Bottom-left X position (pixels)
    int     y;              // Bottom-left Y position (pixels)
    int     width;          // Total bar width (pixels)
    int     height;         // Bar height (pixels)
    int     bg_color;       // Background color (0xRRGGBB)
    int     fg_color;       // Foreground fill color (0xRRGGBB)
}   t_progress_bar;
```

**Default Configuration**:

```c
#define PROGRESS_BAR_X      10
#define PROGRESS_BAR_Y      (WINDOW_HEIGHT - 30)
#define PROGRESS_BAR_WIDTH  200
#define PROGRESS_BAR_HEIGHT 20
#define PROGRESS_BAR_BG     0x333333  // Dark gray
#define PROGRESS_BAR_FG     0x00FF00  // Green
```

**Progress Calculation**:

```c
// Calculate progress (0.0 to 1.0) from elapsed time
double calculate_progress(struct timeval *start) {
    struct timeval now;
    gettimeofday(&now, NULL);
    
    long elapsed_us = (now.tv_sec - start->tv_sec) * 1000000 + 
                      (now.tv_usec - start->tv_usec);
    
    // 1 second = 1,000,000 microseconds
    double progress = (double)elapsed_us / 1000000.0;
    
    // Clamp to [0.0, 1.0]
    if (progress < 0.0) progress = 0.0;
    if (progress > 1.0) progress = 1.0;
    
    return progress;
}
```

**Rendering**:
- Background rectangle: Full width, dark gray
- Foreground rectangle: `width * progress`, green
- Update frequency: Every render frame (60 FPS) when timer is PENDING

---

## Memory Layout

### Object with Identifier

```
t_sphere (56 bytes on 64-bit system):
  0-23:   t_vec3 center (3 × double = 24 bytes)
  24-31:  double diameter (8 bytes)
  32-44:  t_color color (3 × int = 12 bytes)
  45-52:  double radius (8 bytes)
  53-60:  double radius_squared (8 bytes)
  61-68:  char id[8] (8 bytes, includes padding)
```

### Timer Slot

```
t_timer_slot (48 bytes on 64-bit system):
  0-3:    int in_use (4 bytes)
  4-7:    t_timer_state state (4 bytes, enum)
  8-23:   struct timeval start_time (16 bytes: tv_sec + tv_usec)
  24-31:  void *context (8 bytes, pointer)
  32-47:  t_input_queue input_queue (16 bytes: see below)
```

### Input Queue

```
t_input_queue (44 bytes):
  0-31:   int keys[8] (8 × 4 bytes = 32 bytes)
  32-35:  int head (4 bytes)
  36-39:  int tail (4 bytes)
  40-43:  int count (4 bytes)
```

**Total Memory Overhead**:
- 100 objects × 8 bytes (IDs) = 800 bytes
- 4 timer slots × 48 bytes = 192 bytes
- **Total: ~1 KB**

---

## Conclusion

Data model defines all entities, structures, states, and relationships needed for implementation. All structures are designed for:
- **42 School compliance**: Fixed arrays, no dynamic allocation during runtime
- **Memory efficiency**: ~1 KB total overhead for 100 objects + 4 timers
- **Performance**: O(1) access to identifiers, O(n) timer pool search (n=4)
- **Maintainability**: Clear ownership, explicit state machines, validation rules

Ready for Phase 1 contracts and Phase 2 implementation.
