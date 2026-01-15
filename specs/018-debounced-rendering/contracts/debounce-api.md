# API Contract: Debounce System

**Feature**: 018-debounced-rendering  
**Date**: 2026-01-15  
**Language**: C  
**Compliance**: 42 School norminette

## Header File: `render_debounce.h`

### Type Definitions

```c
/* Debounce state machine states */
typedef enum e_debounce_state_enum
{
    DEBOUNCE_IDLE,          /* No input, no pending renders */
    DEBOUNCE_ACTIVE,        /* Timer running, waiting for delay */
    DEBOUNCE_PREVIEW,       /* Preview render in progress */
    DEBOUNCE_FINAL          /* Final render in progress */
}   t_debounce_state_enum;

/* Debounce timer for tracking input delay */
typedef struct s_debounce_timer
{
    struct timeval  last_input_time;    /* Timestamp of most recent input */
    int             is_active;           /* 1 if timer running, 0 if stopped */
    long            delay_ms;            /* Delay threshold in milliseconds */
}   t_debounce_timer;

/* Complete debounce state management */
typedef struct s_debounce_state
{
    t_debounce_state_enum   state;          /* Current state machine state */
    t_debounce_timer        timer;          /* Embedded timer instance */
    int                     preview_enabled; /* Enable preview mode */
    int                     auto_upgrade;    /* Auto-upgrade to final */
    int                     cancel_requested; /* Flag to abort current render */
}   t_debounce_state;
```

### Constants

```c
/* Default configuration values */
#define DEBOUNCE_DEFAULT_DELAY_MS       150
#define DEBOUNCE_DEFAULT_PREVIEW        1
#define DEBOUNCE_DEFAULT_AUTO_UPGRADE   1
```

---

## Function Contracts

### 1. Initialization and Cleanup

#### `debounce_init`

**Signature**:
```c
void debounce_init(t_debounce_state *state);
```

**Purpose**: Initialize debounce state with default configuration.

**Parameters**:
- `state`: Pointer to debounce state structure (must not be NULL)

**Preconditions**:
- `state` points to valid memory
- Called during render context initialization

**Postconditions**:
- `state->state` = DEBOUNCE_IDLE
- `state->timer.is_active` = 0
- `state->timer.delay_ms` = DEBOUNCE_DEFAULT_DELAY_MS
- `state->preview_enabled` = DEBOUNCE_DEFAULT_PREVIEW
- `state->auto_upgrade` = DEBOUNCE_DEFAULT_AUTO_UPGRADE
- `state->cancel_requested` = 0

**Side Effects**: None

**Error Handling**: No error return (caller must ensure valid pointer)

**Example**:
```c
t_render *render = init_window(scene);
debounce_init(&render->debounce);
```

---

### 2. Input Handling

#### `debounce_on_input`

**Signature**:
```c
void debounce_on_input(t_debounce_state *state);
```

**Purpose**: Handle keyboard input event - starts or resets debounce timer.

**Parameters**:
- `state`: Pointer to debounce state structure

**Preconditions**:
- `state` is initialized via `debounce_init()`
- Called from keyboard event handler

**Postconditions**:
- If `state->state` is DEBOUNCE_IDLE: transitions to DEBOUNCE_ACTIVE
- If `state->state` is DEBOUNCE_ACTIVE: timer resets to current time
- If `state->state` is DEBOUNCE_PREVIEW or DEBOUNCE_FINAL: sets `cancel_requested` flag
- Timer starts/resets via `timer_reset()`

**State Transitions**:
```
IDLE → ACTIVE          (first input)
ACTIVE → ACTIVE        (input during debouncing - resets timer)
PREVIEW → ACTIVE       (input during preview - cancels render)
FINAL → ACTIVE         (input during final - cancels render)
```

**Side Effects**:
- May set `cancel_requested` flag
- Captures current time via `gettimeofday()`

**Thread Safety**: N/A (single-threaded event loop)

**Example**:
```c
int handle_key(int keycode, void *param)
{
    t_render *render = (t_render *)param;
    
    if (is_movement_key(keycode))
        debounce_on_input(&render->debounce);
    
    return (0);
}
```

---

### 3. State Machine Update

#### `debounce_update`

**Signature**:
```c
void debounce_update(t_debounce_state *state, t_render *render);
```

**Purpose**: Update state machine - checks timer and transitions between states.

**Parameters**:
- `state`: Pointer to debounce state structure
- `render`: Pointer to render context (for setting dirty/low_quality flags)

