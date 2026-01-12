# Research Document: BVH Tree Visualization Improvements

**Feature**: 015-bvh-viz-improvements  
**Date**: 2026-01-12  
**Phase**: Phase 0 - Research & Design Decisions

## Overview

This document consolidates all research decisions and rationale for implementing unified object identification and debounced keyboard rendering in the miniRT BVH tree visualization system.

## Research Areas

### 1. Object Identifier Storage Strategy

**Decision**: Store object identifiers directly in each object struct as `char id[8]`

**Rationale**:
- **Memory efficiency**: 8 bytes per object is minimal overhead (0.16KB for 100 objects)
- **Access performance**: O(1) direct access, no lookups or hash tables needed
- **Simplicity**: No additional data structures to manage or synchronize
- **Cache-friendly**: Identifier stored with object data, improves spatial locality
- **Parse-time generation**: IDs generated once during scene loading, zero runtime cost

**Alternatives Considered**:
1. **Hash table mapping**: Rejected - adds complexity, memory overhead, pointer indirection
2. **Separate ID array**: Rejected - requires index synchronization, cache misses
3. **Dynamic string allocation**: Rejected - violates 42 constraints, memory management overhead
4. **Compute ID on-the-fly**: Rejected - requires passing type/index everywhere, CPU waste

**Implementation Details**:
- Format: `<type>-<number>` (e.g., "sp-1", "pl-2", "cy-3")
- Storage: Fixed char array `char id[8]` (sufficient for "cy-999\0")
- Generation: During parse_objects.c, use snprintf with counter per type
- Display: Direct string copy to HUD/BVH visualization buffers

---

### 2. Timer State Machine Design

**Decision**: Enum-based state machine with three states (IDLE/PENDING/RENDERING)

**Rationale**:
- **Clarity**: Explicit states prevent invalid transitions and edge case bugs
- **Debuggability**: State transitions can be logged for observability (FR requirement)
- **Maintainability**: Easy to understand and modify compared to boolean flags
- **42 School compliant**: Pure algorithmic solution, no threading required

**State Transition Rules**:
```
IDLE → PENDING:       First keyboard input received
PENDING → PENDING:    Subsequent keyboard input (reset timer)
PENDING → RENDERING:  Timer expires after 1 second
PENDING → IDLE:       Mouse action OR view switch (discard inputs)
RENDERING → PENDING:  Render completes AND queued inputs exist
RENDERING → IDLE:     Render completes AND no queued inputs
```

**Alternatives Considered**:
1. **Boolean flags (is_pending, is_rendering)**: Rejected - leads to invalid state combinations
2. **Bitfield states**: Rejected - obscures logic, harder to debug
3. **Callback-based**: Rejected - increases complexity, harder to trace execution

**Implementation Details**:
- Enum: `typedef enum { TIMER_IDLE, TIMER_PENDING, TIMER_RENDERING } t_timer_state`
- Transition function: `timer_transition(t_debounce *timer, t_timer_event event)`
- Logging: State changes logged to stderr with timestamp (OBS requirement)
- Validation: Assert valid transitions in debug builds

---

### 3. Timing Mechanism Selection

**Decision**: Use `gettimeofday()` for microsecond-precision timestamps

**Rationale**:
- **42 School allowed**: Standard POSIX function, in allowed libc list
- **Sufficient precision**: Microsecond resolution far exceeds 1-second requirement
- **Simplicity**: Single function call, no complex setup
- **Cross-platform**: Works on Linux and macOS (miniRT target platforms)
- **Low overhead**: Syscall cost ~100ns, negligible for 1-second intervals

**Alternatives Considered**:
1. **clock_gettime(CLOCK_MONOTONIC)**: Better but not in 42 allowed list
2. **time()**: Rejected - only 1-second resolution, insufficient for smooth UX
3. **MLX timer hooks**: Rejected - not designed for debouncing, adds MLX coupling
4. **busy-wait loops**: Rejected - wastes CPU, violates <5% overhead requirement

**Implementation Details**:
```c
struct timeval start, now;
gettimeofday(&start, NULL);
// ... later
gettimeofday(&now, NULL);
long elapsed_us = (now.tv_sec - start.tv_sec) * 1000000 + 
                  (now.tv_usec - start.tv_usec);
int expired = (elapsed_us >= 1000000); // 1 second = 1,000,000 microseconds
```

**Performance**: Each gettimeofday() call ~0.0001ms, checking timer every render frame (60fps) = 0.006ms/sec = 0.0006% CPU

---

### 4. Timer Pool Architecture

**Decision**: Pre-allocated fixed array of 4 timer slots

