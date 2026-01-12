# Quickstart Guide: BVH Tree Visualization Improvements

**Feature**: 015-bvh-viz-improvements  
**Date**: 2026-01-12  
**Audience**: Developers implementing or using the improved BVH visualization

---

## Overview

This feature adds two key improvements to miniRT's BVH tree visualization:

1. **Unified Object Identifiers**: Consistent lowercase format (sp-1, pl-2, cy-3) across HUD and BVH tree displays
2. **Debounced Keyboard Input**: 1-second delay prevents rendering stuttering during rapid navigation

---

## Quick Start (5 Minutes)

### 1. Build the Project

```bash
make
```

### 2. Load a Test Scene

```bash
./miniRT scenes/test_scene.rt
```

### 3. Verify Unified Identifiers

1. Press `H` to toggle the HUD display
2. Observe object identifiers (e.g., "sp-1: pos(0.00, 1.00, 5.00)")
3. Press `V` to toggle BVH tree visualization
4. Observe leaf nodes show matching identifiers (e.g., "Leaf [sp-1, pl-1]")

**Expected Result**: HUD and BVH tree use identical identifier format

### 4. Test Keyboard Debounce

1. With HUD or BVH visible, press arrow keys rapidly (10+ times in 2 seconds)
2. Observe: No stuttering during key presses
3. Observe: Single smooth render occurs 1 second after you stop pressing keys

**Expected Result**: Navigation feels responsive, final render shows accumulated changes

---

## Identifier Format Reference

### Format Specification

```text
Pattern:  [type]-[number]
Examples: sp-1, pl-2, cy-100

Components:
  - type:   Two lowercase letters
  - sep:    Hyphen character '-'
  - number: Sequential integer starting from 1
```

### Supported Object Types

| Object Type | Prefix | Example |
|-------------|--------|---------|
| Sphere      | sp     | sp-1, sp-2, sp-3 |
| Plane       | pl     | pl-1, pl-2 |
| Cylinder    | cy     | cy-1, cy-2, cy-3 |

### Numbering Rules

- **1-based**: First object of each type is #1 (not #0)
- **Sequential**: Numbers increment per type (sp-1, sp-2, sp-3)
- **Stable**: Identifiers remain constant during session (no re-numbering)
- **Independent**: Each type has separate numbering (sp-1 and pl-1 can coexist)

---

## Debounce System Usage

### How It Works

```text
┌─────────────────────────────────────────────────────┐
│          KEYBOARD DEBOUNCE TIMELINE                 │
└─────────────────────────────────────────────────────┘

Time:     0.0s    0.5s    1.0s    1.5s    2.5s
          ↓       ↓       ↓       ↓       ↓
Input:    [Key1]  [Key2]  [Key3]  [----]  [Render]
State:    START   RESET   RESET   WAIT    EXECUTE
Timer:    1.0s    1.0s    1.0s    0.5s    EXPIRED

Explanation:
- Key1 (0.0s): Start 1-second timer
- Key2 (0.5s): Reset timer to 1 second
- Key3 (1.0s): Reset timer to 1 second again
- No input for 1 second: Timer expires
- Render (2.5s): Execute render with accumulated changes
```

### Debounce Behavior

| Scenario | Behavior |
|----------|----------|
| Single key press | Render after 1 second |
| Rapid key presses (<1s apart) | Timer resets each time |
| Holding arrow key | Timer resets continuously |
| Release key | Render 1 second after release |
| Input during render | Event queued, processed after render |

### Which Keys Are Debounced?

**Debounced Keys** (navigation):
- Arrow keys (up, down, left, right)
- Camera movement (W, A, S, D, Q, E, etc.)
- Object transformations (R, T, F, G, V, B)
- View controls (Insert, Home, PgUp, Delete, End, PgDn)

**Not Debounced** (immediate):
- ESC (quit)
- H (HUD toggle)
- Tab (cycle objects)
- I (info toggle)
- Other UI controls

---

## Using the Identifier Formatting API

### Basic Usage

```c
#include "format_object_id.h"

char id[16];
int len;

/* Format a sphere at array index 0 */
len = format_object_id(id, OBJ_SPHERE, 0);
printf("Identifier: %s (length: %d)\n", id, len);
// Output: Identifier: sp-1 (length: 4)

/* Format a plane at array index 5 */
len = format_object_id(id, OBJ_PLANE, 5);
printf("Identifier: %s\n", id);
// Output: Identifier: pl-6
```

### HUD Display Integration

```c
/* In hud_obj_render.c */
void render_sphere_obj(t_render *render, int idx, int *y, int color)
{
    char buf[128];
    char id[16];
    int len;
    
    /* Get unified identifier */
    format_object_id(id, OBJ_SPHERE, idx);
    
    /* Build display string */
    len = 0;
    len += copy_str_to_buf(buf + len, id);
    buf[len++] = ':';
    buf[len++] = ' ';
    len += copy_str_to_buf(buf + len, "pos");
    hud_format_vec3(buf + len, render->scene->spheres[idx].center);
    
    /* Render to screen */
    mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
        *y, color, buf);
    *y += HUD_LINE_HEIGHT;
}
```

### BVH Leaf Node Integration

```c
/* In bvh_vis_node.c */
void format_object_list(t_object_ref *objects, int count, char *buffer)
{
    int i;
    char id[16];
    
    strcpy(buffer, "Objects: [");
    i = 0;
    while (i < count)
    {
        /* Get unified identifier */
        format_object_id(id, objects[i].type, objects[i].index);
        strcat(buffer, id);
        
        if (i < count - 1)
            strcat(buffer, ", ");
        i++;
    }
    strcat(buffer, "]");
}
```

---

## Using the Debounce Timer API

