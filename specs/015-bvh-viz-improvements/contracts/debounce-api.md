# API Contract: Debounce Timer System

**Module**: Keyboard Input Debounce  
**Header**: `includes/window/window_debounce.h`  
**Implementation**: `src/window/window_debounce.c`

---

## Function: init_timer_pool

### Signature

```c
void init_timer_pool(t_timer_pool *pool);
```

### Purpose

Initialize the timer pool structure with all timers in IDLE state and empty input queue.

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| pool | t_timer_pool* | in/out | Timer pool to initialize |

### Preconditions

- `pool` must not be NULL
- Called once during window initialization

### Postconditions

- All timers in pool have state = TIMER_IDLE
- All timers have start_usec = 0, accumulated = 0
- Input queue is empty (head = tail = count = 0)

### Example

```c
t_render render;

init_timer_pool(&render.timer_pool);
/* All timers ready for use */
```

---

## Function: get_timer

### Signature

```c
t_debounce_timer *get_timer(t_timer_pool *pool, t_timer_id id);
```

### Purpose

Retrieve a specific timer from the pool by ID.

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| pool | t_timer_pool* | in | Timer pool |
| id | t_timer_id | in | Timer identifier (TIMER_NAVIGATION, TIMER_HUD, etc.) |

### Return Value

| Value | Meaning |
|-------|---------|
| Non-NULL | Pointer to requested timer |
| NULL | Invalid timer ID |

### Example

```c
t_debounce_timer *nav_timer;

nav_timer = get_timer(&render.timer_pool, TIMER_NAVIGATION);
if (nav_timer != NULL)
    reset_timer(nav_timer);
```

---

## Function: reset_timer

### Signature

```c
void reset_timer(t_debounce_timer *timer);
```

### Purpose

Start or reset the 1-second countdown timer, incrementing the accumulated input count.

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| timer | t_debounce_timer* | in/out | Timer to reset |

### Preconditions

- `timer` must not be NULL
- `timer->state` must be TIMER_IDLE or TIMER_PENDING

### Postconditions

- `timer->start_usec` = current timestamp (microseconds)
- `timer->accumulated` incremented by 1
- If previously IDLE, `timer->state` = TIMER_PENDING

### Behavior

```c
/* First call: Start timer */
reset_timer(timer);  // state: IDLE → PENDING, accumulated: 0 → 1

/* Subsequent calls within 1 second: Reset countdown */
reset_timer(timer);  // state: PENDING, accumulated: 1 → 2
reset_timer(timer);  // state: PENDING, accumulated: 2 → 3
```

---

## Function: is_timer_expired

### Signature

```c
int is_timer_expired(t_debounce_timer *timer);
```

### Purpose

Check if 1 second has elapsed since the timer was last reset.

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| timer | t_debounce_timer* | in | Timer to check |

### Return Value

| Value | Meaning |
|-------|---------|
| 1 | Timer expired (≥1,000,000 microseconds elapsed) |
| 0 | Timer not expired or not active |

### Preconditions

- `timer` must not be NULL

### Example

```c
t_debounce_timer *timer = get_timer(&pool, TIMER_NAVIGATION);

if (is_timer_expired(timer))
{
    /* Execute debounced action */
    render_scene(render);
}
```

### Timing Precision

- Checks elapsed time: `current_time - timer->start_usec >= 1,000,000`
- Resolution: Microsecond (±10ms tolerance in practice)
- Called every ~16ms in render loop (60 fps)

---

## Function: start_rendering

### Signature

```c
void start_rendering(t_debounce_timer *timer);
```

### Purpose

Transition timer to RENDERING state, indicating render operation in progress.

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| timer | t_debounce_timer* | in/out | Timer to transition |

### Preconditions

- `timer->state` must be TIMER_PENDING
- Timer must have expired (is_timer_expired returned 1)

### Postconditions

- `timer->state` = TIMER_RENDERING

### Example

```c
if (is_timer_expired(timer))
{
    start_rendering(timer);
    render_scene(render);  /* Perform actual render */
}
```

---

## Function: finish_rendering

### Signature

```c
void finish_rendering(t_debounce_timer *timer, int has_queued_inputs);
```

### Purpose

Transition timer out of RENDERING state, either to IDLE or back to PENDING if inputs queued.

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| timer | t_debounce_timer* | in/out | Timer to transition |
| has_queued_inputs | int | in | 1 if input queue has events, 0 otherwise |

### Preconditions