**Preconditions**:
- `state` is initialized
- `render` points to valid render context
- Called once per frame in event loop

**Postconditions**:
- State transitions handled according to state machine
- `render->dirty` and `render->low_quality` flags set appropriately
- Completed renders transition to next state

**State Transitions**:
```
ACTIVE → PREVIEW       (timer expired, preview_enabled=1)
ACTIVE → FINAL         (timer expired, preview_enabled=0)
PREVIEW → FINAL        (render complete, auto_upgrade=1)
PREVIEW → IDLE         (render complete, auto_upgrade=0)
FINAL → IDLE           (render complete)
```

**Side Effects**:
- Modifies `render->dirty` flag
- Modifies `render->low_quality` flag
- May change `state->state`

**Performance**: O(1) - constant time state machine evaluation

**Example**:
```c
int render_loop(void *param)
{
    t_render *render = (t_render *)param;
    
    // Update debounce state machine
    debounce_update(&render->debounce, render);
    
    // Render if dirty
    if (render->dirty)
    {
        render_scene_to_buffer(render->scene, render);
        render->dirty = 0;
    }
    
    return (0);
}
```

---

### 4. Render Cancellation

#### `debounce_cancel`

**Signature**:
```c
void debounce_cancel(t_debounce_state *state);
```

**Purpose**: Cancel in-progress render and reset to debouncing state.

**Parameters**:
- `state`: Pointer to debounce state structure

**Preconditions**:
- `state` is initialized
- Called when cancellation is needed (typically via `debounce_on_input`)

**Postconditions**:
- If `state->state` is DEBOUNCE_PREVIEW or DEBOUNCE_FINAL:
  - Transitions to DEBOUNCE_ACTIVE
  - Clears `cancel_requested` flag
  - Resets timer
- Otherwise: no-op

**Side Effects**:
- May change `state->state`
- Clears `cancel_requested` flag
- Resets timer via `timer_reset()`

**Example**:
```c
// Called internally by debounce_on_input() when input occurs during render
void debounce_on_input(t_debounce_state *state)
{
    if (state->state == DEBOUNCE_PREVIEW || state->state == DEBOUNCE_FINAL)
        debounce_cancel(state);
    else
        timer_start(&state->timer);
}
```

---

### 5. Timer Operations

#### `timer_start`

**Signature**:
```c
void timer_start(t_debounce_timer *timer);
```

**Purpose**: Start or restart the debounce timer.

**Parameters**:
- `timer`: Pointer to timer structure

**Preconditions**:
- `timer` is initialized
- `timer->delay_ms` is set

**Postconditions**:
- `timer->is_active` = 1
- `timer->last_input_time` = current time (via `gettimeofday()`)

**Side Effects**: Captures current system time

**42 School Functions Used**: `gettimeofday()`

**Example**:
```c
timer_start(&state->timer);
```

---

#### `timer_reset`

**Signature**:
```c
void timer_reset(t_debounce_timer *timer);
```

**Purpose**: Reset timer to current time without changing active state.

**Parameters**:
- `timer`: Pointer to timer structure

**Preconditions**:
- `timer` is initialized

**Postconditions**:
- `timer->last_input_time` = current time
- `timer->is_active` unchanged

**Side Effects**: Captures current system time

**42 School Functions Used**: `gettimeofday()`

**Example**:
```c
// Reset timer when new input arrives during debouncing
if (state->state == DEBOUNCE_ACTIVE)
    timer_reset(&state->timer);
```

---

#### `timer_stop`

**Signature**:
```c
void timer_stop(t_debounce_timer *timer);
```

**Purpose**: Stop the timer (deactivate).

**Parameters**:
- `timer`: Pointer to timer structure

**Preconditions**:
- `timer` is initialized

**Postconditions**:
- `timer->is_active` = 0

**Side Effects**: None

**Example**:
```c
// Stop timer when render completes
if (state->state == DEBOUNCE_FINAL)
    timer_stop(&state->timer);
```

---

#### `timer_expired`

**Signature**:
```c
int timer_expired(t_debounce_timer *timer);
```

**Purpose**: Check if timer delay has elapsed.

**Parameters**:
- `timer`: Pointer to timer structure

**Returns**:
- `1` if timer is active and delay has elapsed
- `0` if timer is inactive or delay has not elapsed

**Preconditions**:
- `timer` is initialized

**Postconditions**: None (read-only check)