### Initialization

```c
/* In init_window() */
t_render *init_window(t_scene *scene)
{
    t_render *render;
    
    /* ... existing initialization ... */
    
    /* Initialize timer pool */
    init_timer_pool(&render->timer_pool);
    
    return (render);
}
```

### Keyboard Event Handling

```c
/* In handle_key() - window_events.c */
int handle_key(int keycode, void *param)
{
    t_render *render = (t_render *)param;
    t_debounce_timer *timer;
    
    /* Handle navigation keys with debounce */
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
        
        /* Apply input logic (camera movement, etc.) */
        handle_camera_keys(render, keycode);
        
        /* Mark for eventual render */
        render->dirty = 1;
    }
    
    return (0);
}
```

### Render Loop Integration

```c
/* In render_loop() - window_loop.c */
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
        
        /* Process queued inputs after render */
        process_input_queue(&render->timer_pool.input_queue, render);
        
        /* Transition timer state */
        has_queued = (render->timer_pool.input_queue.count > 0);
        finish_rendering(timer, has_queued);
    }
    
    return (0);
}
```

---

## Testing Your Implementation

### Manual Test Checklist

#### Identifier Format Tests

- [ ] Load scene with 3+ objects of each type
- [ ] Verify HUD shows: sp-1, sp-2, sp-3, pl-1, pl-2, cy-1, cy-2, ...
- [ ] Verify BVH tree leaf nodes show matching identifiers
- [ ] Cycle through HUD pages (Tab key)
- [ ] Expand/collapse BVH tree nodes (arrow keys)
- [ ] Confirm identifiers remain consistent across displays

#### Debounce Timing Tests

- [ ] Press single navigation key → Verify 1-second delay before render
- [ ] Press key twice 0.5s apart → Verify render occurs 1.5s after first press
- [ ] Hold arrow key for 2 seconds → Verify single render 1s after release
- [ ] Rapidly press arrow keys 10 times → Verify single render after last press
- [ ] Press non-navigation key (H) → Verify immediate effect (no debounce)

#### Input Queue Tests

- [ ] Start complex render (large scene)
- [ ] Press navigation keys during render
- [ ] Verify all inputs processed after render completes
- [ ] Verify final state reflects all accumulated changes

### Automated Test Commands

```bash
# Build and run with test scene
make && ./miniRT scenes/test_debounce.rt

# Run with memory leak check
make && valgrind --leak-check=full ./miniRT scenes/test_debounce.rt

# Run with norminette check
norminette src/window/window_debounce.c
norminette src/utils/format_object_id.c
```

---

## Troubleshooting

### Identifiers Don't Match Between HUD and BVH

**Symptom**: HUD shows "Sphere #1" while BVH shows "0"

**Solution**: Ensure both rendering paths call `format_object_id()`:
- Update `src/hud/hud_obj_render.c` functions
- Update `src/bvh_vis/bvh_vis_node.c::format_object_list()`

### Debounce Not Working (Immediate Render)

**Symptom**: Render occurs immediately on keypress, no 1-second delay

**Solution**: Verify timer check in render loop:
```c
/* In render_loop() */
if (is_timer_expired(timer) && render->dirty)  // Must check BOTH conditions
{
    /* ... render logic ... */
}
```

### Inputs Lost During Render

**Symptom**: Keypresses during render operation are ignored

**Solution**: Verify input queueing in `handle_key()`:
```c
if (timer->state == TIMER_RENDERING)
{
    enqueue_input(&render->timer_pool.input_queue, keycode);  // MUST enqueue
}
```

### Timer Never Expires

**Symptom**: Scene dirty, but render never executes

**Solution**: Verify `gettimeofday()` usage:
```c
long get_timestamp_usec(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000L + tv.tv_usec);  // Note: 1000000L (long)
}
```

### Rendering Stutters on macOS

**Symptom**: Debounce works on Linux but stutters on macOS

**Solution**: macOS MLX may have different event handling. Verify:
- `gettimeofday()` returns consistent values
- Render loop calls frequency (~60 fps)
- No conflicts with macOS event coalescing

---

## Performance Tips

### Optimizing Render Performance

```c
/* Use low-quality rendering during interaction */
if (timer->state == TIMER_PENDING)
    render->low_quality = 1;  /* Reduce samples/resolution */
else
    render->low_quality = 0;  /* Full quality after debounce */
```

### Tuning Debounce Delay

**Current**: 1 second (1,000,000 microseconds)

**To Change**: Modify check in `is_timer_expired()`:
```c
/* Shorter delay (500ms) */
return (elapsed_usec >= 500000);

/* Longer delay (2 seconds) */
return (elapsed_usec >= 2000000);
```

### Monitoring Timer Statistics

```c
/* Add debugging output in render_loop() */
if (timer->state == TIMER_PENDING)
{
    long elapsed = get_timestamp_usec() - timer->start_usec;
    printf("Timer pending: %ld ms, accumulated: %d inputs\n",
           elapsed / 1000, timer->accumulated);
}
```

---

## Further Reading

- **Feature Specification**: `specs/015-bvh-viz-improvements/spec.md`
- **Research Document**: `specs/015-bvh-viz-improvements/research.md`
- **Data Model**: `specs/015-bvh-viz-improvements/data-model.md`
- **API Contracts**: `specs/015-bvh-viz-improvements/contracts/`

---

## Support

For issues or questions:
1. Check existing code comments in `src/window/window_debounce.c`
2. Review integration tests in `tests/` directory
3. Consult constitution: `.specify/memory/constitution.md`

---

## Changelog

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2026-01-12 | Initial quickstart guide |

---

**Next Steps**: Proceed to Phase 2 (task breakdown) using `/speckit.tasks` command.
