# Research: Debounced Rendering with Preview Mode

**Feature**: 018-debounced-rendering  
**Date**: 2026-01-15  
**Purpose**: Resolve technical unknowns and establish implementation patterns

## Research Tasks

### 1. Timer Implementation Without Threads

**Question**: How to implement debounce timer using only allowed 42 School functions?

**Research Findings**:

**Decision**: Use `gettimeofday()` for timestamp tracking with polling in event loop

**Rationale**:
- `gettimeofday()` is a standard libc function allowed by 42 School constraints
- Provides microsecond precision (sufficient for 150ms debounce delay)
- No threads required - timer checked during each iteration of MiniLibX event loop
- `mlx_loop_hook()` provides continuous callback for timer polling
- Existing codebase already uses `struct timeval` in `render_state.h` for interaction tracking

**Implementation Pattern**:
```c
typedef struct s_debounce_timer {
    struct timeval  last_input_time;
    int             is_active;
    long            delay_ms;      // Default 150ms
} t_debounce_timer;

// Check if timer has expired
int timer_expired(t_debounce_timer *timer) {
    struct timeval now;
    long elapsed_ms;
    
    if (!timer->is_active)
        return (0);
    
    gettimeofday(&now, NULL);
    elapsed_ms = (now.tv_sec - timer->last_input_time.tv_sec) * 1000;
    elapsed_ms += (now.tv_usec - timer->last_input_time.tv_usec) / 1000;
    
    return (elapsed_ms >= timer->delay_ms);
}
```

**Alternatives Considered**:
- `clock_gettime()`: More precise but may not be universally available on all 42 School Macs
- Frame counting: Less precise, couples timer to frame rate
- `usleep()` in blocking loop: Would freeze the UI, violates interactive requirement

**42 School Compliance**: ✅ `gettimeofday()` is in allowed libc functions

---

### 2. Render Cancellation Mechanism

**Question**: How to cancel an in-progress render without threads or signals?

**Research Findings**:

**Decision**: State-based cancellation using flag checking during render loop

**Rationale**:
- Current render loop in `render_scene_to_buffer()` iterates pixel-by-pixel
- Can check cancellation flag at each scanline (every WINDOW_WIDTH pixels)
- Provides cancellation within 1-2ms on typical hardware (sub-frame latency)
- No threads means no race conditions - simple flag is sufficient
- Existing `dirty` flag pattern already established in codebase

**Implementation Pattern**:
```c
typedef enum e_render_state {
    RENDER_IDLE,
    RENDER_DEBOUNCING,
    RENDER_PREVIEW,
    RENDER_FINAL,
    RENDER_CANCELLED
} t_render_state_enum;

// In render loop (render.c):
void render_scene_to_buffer(t_scene *scene, t_render *render) {
    int x, y;
    for (y = 0; y < WINDOW_HEIGHT; y++) {
        // Check cancellation at each scanline
        if (render->debounce.state == RENDER_CANCELLED)
            return;
        
        for (x = 0; x < WINDOW_WIDTH; x++) {
            render_pixel(scene, render, x, y);
        }
    }
}
```

**Alternatives Considered**:
- Signal-based interruption: Requires signal handlers, too complex for this use case
- Thread cancellation: Prohibited by 42 School constraints
- No cancellation: Would cause render queue buildup and lag

**42 School Compliance**: ✅ Pure C state machine, no prohibited functions

---

### 3. State Machine Design for Debouncing

**Question**: What state transitions are needed for debounce → preview → final flow?

**Research Findings**:

**Decision**: Four-state machine: IDLE → DEBOUNCING → PREVIEW → FINAL

**Rationale**:
- **IDLE**: No input, no pending renders - default state after final render completes
- **DEBOUNCING**: Input stopped, timer counting down - waits for delay_ms before preview
- **PREVIEW**: Low-quality render in progress - uses existing `low_quality=1` flag
- **FINAL**: High-quality render in progress - uses existing `low_quality=0` flag
- Any new input from any state → reset to DEBOUNCING (except during IDLE, which stays IDLE until first input)

**State Transition Table**:
```
Current State    | Event                  | Next State        | Action
-----------------|------------------------|-------------------|---------------------------
IDLE             | Key press              | DEBOUNCING        | Start timer, set dirty=1
DEBOUNCING       | Key press              | DEBOUNCING        | Reset timer
DEBOUNCING       | Timer expired          | PREVIEW           | low_quality=1, render
PREVIEW          | Key press              | DEBOUNCING        | Cancel render, reset timer
PREVIEW          | Render complete        | FINAL             | low_quality=0, render
FINAL            | Key press              | DEBOUNCING        | Cancel render, reset timer
FINAL            | Render complete        | IDLE              | Clear dirty flag
ANY              | Cancel requested       | DEBOUNCING        | Abort render, reset timer
```

**Integration with Existing Code**:
- Current code uses `render->dirty` to trigger re-render
- Current code uses `render->low_quality` to control quality mode
- New state machine orchestrates when to set these flags
- `handle_key_release()` currently transitions low_quality → high_quality immediately
- **KEY CHANGE**: Remove immediate transition in `handle_key_release()`, delegate to debounce state machine

