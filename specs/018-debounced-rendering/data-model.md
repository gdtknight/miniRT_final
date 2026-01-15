# Data Model: Debounced Rendering

**Feature**: 018-debounced-rendering  
**Date**: 2026-01-15  
**Status**: Phase 1 - Design

## Entity Overview

The debounced rendering feature introduces three primary entities:

1. **Debounce Timer** - Tracks time since last input and determines when to trigger rendering
2. **Debounce State** - Manages render state machine and orchestrates preview→final transitions
3. **Debounce Config** - Configuration settings for debounce behavior

## Entity Definitions

### 1. Debounce Timer (`t_debounce_timer`)

**Purpose**: Track elapsed time since last keyboard input using system time APIs.

**Fields**:
```c
typedef struct s_debounce_timer
{
    struct timeval  last_input_time;    // Timestamp of most recent input
    int             is_active;           // 1 if timer running, 0 if stopped
    long            delay_ms;            // Delay threshold in milliseconds
}   t_debounce_timer;
```

**Field Descriptions**:
- `last_input_time`: Captured via `gettimeofday()` when keyboard input occurs
- `is_active`: Prevents timer checks when no debouncing is needed (optimization)
- `delay_ms`: Configurable delay (default 150ms per spec FR-008)

**Validation Rules**:
- `delay_ms` must be >= 0 (0 disables debouncing per spec edge case)
- `delay_ms` should be <= 1000ms (sanity check - longer delays feel broken)
- `last_input_time` must be valid `timeval` structure

**State Transitions**:
- Timer starts (is_active=1) when keyboard input detected
- Timer resets when new input arrives during debouncing
- Timer stops (is_active=0) when debounce completes or render finishes

---

### 2. Debounce State (`t_debounce_state`)

**Purpose**: Central state machine managing the debounce → preview → final render flow.

**Fields**:
```c
typedef enum e_debounce_state_enum
{
    DEBOUNCE_IDLE,          // No input, no pending renders
    DEBOUNCE_ACTIVE,        // Timer running, waiting for delay
    DEBOUNCE_PREVIEW,       // Preview render in progress
    DEBOUNCE_FINAL          // Final render in progress
}   t_debounce_state_enum;

typedef struct s_debounce_state
{
    t_debounce_state_enum   state;          // Current state machine state
    t_debounce_timer        timer;          // Embedded timer instance
    int                     preview_enabled; // Enable preview mode (FR-009)
    int                     auto_upgrade;    // Auto-upgrade to final (FR-010)
    int                     cancel_requested; // Flag to abort current render
}   t_debounce_state;
```

