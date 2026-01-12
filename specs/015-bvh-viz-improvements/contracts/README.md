# API Contracts: BVH Tree Visualization Improvements

**Feature**: 015-bvh-viz-improvements  
**Date**: 2026-01-12  
**Version**: 1.0.0

## Overview

This directory contains API contracts (header specifications) for the BVH tree visualization improvements feature. These contracts define the public interfaces for:

1. **Object Identifier Formatting** (`identifier_api.h`)
2. **Keyboard Debounce System** (`debounce_api.h`)

## Purpose

API contracts serve as:
- **Design specification**: Documents expected behavior before implementation
- **Contract testing**: Enables validation of implementations against specification
- **Integration guide**: Shows other developers how to use the APIs
- **Maintenance reference**: Provides canonical documentation of guarantees and constraints

## Contract Format

Each header file follows this structure:

```c
/**
 * API Contract: [Name]
 * Feature: 015-bvh-viz-improvements
 * Version: 1.0.0
 *
 * Purpose:
 *   [Brief description]
 *
 * Contract Guarantees:
 *   - [List of invariants and guarantees]
 *
 * 42 School Constraints:
 *   - [Compliance requirements]
 */

/* Function declarations with detailed documentation */
```

### Documentation Standards

Each function includes:
- `@brief`: One-line description
- `@param`: Parameter descriptions with types
- `@return`: Return value semantics
- `@pre`: Preconditions (caller requirements)
- `@post`: Postconditions (function guarantees)
- **Thread Safety**: Concurrency behavior
- **Complexity**: Time and space complexity

## Files

### 1. identifier_api.h

**Purpose**: Unified object identifier formatting (sp-1, pl-2, cy-3)

**Key Functions**:
- `format_object_id()`: Format identifier into buffer
- `get_type_prefix()`: Get lowercase type prefix string
- `index_to_display_number()`: Convert 0-based to 1-based numbering

**Guarantees**:
- Consistent lowercase format across all displays
- 1-based sequential numbering per object type
- No dynamic allocation (caller provides buffer)
- Thread-safe (no shared state)

**Usage Example**:
```c
char id[16];
int len = format_object_id(id, OBJ_SPHERE, 0);
printf("Object: %s\n", id);  // Output: Object: sp-1
```

### 2. debounce_api.h

**Purpose**: 1-second keyboard debounce with state machine

**Key Functions**:
- `init_timer_pool()`: Initialize timer pool structure
- `get_timer()`: Access timer by ID
- `is_timer_expired()`: Check if 1 second elapsed
- `reset_timer()`: Reset countdown on new input
- `start_rendering()` / `finish_rendering()`: State transitions
- `enqueue_input()` / `dequeue_input()`: Input queue operations
- `process_input_queue()`: Flush queued inputs after render

**Guarantees**:
- Microsecond precision timing (gettimeofday)
- State machine prevents race conditions
- FIFO input queue preserves order
- Fixed allocation (no malloc/free)
- Single-threaded event loop compatible

**Usage Example**:
```c
/* In handle_key() */
t_debounce_timer *timer = get_timer(&render->timer_pool, TIMER_NAVIGATION);
if (timer->state == TIMER_RENDERING)
    enqueue_input(&render->timer_pool.input_queue, keycode);
else
{
    if (timer->state == TIMER_IDLE)
        timer->state = TIMER_PENDING;
    reset_timer(timer);
}

/* In render_loop() */
if (is_timer_expired(timer) && render->dirty)
{
    start_rendering(timer);
    render_scene(render);
    process_input_queue(&render->timer_pool.input_queue, render);
    finish_rendering(timer, render->timer_pool.input_queue.count > 0);
}
```

## 42 School Compliance

All contracts comply with 42 School miniRT constraints:

**Allowed Functions**:
- Standard C library: `gettimeofday`, `snprintf`, `memcpy`
- Math library: N/A (not required for these APIs)
- MiniLibX: N/A (contracts are data/logic only)

**Prohibited**:
- pthread: Not used (single-threaded state machine)
- fork/pipe: Not used (no multiprocessing)
- External libraries: None required

**Optimization Techniques**:
- Algorithmic: State machine, circular buffer
- Memory layout: Fixed stack-allocated structures
- Caching: Pre-allocated timer pool avoids malloc overhead

## Testing Contracts

### Contract Validation

Each contract can be validated by:

1. **Pre/Post Condition Testing**: Verify function satisfies documented guarantees
2. **State Machine Testing**: Verify valid transitions, detect invalid transitions
3. **Boundary Testing**: Test buffer sizes, queue limits, timer overflow
4. **Integration Testing**: Verify contracts compose correctly

### Example Test Cases

**Identifier Format** (`identifier_api.h`):
```c
// Test: format_object_id() produces correct format
char buf[16];
assert(format_object_id(buf, OBJ_SPHERE, 0) == 4);
assert(strcmp(buf, "sp-1") == 0);

// Test: 1-based numbering
assert(format_object_id(buf, OBJ_PLANE, 5) == 4);
assert(strcmp(buf, "pl-6") == 0);

// Test: boundary (3-digit numbers)
assert(format_object_id(buf, OBJ_CYLINDER, 99) == 6);
assert(strcmp(buf, "cy-100") == 0);
```

**Debounce Timer** (`debounce_api.h`):
```c
// Test: timer expiration after 1 second
t_debounce_timer timer = {0, TIMER_IDLE, 0};
timer.state = TIMER_PENDING;
timer.start_usec = get_timestamp_usec();

usleep(500000);  // Wait 0.5 seconds
assert(is_timer_expired(&timer) == 0);  // Not expired

usleep(500000);  // Wait another 0.5 seconds (total 1.0s)
assert(is_timer_expired(&timer) == 1);  // Expired

// Test: input queue FIFO order
t_input_queue queue = {0};
enqueue_input(&queue, KEY_W);
enqueue_input(&queue, KEY_A);
t_input_event event;
assert(dequeue_input(&queue, &event) == 1);
assert(event.keycode == KEY_W);  // First in, first out
```

## Implementation Checklist

When implementing these contracts:

- [ ] Create `src/utils/format_object_id.c` implementing `identifier_api.h`
- [ ] Create `src/window/window_debounce.c` implementing `debounce_api.h`
- [ ] Add corresponding header in `includes/` directory
- [ ] Update `includes/window.h` to embed `t_timer_pool` in `t_render`
- [ ] Update `src/hud/hud_obj_render.c` to use `format_object_id()`
- [ ] Update `src/bvh_vis/bvh_vis_node.c` to use `format_object_id()`
- [ ] Update `src/window/window_events.c` to use debounce API
- [ ] Update `src/window/window_loop.c` to check timer expiration
- [ ] Write unit tests for identifier formatting
- [ ] Write integration tests for debounce timing
- [ ] Run norminette on all new files
- [ ] Run valgrind to verify zero leaks

## References

- **Feature Specification**: `../spec.md`
- **Research Document**: `../research.md`
- **Data Model**: `../data-model.md`
- **Quickstart Guide**: `../quickstart.md`
- **Implementation Plan**: `../plan.md`

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2026-01-12 | Initial API contracts |

---

**Next Steps**: Implement contracts in source code, then run contract validation tests.