**Alternatives Considered**:
- Two-state (debouncing/rendering): Insufficient - can't distinguish preview from final
- Three-state (idle/debouncing/rendering): Can't handle preview→final auto-upgrade
- Five+ states: Over-engineered, adds complexity without benefit

**42 School Compliance**: ✅ Simple enum-based state machine, no external dependencies

---

### 4. Integration with Existing Event Loop

**Question**: How to integrate timer polling with MiniLibX event loop?

**Research Findings**:

**Decision**: Use `mlx_loop_hook()` for continuous timer polling and state updates

**Rationale**:
- MiniLibX provides `mlx_loop_hook(mlx, function, param)` for idle-time callbacks
- Called continuously when no events are pending
- Existing `render_loop()` already uses this for continuous rendering
- Can check debounce timer and trigger state transitions here
- No polling overhead - only checks timer when state != IDLE

**Implementation Pattern**:
```c
// Current render_loop() in window_events.c already exists
// Enhance it with debounce state machine update

int render_loop(void *param) {
    t_render *render = (t_render *)param;
    
    // NEW: Update debounce state machine
    debounce_update(&render->debounce, render);
    
    // EXISTING: Render if dirty
    if (render->dirty) {
        render_scene_to_buffer(render->scene, render);
        mlx_put_image_to_window(...);
        render->dirty = 0;
    }
    
    return (0);
}
```

**Debounce Update Logic**:
```c
void debounce_update(t_debounce_state *deb, t_render *render) {
    if (deb->state == RENDER_DEBOUNCING && timer_expired(&deb->timer)) {
        // Transition: DEBOUNCING → PREVIEW
        deb->state = RENDER_PREVIEW;
        render->low_quality = 1;
        render->dirty = 1;
    }
    else if (deb->state == RENDER_PREVIEW && !render->dirty) {
        // Preview complete, start final render
        deb->state = RENDER_FINAL;
        render->low_quality = 0;
        render->dirty = 1;
    }
    else if (deb->state == RENDER_FINAL && !render->dirty) {
        // Final render complete
        deb->state = RENDER_IDLE;
    }
}
```

**Alternatives Considered**:
- Separate timer thread: Prohibited by 42 School constraints
- `usleep()` in blocking loop: Would freeze UI
- Event-based timer (X11 timer events): Not portable, MiniLibX abstraction doesn't expose

**42 School Compliance**: ✅ Uses only mlx_loop_hook() and allowed libc functions

---

### 5. Configuration and Tuning

**Question**: What debounce delay provides optimal user experience?

**Research Findings**:

**Decision**: Default 150ms, configurable at compile time or runtime

**Rationale**:
- Human perception: 100-200ms is perceptible but feels responsive
- Too short (<100ms): May trigger preview on brief pauses between keypresses
- Too long (>300ms): Feels laggy, user waits too long for feedback
- Industry standard: Most UI frameworks use 150-300ms for debouncing
- **Spec requirement**: 150ms default (FR-008), preview within 200ms (SC-002)

**Configuration Strategy**:
```c
// Compile-time default
#define DEBOUNCE_DELAY_MS 150

// Runtime configuration (optional, not in initial implementation)
typedef struct s_debounce_config {
    long delay_ms;              // Debounce delay (default 150ms)
    int preview_enabled;        // Enable preview mode (default 1)
    int auto_upgrade;           // Auto-upgrade to final (default 1)
} t_debounce_config;
```

**Testing Strategy**:
- Test with rapid WASD sequences (should show 0 intermediate renders)
- Test with 100ms pauses (should trigger debounce correctly)
- Test cancellation by pressing key during preview render
- Measure preview render time to ensure <200ms start time

**Alternatives Considered**:
- Fixed 100ms: Too aggressive, triggers on normal typing rhythm
- Fixed 300ms: Too slow, feels unresponsive
- Adaptive delay based on input velocity: Over-engineered for initial version

**42 School Compliance**: ✅ Simple configuration struct, no external dependencies

---

## Summary of Resolved Unknowns

| Unknown | Resolution | 42 School Compliant |
|---------|------------|---------------------|
| Timer implementation | `gettimeofday()` + polling | ✅ Yes |
| Render cancellation | State flag + scanline checks | ✅ Yes |
| State machine design | 4-state enum | ✅ Yes |
| Event loop integration | `mlx_loop_hook()` enhancement | ✅ Yes |
| Debounce delay tuning | 150ms default | ✅ Yes |

## Implementation Risk Assessment

**Low Risk**:
- Timer implementation (proven pattern, simple logic)
- State machine (straightforward enum, clear transitions)
- Configuration (compile-time constants)

**Medium Risk**:
- Event loop integration (must not break existing render loop behavior)
- Render cancellation (must test that partial renders don't corrupt state)

**Mitigation Strategy**:
- Incremental implementation: Timer → State machine → Integration → Testing
- Preserve existing behavior: Only activate debouncing for movement keys
- Feature flag: Can disable debouncing via config if issues arise

## Next Steps (Phase 1)

1. Design data model for debounce state structures
2. Define API contracts for debounce functions
3. Create quickstart guide for using debounced rendering
4. Update agent context with debounce architecture
