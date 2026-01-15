# Implementation Summary: Debounced Rendering

## Overview

This document provides a high-level summary of the debounced rendering implementation plan for miniRT.

## Problem Statement

Users experience stuttering and lag when rapidly navigating the scene with WASD keys because the system buffers multiple render requests, creating a queue of intermediate renders that never complete.

## Solution Approach

Implement a debounced rendering mechanism with automatic preview-to-final quality upgrade:

1. **Defer rendering** during continuous keyboard input
2. **Start debounce timer** (150ms) when input stops
3. **Render preview** (low quality) when timer expires
4. **Auto-upgrade** to final render when preview completes
5. **Cancel in-progress renders** if new input arrives

## Key Design Decisions

### 1. Timer Implementation
- **Choice**: `gettimeofday()` with event loop polling
- **Why**: 42 School compliant (no threads), microsecond precision sufficient
- **Alternative rejected**: pthread timers (prohibited by 42 constraints)

### 2. Render Cancellation
- **Choice**: Flag-based cancellation checked per scanline
- **Why**: Sub-frame latency (<16ms), no race conditions, simple implementation
- **Alternative rejected**: Signal-based interruption (too complex)

### 3. State Machine
- **Choice**: 4-state enum (IDLE → DEBOUNCING → PREVIEW → FINAL)
- **Why**: Clear transitions, easy to test, handles all edge cases
- **Alternative rejected**: 2-state machine (insufficient for preview upgrade)