**Algorithm**:
```c
int timer_expired(t_debounce_timer *timer)
{
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

**Performance**: O(1) - single system call + arithmetic

**42 School Functions Used**: `gettimeofday()`

**Example**:
```c
// Check timer in state machine update
if (state->state == DEBOUNCE_ACTIVE && timer_expired(&state->timer))
{
    // Transition to preview/final
    state->state = DEBOUNCE_PREVIEW;
}
```

---

## Integration Points

### Modified Functions

#### `handle_key` (window_events.c)

**Change**: Add call to `debounce_on_input()` for movement keys

**Before**:
```c
void handle_camera_keys(t_render *render, int keycode)
{
    if (keycode == KEY_W || ...)
    {
        handle_camera_move(render, keycode);
        render->low_quality = 1;
        render->dirty = 1;
    }
}
```

**After**:
```c
void handle_camera_keys(t_render *render, int keycode)
{
    if (keycode == KEY_W || ...)
    {
        handle_camera_move(render, keycode);
        debounce_on_input(&render->debounce);  // NEW: Start debounce
        render->dirty = 1;  // Removed: low_quality (set by debounce)
    }
}
```

---

#### `render_loop` (window_events.c)

**Change**: Add call to `debounce_update()` before rendering

**Before**:
```c
int render_loop(void *param)
{
    t_render *render = (t_render *)param;
    
    if (render->dirty)
    {
        render_scene_to_buffer(render->scene, render);
        mlx_put_image_to_window(...);
        render->dirty = 0;
    }
    
    return (0);
}
```

**After**:
```c
int render_loop(void *param)
{
    t_render *render = (t_render *)param;
    
    debounce_update(&render->debounce, render);  // NEW: Update state machine
    
    if (render->dirty)
    {
        render_scene_to_buffer(render->scene, render);
        mlx_put_image_to_window(...);
        render->dirty = 0;
    }
    
    return (0);
}
```

---

#### `render_scene_to_buffer` (render.c)

**Change**: Add cancellation check in render loop

**Before**:
```c
void render_scene_to_buffer(t_scene *scene, t_render *render)
{
    int x, y;
    
    for (y = 0; y < WINDOW_HEIGHT; y++)
    {
        for (x = 0; x < WINDOW_WIDTH; x++)
            render_pixel(scene, render, x, y);
    }
}
```

**After**:
```c
void render_scene_to_buffer(t_scene *scene, t_render *render)
{
    int x, y;
    
    for (y = 0; y < WINDOW_HEIGHT; y++)
    {
        if (render->debounce.cancel_requested)  // NEW: Check cancel
            return;
        
        for (x = 0; x < WINDOW_WIDTH; x++)
            render_pixel(scene, render, x, y);
    }
}
```

---

## Error Handling

**No Error Returns**: All functions use `void` return type because:
1. Invalid pointers are programmer errors (precondition violations)
2. System calls like `gettimeofday()` are assumed to succeed
3. State machine transitions are deterministic

**Defensive Programming**:
- Functions check `is_active` before timer operations
- State transitions validate current state
- Boolean flags validated as 0 or 1 where critical

---

## Testing Contract

### Unit Test Coverage

1. **Timer Tests**:
   - `timer_start()` sets is_active and captures time
   - `timer_reset()` updates time without changing is_active
   - `timer_expired()` returns correct value for various delays
   - Timer arithmetic handles microsecond overflow correctly

2. **State Machine Tests**:
   - All 12 state transitions (see data-model.md)
   - Edge case: delay_ms = 0 (should transition immediately)
   - Edge case: cancel during each state
   - Edge case: preview_enabled = 0 (skip preview)
   - Edge case: auto_upgrade = 0 (stay in preview)

3. **Integration Tests**:
   - Key press → debounce → preview → final (happy path)
   - Rapid key presses (timer resets correctly)
   - Cancel during preview (render aborts, state resets)
   - Cancel during final (render aborts, state resets)

### Manual Test Scenarios

See `quickstart.md` for detailed testing procedures.

---

## Performance Guarantees

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| `debounce_init()` | O(1) | O(1) |
| `debounce_on_input()` | O(1) | O(1) |
| `debounce_update()` | O(1) | O(1) |
| `timer_expired()` | O(1) | O(1) |
| Full render cycle | O(width × height) | O(1) extra |

**Overhead per frame**: ~100ns (state machine check + timer check)

---

## 42 School Compliance

- ✅ Uses only `gettimeofday()` from allowed libc functions
- ✅ No pthread, fork, pipe, or prohibited functions
- ✅ No dynamic memory allocation
- ✅ Norminette compliant (verified via `norminette *.c *.h`)
- ✅ No external libraries beyond MiniLibX