**Rationale**:
- **42 School compliant**: No dynamic allocation during runtime, malloc only at init
- **Deterministic behavior**: Fixed capacity prevents unbounded growth
- **Simple allocation**: Linear search for free slot, O(n) acceptable for n=4
- **Error handling**: Return error code on overflow, let caller decide fallback
- **Memory efficient**: 4 timers × ~32 bytes = 128 bytes total

**Capacity Justification**:
- Current views: HUD (1), BVH visualization (1), camera controls (1), spare (1)
- Overflow scenario: Only if >4 concurrent components need debouncing (unlikely)
- Graceful degradation: Component falls back to immediate rendering on pool full

**Alternatives Considered**:
1. **Dynamic linked list**: Rejected - violates 42 constraints, malloc during runtime
2. **Single global timer**: Rejected - conflicts between multiple views
3. **Unlimited pool**: Rejected - unbounded memory, no error handling
4. **Per-component timer**: Rejected - duplicates code, harder to manage

**Implementation Details**:
```c
typedef struct s_timer_slot {
    int             in_use;
    t_timer_state   state;
    struct timeval  start_time;
    void            *context;
} t_timer_slot;

typedef struct s_timer_pool {
    t_timer_slot slots[4];
    int          active_count;
} t_timer_pool;
```

---

### 5. Input Queue Design

**Decision**: Fixed-capacity circular buffer with 8 slots, reject on overflow

**Rationale**:
- **Loss prevention**: Buffers inputs during rendering to prevent dropped keystrokes
- **Bounded memory**: 8 inputs × ~16 bytes = 128 bytes, acceptable overhead
- **42 School compliant**: Fixed array, no dynamic allocation
- **Overflow policy**: Reject newest input preserves most recent user intent
- **Simple implementation**: Head/tail pointers, modulo arithmetic for wrap

**Capacity Justification**:
- Typical render time: 16ms (60fps) to 100ms (complex scene)
- Keyboard repeat rate: ~30 keys/sec = 3 keys per 100ms worst case
- 8 slots = 260ms buffering capacity, covers 2.6× worst case + margin