**Field Descriptions**:
- `state`: Current position in state machine (see State Transition Table below)
- `timer`: Embedded timer for tracking debounce delay
- `preview_enabled`: If 0, skip preview and go directly to final render
- `auto_upgrade`: If 0, stay in preview mode (don't auto-upgrade to final)
- `cancel_requested`: Set to 1 when new input arrives during render

**State Transition Table**:
```
+-------------------+------------------+-------------------+------------------------+
| Current State     | Event            | Next State        | Actions                |
+-------------------+------------------+-------------------+------------------------+
| IDLE              | Key press        | ACTIVE            | Start timer, dirty=1   |
| IDLE              | Timer check      | IDLE              | No-op                  |
+-------------------+------------------+-------------------+------------------------+
| ACTIVE            | Key press        | ACTIVE            | Reset timer            |
| ACTIVE            | Timer expired    | PREVIEW (if en.)  | low_quality=1, render  |
|                   |                  | or FINAL          |                        |
+-------------------+------------------+-------------------+------------------------+
| PREVIEW           | Key press        | ACTIVE            | Cancel, reset timer    |
| PREVIEW           | Render complete  | FINAL (if auto)   | low_quality=0, render  |
|                   |                  | or IDLE           |                        |
+-------------------+------------------+-------------------+------------------------+
| FINAL             | Key press        | ACTIVE            | Cancel, reset timer    |
| FINAL             | Render complete  | IDLE              | Clear dirty            |
+-------------------+------------------+-------------------+------------------------+
```

**Validation Rules**:
- State must be one of the four enum values
- `cancel_requested` must be cleared after cancel handling
- Timer must be active only in ACTIVE state
- `low_quality` flag must be 1 in PREVIEW state, 0 in FINAL state

**Relationships**:
- **Contains** one `t_debounce_timer`
- **Referenced by** `t_render` structure (window.h)
- **Interacts with** `render->dirty` and `render->low_quality` flags

---

### 3. Debounce Config (`t_debounce_config`)

**Purpose**: Configuration settings for debounce behavior (optional, for future extensibility).

**Fields**:
```c
typedef struct s_debounce_config
{
    long    delay_ms;           // Debounce delay in milliseconds
    int     preview_enabled;    // Enable preview rendering
    int     auto_upgrade;       // Auto-upgrade from preview to final
}   t_debounce_config;
```

**Field Descriptions**:
- `delay_ms`: Debounce delay threshold (default 150ms)
- `preview_enabled`: Toggle preview mode (FR-009)
- `auto_upgrade`: Toggle auto-upgrade to final render (FR-010)

**Default Values**:
```c
#define DEBOUNCE_DEFAULT_DELAY_MS       150
#define DEBOUNCE_DEFAULT_PREVIEW        1
#define DEBOUNCE_DEFAULT_AUTO_UPGRADE   1
```

**Validation Rules**:
- `delay_ms` must be in range [0, 1000] (0 disables debouncing)
- Boolean flags must be 0 or 1

**Note**: Initial implementation uses compile-time defaults. This structure provides extension point for runtime configuration without breaking existing code.

---

## Integration with Existing Structures

### Modified Structure: `t_render` (window.h)

**Current Definition** (partial):
```c
typedef struct s_render
{
    // ... existing fields ...
    int     dirty;              // EXISTING: Triggers re-render
    int     low_quality;        // EXISTING: Controls render quality
    // ... other fields ...
}   t_render;
```

**Modified Definition**:
```c
typedef struct s_render
{
    // ... existing fields ...
    int                 dirty;              // EXISTING: Triggers re-render
    int                 low_quality;        // EXISTING: Controls render quality
    t_debounce_state    debounce;           // NEW: Debounce state machine
    // ... other fields ...
}   t_render;
```

**Change Impact**:
- Adds one field: `t_debounce_state debounce`
- No changes to existing field meanings or usage
- Initialization in `init_window()` (window_init.c)
- No memory allocation required (struct embedded by value)

---

## Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         Event Loop                               │
│                    (mlx_loop_hook callback)                      │
└────────────┬────────────────────────────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────────────────────────────┐
│ Input Handler (handle_key)                                       │
│  - Detects movement key press                                    │
│  - Calls: debounce_on_input(&render->debounce)                  │
└────────────┬────────────────────────────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────────────────────────────┐
│ Debounce State Machine (debounce_update)                        │
│  - Checks timer: timer_expired(&debounce->timer)                │
│  - Transitions state based on events                             │
│  - Sets render->dirty and render->low_quality                    │
└────────────┬────────────────────────────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────────────────────────────┐
│ Render Loop (render_loop)                                        │
│  - If dirty: calls render_scene_to_buffer()                      │
│  - Render checks cancel flag per scanline                        │
│  - On complete: clears dirty flag                                │
└────────────┬────────────────────────────────────────────────────┘
             │
             ▼
┌─────────────────────────────────────────────────────────────────┐
│ State Machine Update (on render complete)                        │
│  - PREVIEW complete → transition to FINAL                        │
│  - FINAL complete → transition to IDLE                           │
└─────────────────────────────────────────────────────────────────┘
```

## Memory Management

**Allocation Strategy**:
- All debounce structures embedded by value (no malloc required)
- `t_debounce_state` embedded in `t_render` structure
- `t_debounce_timer` embedded in `t_debounce_state` structure
- Zero heap allocations = zero memory leaks

**Initialization**:
```c
void debounce_init(t_debounce_state *state)
{
    state->state = DEBOUNCE_IDLE;
    state->timer.is_active = 0;
    state->timer.delay_ms = DEBOUNCE_DEFAULT_DELAY_MS;
    state->preview_enabled = DEBOUNCE_DEFAULT_PREVIEW;
    state->auto_upgrade = DEBOUNCE_DEFAULT_AUTO_UPGRADE;
    state->cancel_requested = 0;
}
```

**Cleanup**:
- No cleanup required (no dynamically allocated memory)
- State reset handled by `debounce_init()` if needed

---

## API Surface

See `contracts/debounce-api.md` for detailed function signatures and contracts.

**Key Functions**:
1. `debounce_init()` - Initialize state machine
2. `debounce_on_input()` - Handle keyboard input event
3. `debounce_update()` - Update state machine (called per frame)
4. `debounce_cancel()` - Cancel in-progress render
5. `timer_start()` - Start debounce timer
6. `timer_reset()` - Reset timer to current time
7. `timer_expired()` - Check if delay elapsed

---

## Testing Considerations

**Unit Testing** (if applicable):
- Timer arithmetic: Verify elapsed time calculations
- State transitions: Verify all state machine paths
- Edge cases: Zero delay, timer overflow, rapid input

**Integration Testing**:
- Key press → debounce → preview → final (happy path)
- Rapid key presses (should reset timer, no intermediate renders)
- Cancel during preview (should abort and restart debounce)
- Cancel during final (should abort and restart debounce)

**Manual Testing Scenarios**:
1. Hold WASD key for 2 seconds → verify no renders during hold
2. Release key → verify preview starts within 200ms
3. Press key during preview → verify render cancels immediately
4. Let preview complete → verify final render starts automatically
5. Test with `delay_ms = 0` → verify debouncing disabled

---

## Performance Characteristics

**Time Complexity**:
- `timer_expired()`: O(1) - simple arithmetic
- `debounce_update()`: O(1) - state machine with constant branches
- `debounce_on_input()`: O(1) - flag setting and timer reset

**Space Complexity**:
- `t_debounce_state`: ~48 bytes (depends on platform)
  - `state` (4 bytes enum)
  - `timer` (16 bytes struct timeval + 4 byte int + 8 byte long)
  - `preview_enabled`, `auto_upgrade`, `cancel_requested` (3 × 4 bytes)

**Memory Overhead**:
- Per-window: +48 bytes (negligible for single-window application)
- No heap allocations: Zero malloc/free overhead

---

## Future Extensions

**Possible Enhancements** (not in initial implementation):
1. Runtime configuration via UI or config file
2. Per-key-type debounce delays (camera vs object movement)
3. Adaptive delay based on input velocity
4. Debounce statistics (average delay, cancel count)
5. Multiple debounce contexts (camera, objects, lights)

**Extensibility Points**:
- `t_debounce_config` structure ready for runtime config
- State machine can be extended with additional states
- Timer implementation abstracted into separate functions

---

## Compliance Checklist

- ✅ Uses only allowed 42 School functions (`gettimeofday`, standard C)
- ✅ No pthread, fork, pipe, or multiprocessing
- ✅ No dynamic memory allocation (embedded structures)
- ✅ Norminette compliant (struct naming, formatting)
- ✅ Integrates with existing code without major refactoring
- ✅ Minimal changes to existing structures