### 4. Integration Strategy
- **Choice**: Minimal changes to existing code (enhance, don't replace)
- **Why**: Low risk, preserves working behavior, easy to revert
- **Alternative rejected**: Rewrite render loop (high risk, unnecessary)

## Implementation Files

### New Files (2)
- `src/render/render_debounce.c` - State machine and orchestration
- `includes/render_debounce.h` - Types and API declarations

### Modified Files (3)
- `src/window/window_events.c` - Add debounce_update() to render loop
- `src/window/window_key_handlers.c` - Call debounce_on_input() on movement keys
- `src/render/render.c` - Add cancellation check in render loop

### Documentation (4)
- `specs/018-debounced-rendering/research.md` - Technical research
- `specs/018-debounced-rendering/data-model.md` - Entity definitions
- `specs/018-debounced-rendering/contracts/debounce-api.md` - API contracts
- `specs/018-debounced-rendering/quickstart.md` - Testing guide

## Technical Specifications

### Data Structures

```c
// State machine states
typedef enum e_debounce_state_enum {
    DEBOUNCE_IDLE,      // No input, no pending renders
    DEBOUNCE_ACTIVE,    // Timer running, waiting for delay
    DEBOUNCE_PREVIEW,   // Preview render in progress
    DEBOUNCE_FINAL      // Final render in progress
} t_debounce_state_enum;

// Timer structure
typedef struct s_debounce_timer {
    struct timeval  last_input_time;
    int             is_active;
    long            delay_ms;
} t_debounce_timer;

// Complete state
typedef struct s_debounce_state {
    t_debounce_state_enum   state;
    t_debounce_timer        timer;
    int                     preview_enabled;
    int                     auto_upgrade;
    int                     cancel_requested;
} t_debounce_state;
```

### Key Functions

1. **debounce_init()** - Initialize state machine
2. **debounce_on_input()** - Handle keyboard input (start/reset timer)
3. **debounce_update()** - Update state machine (called per frame)
4. **timer_expired()** - Check if debounce delay elapsed

### Configuration

- **Default delay**: 150ms (configurable at compile time)
- **Preview enabled**: Yes (can be disabled)
- **Auto-upgrade**: Yes (can be disabled)
- **Cancellation**: Always enabled

## State Transitions

```
┌──────────────────────────────────────────────────────────┐
│                    State Machine                          │
└──────────────────────────────────────────────────────────┘

IDLE ─────────────────────────────────────┐
  ▲                                        │
  │                                        │ Key press
  │ Final render complete                  │
  │                                        ▼
FINAL                                  DEBOUNCING
  ▲                                        │
  │                                        │ Timer expired
  │ Auto-upgrade & preview complete        │ (150ms)
  │                                        ▼
PREVIEW ◄──────────────────────────── (select next state)
                                          │
                         ┌────────────────┴────────────────┐
                         │                                 │
                    preview_enabled=1              preview_enabled=0
                         │                                 │
                         ▼                                 ▼
                      PREVIEW                           FINAL

Special transitions:
- Any state + Key press → DEBOUNCING (with cancellation if rendering)
```

## Integration Points

### 1. Event Loop (window_events.c)

**Before**:
```c
int render_loop(void *param) {
    if (render->dirty) {
        render_scene_to_buffer(scene, render);
        render->dirty = 0;
    }
}
```

**After**:
```c
int render_loop(void *param) {
    debounce_update(&render->debounce, render);  // NEW
    if (render->dirty) {
        render_scene_to_buffer(scene, render);
        render->dirty = 0;
    }
}
```

### 2. Key Handlers (window_key_handlers.c)

**Before**:
```c
void handle_camera_keys(t_render *render, int keycode) {
    handle_camera_move(render, keycode);
    render->low_quality = 1;  // Manual quality control
    render->dirty = 1;
}
```

**After**:
```c
void handle_camera_keys(t_render *render, int keycode) {
    handle_camera_move(render, keycode);
    debounce_on_input(&render->debounce);  // NEW: Delegate to debounce
    render->dirty = 1;
    // low_quality now set by state machine
}
```

### 3. Render Loop (render.c)

**Before**:
```c
void render_scene_to_buffer(t_scene *scene, t_render *render) {
    for (y = 0; y < WINDOW_HEIGHT; y++) {
        for (x = 0; x < WINDOW_WIDTH; x++)
            render_pixel(scene, render, x, y);
    }
}
```

**After**:
```c
void render_scene_to_buffer(t_scene *scene, t_render *render) {
    for (y = 0; y < WINDOW_HEIGHT; y++) {
        if (render->debounce.cancel_requested)  // NEW: Check cancel
            return;
        for (x = 0; x < WINDOW_WIDTH; x++)
            render_pixel(scene, render, x, y);
    }
}
```

## 42 School Compliance

✅ **Functions Used**:
- `gettimeofday()` (allowed libc)
- `mlx_loop_hook()` (MiniLibX)
- No pthread, fork, pipe, or prohibited functions

✅ **Memory Management**:
- Zero `malloc()` calls
- All structures embedded by value
- Guaranteed zero leaks

✅ **Optimizations**:
- Algorithmic: State machine defers unnecessary renders
- No SIMD, no threads, no external libraries

## Performance Characteristics

- **Overhead per frame**: ~100ns (state check + timer check)
- **Memory overhead**: +48 bytes per window (negligible)
- **Debounce latency**: 150ms (configurable)
- **Preview start time**: <200ms (target)
- **Cancellation latency**: <16ms (one frame at 60fps)

## Testing Strategy

### Manual Tests
1. Hold WASD → verify no intermediate renders
2. Release key → verify preview within 200ms
3. Preview complete → verify auto-upgrade to final
4. Key during render → verify immediate cancellation
5. Rapid keypresses → verify timer resets correctly

### Success Criteria
- Zero intermediate renders during continuous input (100% elimination)
- Preview starts within 200ms of input stop
- Automatic preview → final transition
- Cancellation within one frame (16ms)
- Smooth navigation with no buffering

## Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|-----------|
| Event loop integration breaks existing behavior | High | Incremental testing, feature flag |
| Partial renders corrupt screen | Medium | Cancellation check per scanline |
| Timer arithmetic overflow | Low | Use long for milliseconds, test edge cases |
| Preview → final transition visible | Low | Tuning delay, user acceptance testing |

## Rollback Plan

If issues arise:
1. Set `DEBOUNCE_DEFAULT_DELAY_MS = 0` (disables feature)
2. Restore old `handle_key_release()` logic
3. Feature compiles out cleanly with zero impact

## Next Steps

1. Run `/speckit.tasks` to generate task breakdown
2. Implement timer utilities
3. Implement state machine
4. Integrate with event loop
5. Test with various scenes
6. Create Korean documentation
7. Merge to main branch

## References

- Feature spec: `specs/018-debounced-rendering/spec.md`
- Implementation plan: `specs/018-debounced-rendering/plan.md`
- Research: `specs/018-debounced-rendering/research.md`
- Data model: `specs/018-debounced-rendering/data-model.md`
- API contracts: `specs/018-debounced-rendering/contracts/debounce-api.md`
- Testing guide: `specs/018-debounced-rendering/quickstart.md`