- `timer->state` must be TIMER_RENDERING

### Postconditions

- If `has_queued_inputs == 0`: `timer->state` = TIMER_IDLE, `accumulated` = 0
- If `has_queued_inputs == 1`: `timer->state` = TIMER_PENDING, `start_usec` reset

### Example

```c
finish_rendering(timer);

/* Process queued inputs */
process_input_queue(&pool.input_queue, render);

/* Determine next state */
if (pool.input_queue.count > 0)
    finish_rendering(timer, 1);  /* Restart timer */
else
    finish_rendering(timer, 0);  /* Go idle */
```

---

## Function: enqueue_input

### Signature

```c
int enqueue_input(t_input_queue *queue, int keycode);
```

### Purpose

Add a keyboard event to the input queue (called when timer is in RENDERING state).

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| queue | t_input_queue* | in/out | Input queue |
| keycode | int | in | MLX/X11 keyboard code |

### Return Value

| Value | Meaning |
|-------|---------|
| 1 | Event successfully enqueued |
| 0 | Queue full (16 events), event dropped |

### Preconditions

- `queue` must not be NULL
- `keycode` must be valid keyboard code

### Postconditions

- Event added to queue at `head` position
- `head` advanced (with wraparound)
- `count` incremented (if not full)
- Event timestamp set to current time

### Example

```c
if (timer->state == TIMER_RENDERING)
{
    if (!enqueue_input(&pool.input_queue, keycode))
        printf("Warning: Input queue full, event dropped\n");
}
```

---

## Function: dequeue_input

### Signature

```c
int dequeue_input(t_input_queue *queue, t_input_event *event);
```

### Purpose

Remove and return the oldest event from the input queue (FIFO).

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| queue | t_input_queue* | in/out | Input queue |
| event | t_input_event* | out | Destination for dequeued event |

### Return Value

| Value | Meaning |
|-------|---------|
| 1 | Event successfully dequeued |
| 0 | Queue empty |

### Preconditions

- `queue` and `event` must not be NULL

### Postconditions

- If queue not empty: Event copied to `*event`, `tail` advanced, `count` decremented
- If queue empty: No changes

### Example

```c
t_input_event event;

while (dequeue_input(&pool.input_queue, &event))
{
    handle_navigation_key(render, event.keycode);
}
```

---

## Function: process_input_queue

### Signature

```c
void process_input_queue(t_input_queue *queue, t_render *render);
```

### Purpose

Drain the input queue and process all accumulated keyboard events.

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| queue | t_input_queue* | in/out | Input queue to drain |
| render | t_render* | in/out | Render context for event handling |

### Preconditions

- Render operation must be complete (timer transitioned from RENDERING)

### Postconditions

- All queued events processed via handle_navigation_key
- Queue is empty (`count` = 0)

### Example

```c
/* After render completes */
process_input_queue(&render->timer_pool.input_queue, render);

/* Queue now empty, ready for new inputs */
```

---

## Function: get_timestamp_usec

### Signature

```c
long get_timestamp_usec(void);
```

### Purpose

Get current time in microseconds since Unix epoch.

### Return Value

| Value | Meaning |
|-------|---------|
| n > 0 | Current timestamp in microseconds |

### Implementation

```c
long get_timestamp_usec(void)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000L + tv.tv_usec);
}
```

### Thread Safety

- Function is thread-safe (gettimeofday is reentrant)

---

## State Machine Flow

```text
┌─────────────────────────────────────────────────────────────┐
│                     TIMER LIFECYCLE                          │
└─────────────────────────────────────────────────────────────┘

IDLE
  ↓ [handle_key: navigation input]
  ↓ reset_timer() → state = PENDING
PENDING
  ↓ [timer check in render_loop]
  ↓ is_timer_expired() returns 1
  ↓ start_rendering()
RENDERING
  ↓ [render operation completes]
  ↓ finish_rendering(has_queued=0)
IDLE

┌─────────────────────────────────────────────────────────────┐
│              TIMER RESET (PENDING → PENDING)                 │
└─────────────────────────────────────────────────────────────┘

PENDING (accumulated=1, timer at 0.5s)
  ↓ [handle_key: another input]
  ↓ reset_timer() → restart countdown
PENDING (accumulated=2, timer at 0.0s)

┌─────────────────────────────────────────────────────────────┐
│       INPUT QUEUE (RENDERING → PENDING)                      │
└─────────────────────────────────────────────────────────────┘

RENDERING
  ↓ [handle_key during render]
  ↓ enqueue_input(queue, keycode)
  ↓ [render completes]
  ↓ process_input_queue()
  ↓ finish_rendering(has_queued=1)
PENDING (accumulated=N, timer restarted)
```

