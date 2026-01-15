# Quickstart Guide: Debounced Rendering

**Feature**: 018-debounced-rendering  
**Date**: 2026-01-15  
**Audience**: Developers and testers

## Overview

This guide explains how to use, test, and verify the debounced rendering feature. Debounced rendering eliminates stuttering during rapid keyboard input by deferring rendering until user interaction pauses, then automatically transitioning from preview to final quality.

---

## User Experience

### Before Debouncing

**Problem**: Holding down WASD keys causes buffered renders:
```
User: WWWWWWWW [holds W for 2 seconds]
System: Render1 → Render2 → Render3 → ... → Render20
Result: Stuttering, lag, queued renders
```

### After Debouncing

**Solution**: Renders deferred until input stops:
```
User: WWWWWWWW [holds W for 2 seconds] → [releases]
System: [waits 150ms] → Preview render → Final render
Result: Smooth navigation, no lag
```

---

## Key Behaviors

### 1. Continuous Input → No Rendering

**What happens**: While holding any movement key (WASD, arrow keys, etc.), no rendering occurs.

**Expected behavior**:
- Screen shows last rendered frame
- No flicker or stutter
- UI remains responsive

**Test**: Hold 'W' for 3 seconds → verify no screen updates during hold.

---

### 2. Input Stops → Preview Render

**What happens**: 150ms after releasing keys, a fast preview render displays.

**Expected behavior**:
- Preview starts within 200ms of key release
- Uses low-quality mode (2x2 pixel blocks)
- Renders quickly (~50-100ms depending on scene)

**Test**: Release 'W' → verify preview appears within 200ms.

---

### 3. Preview Complete → Final Render

**What happens**: After preview finishes, high-quality render automatically starts.

**Expected behavior**:
- Seamless transition (no visible "jump")
- Final render uses full resolution
- Takes longer than preview (~200-500ms)

**Test**: Watch preview → final transition → verify smooth upgrade.

---

### 4. New Input During Render → Cancellation

**What happens**: Pressing any key during preview or final render cancels it immediately.

**Expected behavior**:
- Render aborts within 1 frame (16ms)
- Debounce timer resets
- New debounce cycle begins

**Test**: Press 'W' → wait 100ms → press 'A' → verify preview cancels and resets.

---

## Configuration

### Compile-Time Settings

Edit `includes/render_debounce.h`:

```c
/* Default debounce delay in milliseconds */
#define DEBOUNCE_DEFAULT_DELAY_MS       150

/* Enable preview rendering (1 = enabled, 0 = disabled) */
#define DEBOUNCE_DEFAULT_PREVIEW        1

/* Auto-upgrade from preview to final (1 = enabled, 0 = disabled) */
#define DEBOUNCE_DEFAULT_AUTO_UPGRADE   1
```

**Rebuild after changes**:
```bash
make re
```

---

### Runtime Behavior Modes

#### Mode 1: Standard (default)

```c
preview_enabled = 1
auto_upgrade = 1
```

**Behavior**: Input → Debounce → Preview → Final
**Use case**: Normal usage, best user experience

---

#### Mode 2: Preview Only

```c
preview_enabled = 1
auto_upgrade = 0
```

**Behavior**: Input → Debounce → Preview (stays in preview)
**Use case**: Performance testing, fast navigation

---

#### Mode 3: Direct to Final

```c
preview_enabled = 0
auto_upgrade = 1  // (ignored when preview disabled)
```

**Behavior**: Input → Debounce → Final (skip preview)
**Use case**: High-quality only, slower but no quality transition

---

#### Mode 4: No Debouncing

```c
delay_ms = 0
```

**Behavior**: Immediate render on key release (legacy behavior)
**Use case**: Debugging, comparing with old behavior

---

## Testing Procedures

### Test 1: Basic Debouncing

**Goal**: Verify no rendering during continuous input