**Overflow Behavior**:
- When full (8 inputs queued): Discard incoming input, no queue modification
- Rationale: Preserves earliest inputs (user's committed actions) over speculative taps
- Alternative "discard oldest" rejected: Would lose user's actual intent

**Alternatives Considered**:
1. **Unbounded queue**: Rejected - memory growth, no 42 compliance
2. **Discard oldest**: Rejected - loses confirmed user actions
3. **Merge consecutive same-key**: Rejected - changes user input semantics
4. **Larger capacity (16+)**: Rejected - unnecessary for measured use case

**Implementation Details**:
```c
typedef struct s_input_queue {
    int      keys[8];        // Key codes
    int      head;           // Next write position
    int      tail;           // Next read position
    int      count;          // Current item count
} t_input_queue;

// Enqueue returns 0 on success, -1 if full (rejects input)
int queue_enqueue(t_input_queue *q, int keycode);
```

---

### 6. Progress Bar Rendering

**Decision**: Bottom-left 200×20px green fill progress bar

**Rationale**:
- **Visual feedback**: User knows system is responsive, debounce is active
- **Non-intrusive**: Bottom-left corner rarely overlaps important content
- **Simple rendering**: Rectangle fill using MLX primitives, <0.1ms render time
- **Color choice**: Green = "processing/waiting" (standard UI convention)
- **Size**: 200px wide = clear visibility, 20px tall = readable but compact

**Update frequency**: Every render frame (60fps) = smooth animation

**Alternatives Considered**:
1. **Text countdown ("0.8s remaining")**: Rejected - more CPU, less intuitive
2. **Spinner animation**: Rejected - more complex, no progress indication
3. **Top overlay**: Rejected - blocks HUD content
4. **Right-side**: Rejected - blocks BVH tree visualization

**Implementation Details**:
```c
void render_progress_bar(t_render *render, double progress) {
    int x = 10;                     // Bottom-left position
    int y = WINDOW_HEIGHT - 30;
    int width = 200;
    int height = 20;
    int filled_width = (int)(width * progress);
    
    // Background (dark gray)
    mlx_rect(render->mlx, render->win, x, y, width, height, 0x333333);
    // Foreground (green fill)
    mlx_rect(render->mlx, render->win, x, y, filled_width, height, 0x00FF00);
}
```

---

### 7. Mouse/View Cancellation Behavior

**Decision**: Mouse actions cancel debounce AND discard all queued keyboard inputs

**Rationale**:
- **User intent**: Mouse action = new interaction, keyboard inputs now irrelevant
- **Responsiveness**: Immediate mouse handling, no stale keyboard delays
- **Predictability**: Clear behavior - mouse action "clears the slate"
- **Edge case handling**: Prevents confusing behavior when user switches input method

**View Switch Behavior**: Cancel timer AND discard pending renders entirely
- Rationale: New view = different visualization, old render is obsolete
- Prevents: Rendering old view's state after switching to new view

**Alternatives Considered**:
1. **Process queued inputs then handle mouse**: Rejected - adds unpredictable delay
2. **Preserve keyboard queue**: Rejected - confusing when user switched to mouse
3. **Separate timers per view**: Rejected - resource waste, complexity

**Implementation Details**:
- Mouse handler calls `debounce_cancel(timer)` + `queue_clear(queue)`
- View switch calls `debounce_cancel_all(pool)` + `queue_clear_all()`
- Atomic operation: Cancel state transition happens before mouse/view processing

---

## Technology Stack Decisions

### Allowed Functions (42 School Constraints)

**Standard C Library**:
- `gettimeofday()` - Timer timestamps
- `snprintf()` - Object ID formatting
- `malloc()`/`free()` - Initial allocation only (timer pool, queue)
- `memset()`/`memcpy()` - Struct initialization

**Math Library**:
- No special math functions needed for this feature

**MiniLibX**:
- `mlx_string_put()` - Text rendering (object IDs)
- `mlx_pixel_put()` or rect drawing - Progress bar rendering

**Custom (libft/GNL)**:
- Existing string utilities if available

---

## Integration Points

### Parser Integration
- **File**: `src/parser/parse_objects.c`
- **Change**: Add ID generation during object parsing
- **Counter tracking**: Static int per type, increments per object

### HUD Integration
- **File**: `src/hud/hud_objects.c`
- **Change**: Display `object.id` instead of generating format
- **New file**: `src/hud/hud_progress.c` - Progress bar rendering

### BVH Visualization Integration
- **File**: `src/bvh_vis/bvh_vis_node.c`
- **Change**: Display object ID from `object_ref->object.id`

### Window Handler Integration
- **File**: `src/window/handle_key.c`
- **Change**: Route keyboard inputs through debounce_process_input()
- **File**: `src/window/handle_mouse.c`
- **Change**: Call debounce_cancel() on mouse events

---

## Performance Analysis

### CPU Overhead Breakdown

| Operation | Frequency | Cost per Call | Total CPU % |
|-----------|-----------|---------------|-------------|
| gettimeofday() | 60/sec (render loop) | 0.0001ms | 0.0006% |
| State check | 60/sec | 0.00001ms | 0.00006% |
| Queue operations | <10/sec (user input) | 0.0001ms | 0.000001% |
| Progress bar render | 60/sec | 0.05ms | 0.3% |
| **TOTAL** | - | - | **~0.3%** |

**Conclusion**: Well below 5% constraint, dominated by progress bar rendering

### Memory Overhead

| Component | Size | Count | Total |
|-----------|------|-------|-------|
| Object IDs | 8 bytes | 100 objects | 0.8 KB |
| Timer pool | 32 bytes | 4 slots | 128 bytes |
| Input queue | 16 bytes | 1 queue | 16 bytes |
| **TOTAL** | - | - | **~1 KB** |

**Conclusion**: Negligible memory impact (<0.01% of typical scene)

---

## Risk Mitigation

### Risk 1: Timer Drift
- **Mitigation**: Use absolute timestamps, calculate elapsed time from start
- **Test**: Verify 1-second delay accurate within ±10ms

### Risk 2: Input Queue Overflow
- **Mitigation**: 8-slot capacity covers 260ms worst-case, log overflow events
- **Test**: Rapid key mashing (>30 keys/sec) for 500ms, verify no loss until full

### Risk 3: State Machine Invalid Transitions
- **Mitigation**: Enum + switch statement prevents invalid states, debug asserts
- **Test**: Fuzz state transitions with random input sequences

### Risk 4: Mouse Action During Render
- **Mitigation**: Check cancellation flag at render loop start, abort early
- **Test**: Click mouse during slow render, verify immediate cancellation

---

## Conclusion

All research areas resolved with clear decisions backed by rationale, alternatives analysis, and 42 School constraint compliance. Implementation can proceed to Phase 1 (data model and contracts) with no outstanding clarifications.

**Key Takeaways**:
1. Object IDs: Stored in-struct (char id[8]), generated at parse time
2. Debounce: State machine (enum) + gettimeofday() timing + fixed pool (4 slots)
3. Input buffering: Fixed queue (8 slots), reject on overflow
4. Cancellation: Mouse/view actions clear state and discard inputs
5. Performance: <0.3% CPU overhead, ~1KB memory, meets all constraints