---

## Integration Example

### Complete Workflow

```c
/* In window_events.c */
int handle_key(int keycode, void *param)
{
    t_render *render = (t_render *)param;
    t_debounce_timer *timer;
    
    if (is_navigation_key(keycode))
    {
        timer = get_timer(&render->timer_pool, TIMER_NAVIGATION);
        
        if (timer->state == TIMER_RENDERING)
        {
            /* Render in progress, queue input */
            enqueue_input(&render->timer_pool.input_queue, keycode);
        }
        else
        {
            /* Start or reset timer */
            if (timer->state == TIMER_IDLE)
                timer->state = TIMER_PENDING;
            reset_timer(timer);
        }
        
        /* Mark scene dirty for eventual render */
        render->dirty = 1;
    }
    
    return (0);
}

/* In window_loop.c */
int render_loop(void *param)
{
    t_render *render = (t_render *)param;
    t_debounce_timer *timer;
    int has_queued;
    
    timer = get_timer(&render->timer_pool, TIMER_NAVIGATION);
    
    /* Check if debounce timer expired */
    if (is_timer_expired(timer) && render->dirty)
    {
        start_rendering(timer);
        
        /* Execute render */
        render_scene_internal(render);
        render->dirty = 0;
        
        /* Process queued inputs */
        process_input_queue(&render->timer_pool.input_queue, render);
        
        /* Update timer state */
        has_queued = (render->timer_pool.input_queue.count > 0);
        finish_rendering(timer, has_queued);
    }
    
    return (0);
}
```

---

## Testing Contract

### Unit Tests

```c
/* Test timer initialization */
void test_init_timer_pool(void)
{
    t_timer_pool pool;
    
    init_timer_pool(&pool);
    assert(pool.timers[0].state == TIMER_IDLE);
    assert(pool.input_queue.count == 0);
}

/* Test timer reset */
void test_reset_timer(void)
{
    t_debounce_timer timer = {0, TIMER_IDLE, 0};
    
    reset_timer(&timer);
    assert(timer.state == TIMER_PENDING);
    assert(timer.accumulated == 1);
    assert(timer.start_usec > 0);
}

/* Test timer expiration */
void test_timer_expired(void)
{
    t_debounce_timer timer;
    
    timer.state = TIMER_PENDING;
    timer.start_usec = get_timestamp_usec() - 1500000;  /* 1.5 seconds ago */
    
    assert(is_timer_expired(&timer) == 1);
}

/* Test input queue */
void test_input_queue(void)
{
    t_input_queue queue = {{}, 0, 0, 0};
    t_input_event event;
    int success;
    
    success = enqueue_input(&queue, KEY_W);
    assert(success == 1);
    assert(queue.count == 1);
    
    success = dequeue_input(&queue, &event);
    assert(success == 1);
    assert(event.keycode == KEY_W);
    assert(queue.count == 0);
}
```

---

## Dependencies

### Required Headers
- `<sys/time.h>` (for gettimeofday, struct timeval)
- `<string.h>` (for memset)
- `includes/window.h` (for t_render)

### Allowed Functions (42 School)
- `gettimeofday` (standard C library - time measurement)
- `memset` (standard C library - initialization)

### No Dependencies On
- pthread (explicitly prohibited)
- clock_gettime (not guaranteed available)
- External timer libraries

---

## Performance Characteristics

| Operation | Time | Space | Frequency |
|-----------|------|-------|-----------|
| is_timer_expired | O(1) | O(1) | ~60 times/sec (render loop) |
| reset_timer | O(1) | O(1) | Per keypress |
| enqueue_input | O(1) | O(1) | Per keypress during render |
| dequeue_input | O(1) | O(1) | Per queued event after render |
| process_input_queue | O(n) | O(1) | Once per render (n ≤ 16) |

**Total Overhead**: <0.1% CPU time (timer checks are trivial arithmetic)

---

## Versioning

- **Version**: 1.0.0
- **Date**: 2026-01-12
- **Stability**: Stable

---

## Notes

- All functions are **internal** to miniRT
- Timer pool size (4) is fixed; no dynamic resizing
- Input queue size (16) is fixed; overflow drops oldest events
- Timing precision: ±10ms acceptable for 1-second debounce