**Steps**:
1. Launch miniRT: `./miniRT scenes/test.rt`
2. Hold down 'W' key for 3 seconds
3. Observe screen

**Expected**:
- No screen updates during hold
- Camera position does not advance frame-by-frame
- No flickering or stuttering

**Pass criteria**: Zero intermediate renders during 3-second hold

---

### Test 2: Preview Timing

**Goal**: Verify preview renders within 200ms

**Steps**:
1. Launch miniRT with timer output enabled
2. Press and release 'W' once
3. Measure time until preview appears

**Expected**:
- Preview starts 150-200ms after key release
- Low-quality render (blocky)
- Completes quickly (~50-100ms)

**Pass criteria**: Preview start time < 200ms after input stop

---

### Test 3: Auto-Upgrade

**Goal**: Verify preview → final transition

**Steps**:
1. Press and release 'W'
2. Wait for preview to complete
3. Observe automatic upgrade to final

**Expected**:
- Preview renders first (blocky)
- ~100ms pause
- Final render starts (smooth)
- No user interaction required

**Pass criteria**: Automatic transition without input

---

### Test 4: Cancellation

**Goal**: Verify render cancellation on new input

**Steps**:
1. Press 'W' and release
2. Wait 100ms (during preview render)
3. Press 'A'

**Expected**:
- Preview render aborts immediately
- New debounce cycle starts
- No leftover partial render

**Pass criteria**: Cancellation within 1 frame (16ms)

---

### Test 5: Rapid Input

**Goal**: Verify timer resets correctly

**Steps**:
1. Press WASD keys rapidly: W → A → S → D → W → A (< 100ms between)
2. Stop all input
3. Observe single preview → final cycle

**Expected**:
- No renders during rapid input
- Timer resets with each key press
- Only one render cycle after input stops

**Pass criteria**: Exactly one preview + one final render

---

### Test 6: Edge Case - Zero Delay

**Goal**: Verify behavior with delay_ms = 0

**Steps**:
1. Set `DEBOUNCE_DEFAULT_DELAY_MS` to 0
2. Rebuild and run
3. Press and release 'W'

**Expected**:
- Render starts immediately on key release (no debouncing)
- Behaves like old system

**Pass criteria**: Immediate render, no delay

---

## Debugging

### Enable Debug Output

Add to `render_debounce.c`:

```c
void debounce_update(t_debounce_state *state, t_render *render)
{
    // Debug: Print state transitions
    static t_debounce_state_enum prev_state = DEBOUNCE_IDLE;
    if (state->state != prev_state)
    {
        printf("Debounce: %d → %d\n", prev_state, state->state);
        prev_state = state->state;
    }
    
    // ... rest of function
}
```

**Output example**:
```
Debounce: 0 → 1    (IDLE → ACTIVE)
Debounce: 1 → 2    (ACTIVE → PREVIEW)
Debounce: 2 → 3    (PREVIEW → FINAL)
Debounce: 3 → 0    (FINAL → IDLE)
```

---

### Common Issues

#### Issue: Preview never appears

**Symptoms**: Key release does not trigger render

**Diagnosis**:
- Check `debounce_on_input()` is called in key handlers
- Verify `debounce_update()` is called in render loop
- Check `delay_ms` is not too large

**Fix**: Ensure integration points are correct (see `contracts/debounce-api.md`)

---

#### Issue: Render never completes

**Symptoms**: Stuck in PREVIEW or FINAL state

**Diagnosis**:
- Check `render->dirty` flag is cleared after render
- Verify state transitions in `debounce_update()`

**Fix**: Ensure `render->dirty = 0` after rendering

---

#### Issue: Cancellation doesn't work

**Symptoms**: Render continues despite new input

**Diagnosis**:
- Check `cancel_requested` flag is set in `debounce_on_input()`
- Verify cancellation check in `render_scene_to_buffer()`

**Fix**: Add cancellation check at each scanline:
```c
if (render->debounce.cancel_requested)
    return;
```

---

## Performance Tuning

### Delay Tuning

| Delay (ms) | User Experience | Use Case |
|------------|----------------|----------|
| 0 | Immediate, no debouncing | Debugging |
| 50-100 | Very responsive, may trigger on pauses | Fast navigation |
| 150-200 | Balanced (recommended) | General use |
| 300-500 | Slower, waits for deliberate pause | Precise control |

**Recommendation**: Start with 150ms, adjust based on user feedback.

---

### Preview Quality

Current implementation uses 2x2 pixel blocks (4x speedup).

**To increase preview quality** (slower but smoother):
- Change `x += 2` to `x += 1` in `render_low_quality()`
- No debounce changes needed

**To decrease preview quality** (faster but blockier):
- Change `x += 2` to `x += 4` (16x speedup)
- Useful for complex scenes

---

## Integration Checklist

### For New Features

If adding new input types (e.g., rotation keys):

1. ✅ Call `debounce_on_input()` in key handler
2. ✅ Do NOT manually set `render->low_quality`
3. ✅ Set `render->dirty = 1` after state change
4. ✅ Ensure render loop checks cancellation flag

**Example**:
```c
void handle_rotation_keys(t_render *render, int keycode)
{
    if (keycode == KEY_LEFT || keycode == KEY_RIGHT)
    {
        rotate_camera(render, keycode);
        debounce_on_input(&render->debounce);  // Add this
        render->dirty = 1;                      // Add this
        // Do NOT set render->low_quality here
    }
}
```

---

### For Code Review

**Checklist before merge**:

- ✅ Norminette passes: `norminette src/render/render_debounce*.c includes/render_debounce.h`
- ✅ Build succeeds: `make re`
- ✅ Memory check passes: `valgrind --leak-check=full ./miniRT scenes/test.rt`
- ✅ All manual tests pass (Tests 1-6 above)
- ✅ No pthread or prohibited functions used
- ✅ Code comments in English with Doxygen format
- ✅ Korean specification synchronized in `docs/specs/018-debounced-rendering/`

---

## Rollback Plan

If debouncing causes issues:

### Disable at Compile Time

```c
// In render_debounce.h
#define DEBOUNCE_DEFAULT_DELAY_MS   0  // Disables debouncing
```

### Disable at Runtime (future enhancement)

```c
render->debounce.preview_enabled = 0;
render->debounce.timer.delay_ms = 0;
```

### Revert to Legacy Behavior

Restore old `handle_key_release()` logic:
```c
int handle_key_release(int keycode, void *param)
{
    t_render *render = (t_render *)param;
    
    if (is_movement_key(keycode))
    {
        render->low_quality = 0;  // Old behavior
        render->dirty = 1;
    }
    
    return (0);
}
```

---

## Success Metrics

**Measurable outcomes** (from spec.md):

- ✅ **SC-001**: Zero intermediate renders during continuous input (100% elimination)
- ✅ **SC-002**: Preview starts within 200ms after input stops
- ✅ **SC-003**: Automatic preview → final transition (no user action)
- ✅ **SC-004**: Cancellation within 16ms (one frame at 60fps)
- ✅ **SC-005**: Smooth navigation with no buffering or stuttering

**How to verify**:
1. Run Tests 1-5 above
2. Measure timings with debug output
3. Get user feedback on perceived smoothness

---

## Next Steps

After verifying this feature:

1. Test with various scenes (simple → complex)
2. Test on both Linux and macOS
3. Gather user feedback on delay tuning
4. Consider runtime configuration (future enhancement)
5. Update Korean documentation in `docs/specs/018-debounced-rendering/`

---

## Support

**Issues**: Report via GitHub issues with:
- miniRT version
- Operating system
- Scene file used
- Steps to reproduce

**Questions**: See `contracts/debounce-api.md` for API details.
