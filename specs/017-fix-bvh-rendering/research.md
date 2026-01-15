# Research: BVH Integration Diagnosis

**Feature**: Fix BVH Integration for Scene Rendering  
**Date**: 2026-01-15  
**Status**: Complete

## Research Overview

This document consolidates findings from analyzing the BVH integration failure in the miniRT ray tracer. All "NEEDS CLARIFICATION" items from the technical context have been resolved through codebase inspection and architectural analysis.

---

## Research Question 1: Root Cause of Black Screen

### Investigation

**Symptom**: After attempting to enable BVH acceleration, the scene renders as a black screen while HUD and key guide display correctly.

**Code Path Analysis**:
```
main() → parse_scene() → scene_build_bvh()
     → render_state_init() sets bvh_enabled = 0
     → scene_build_bvh() checks bvh_enabled → early return (no BVH built)
     → render_frame() → trace_ray() → checks bvh->enabled
     → if enabled: calls bvh_intersect() → returns 0 (no root)
     → returns black color {0,0,0} immediately
     → NEVER tries fallback non-BVH path
```

### Decision: Three-Part Fix Required

1. **Enable BVH by default**: Change `render_state.c:37` from `bvh_enabled = 0` to `bvh_enabled = 1`
2. **Remove early return**: In `trace.c`, change BVH path from returning black to setting `hit_found` flag
3. **Allow fallback**: Continue to brute-force checks if BVH doesn't find intersection

### Rationale

- The early return at `trace.c:163` prevents the working non-BVH code from executing
- BVH should be enabled by default since it's a performance optimization feature
- Fallback ensures robustness even if BVH has bugs or edge cases

### Alternatives Considered

**Alt 1**: Remove BVH flag check from scene_build_bvh()
- Pro: BVH always built
- Con: Wastes memory when BVH not wanted
- **Rejected**: User should control BVH usage

**Alt 2**: Remove early return but don't enable BVH by default
- Pro: Backward compatible (no BVH unless explicitly enabled)
- Con: Users won't get performance benefit without knowing to enable
- **Rejected**: BVH is main feature, should work out-of-box

**Alt 3**: Build BVH but keep disabled by default
- Pro: Ready to use, but opt-in
- Con: Wastes memory building unused structure
- **Rejected**: If building, should use it

---

## Research Question 2: Data Structure Compatibility

### Investigation

**Concern**: Do `t_hit` and `t_hit_record` have compatible layouts?

**Findings** (from `includes/ray.h`):
```c
typedef struct s_hit
{
    bool    hit;
    double  distance;
    t_vec3  point;
    t_vec3  normal;
    t_color color;
} t_hit;

typedef t_hit t_hit_record;  // Line 38: They are THE SAME TYPE
```

### Decision: No Changes Needed

Both types are identical. The `typedef` makes `t_hit_record` an alias for `t_hit`.

### Rationale

- Code uses both names interchangeably for historical reasons
- Some functions use `t_hit`, others use `t_hit_record`
- C typedef creates true alias, not a new type
- No conversion or compatibility issues exist

---

## Research Question 3: BVH Initialization Sequence

### Investigation

**Current Sequence**:
1. `parse_scene()` opens file and parses elements
2. `validate_scene()` checks required elements exist
3. `scene_build_bvh()` called if validation succeeds
4. `scene_build_bvh()` checks `bvh_enabled` flag → returns early if 0
5. `render_state_init()` previously set `bvh_enabled = 0`

**Problem**: Chicken-and-egg - flag is 0, so BVH never built

### Decision: Enable Flag Before Build

Change `render_state_init()` to set `bvh_enabled = 1` so `scene_build_bvh()` actually builds the tree.

### Rationale

- Parser sequence is correct (build after validation)
- BVH initialization code is correct (checks for empty scenes, allocates properly)
- Only issue is the flag default value
- Simplest fix with minimal code change

### Best Practices Applied

**BVH Initialization Best Practices**:
1. ✅ Build after scene validation (objects must exist)
2. ✅ Check for empty scenes before allocation
3. ✅ Store BVH in render state for easy access
4. ✅ Allow runtime enable/disable via flag
5. ✅ Clean up BVH on scene destruction

**Ray Tracing Fallback Pattern**:
1. ✅ Try optimized path first (BVH)
2. ✅ Fall back to guaranteed-working path (brute force)
3. ✅ Never leave code path that returns invalid result
4. ❌ **CURRENT BUG**: Early return prevents fallback

---

## Research Question 4: Performance Validation Strategy

### Investigation

**BVH Performance Characteristics**:
- Reduces O(n) intersection tests to O(log n) for most rays
- Typical speedup: 2-10x for 50-100 objects
- Greater speedup for larger scenes (100-1000+ objects)
- Minimal benefit for small scenes (<10 objects)
- Overhead: Memory for tree nodes, build time on scene load

### Decision: Two-Phase Testing

**Phase 1: Correctness**
- Test with simple scenes (1-5 objects)
- Verify visual output identical between BVH/non-BVH
- Confirm no crashes, memory leaks, or artifacts

**Phase 2: Performance**
- Test with complex scenes (50-100+ objects)
- Measure frame time with BVH enabled vs disabled
- Target: 2x minimum speedup on 50-object scene

### Rationale

- Correctness must come before performance optimization
- Small scenes validate algorithm works properly
- Large scenes demonstrate performance benefit
- Comparing BVH vs non-BVH proves BVH is helping

### Measurement Approach

```c
// Already instrumented in existing code:
metrics_add_ray(&scene->render_state.metrics);  // trace.c:156

// HUD displays:
- Frame time (milliseconds)
- Ray count
- BVH status (enabled/disabled)
```

**Manual Testing**:
1. Load scene, note frame time in HUD
2. Toggle BVH off (press 'B' key if implemented)
3. Note new frame time
4. Calculate speedup ratio
5. Verify visual output unchanged

---

## Research Question 5: Robustness and Error Handling

### Investigation

**Failure Modes Identified**:
1. BVH not initialized (bvh == NULL)
2. BVH disabled (bvh->enabled == 0)
3. BVH tree empty (bvh->root == NULL)
4. BVH returns no intersection (valid case: ray misses all objects)

**Current Handling**:
- ✅ `bvh_intersect()` checks NULL pointers (line 124-125)
- ✅ `scene_build_bvh()` checks for empty scenes (line 108)
- ❌ `trace_ray()` early returns black on BVH failure (line 163)

### Decision: Graceful Fallback Required

When BVH path returns 0 (no intersection), continue to brute-force checks instead of returning black immediately.

### Rationale

- BVH might fail for various reasons (bugs, edge cases, unimplemented features)
- Non-BVH path is proven to work correctly
- Falling back ensures rendering always succeeds
- User sees correct output even if BVH has issues

### Error Scenarios & Handling

| Scenario | Current Behavior | Fixed Behavior |
|----------|------------------|----------------|
| BVH NULL | Falls back to brute force | Same (correct) |
| BVH disabled | Falls back to brute force | Same (correct) |
| BVH enabled but root NULL | Returns black screen | Fall back to brute force |
| BVH finds no intersection | Returns black screen | Fall back to brute force |
| BVH finds intersection | Returns lit color | Same (correct) |

---

## Research Question 6: Testing Strategy for 42 School Constraints

### Investigation

**42 School Requirements**:
- No pthread (threading) allowed
- No fork/pipe (multiprocessing) allowed
- No external optimization libraries
- Must use only allowed functions: libc, math.h, mlx_*, libft

**BVH Implementation Audit**:
```bash
# Check for prohibited functions
grep -r "pthread" src/spatial/  # No results
grep -r "fork\|pipe" src/spatial/  # No results
grep -r "simd\|omp\|tbb" src/spatial/  # No results
```

**Functions Used in BVH Code**:
- `malloc()`, `free()` - ✅ Allowed (stdlib)
- `sqrt()`, `fmin()`, `fmax()` - ✅ Allowed (math.h)
- No threading primitives - ✅ Compliant
- No external libraries - ✅ Compliant

### Decision: No Changes Needed for Compliance

BVH implementation already complies with 42 School constraints. It uses only algorithmic optimization (spatial partitioning), which is explicitly allowed.

### Rationale

- BVH is a pure algorithm, not a library
- Uses only allowed functions (malloc, free, math.h)
- No parallel execution or external dependencies
- Fits "algorithmic optimization" category in constitution

---

## Implementation Recommendations

### Priority 1: Critical Fixes (Required for MVP)
1. Change `bvh_enabled` default from 0 to 1 in `render_state.c`
2. Fix early return logic in `trace_ray()` to allow fallback
3. Test with simple scene to verify rendering works

### Priority 2: Validation (Required for Release)
1. Test visual output parity (BVH vs non-BVH)
2. Test performance improvement on complex scenes
3. Verify memory leaks with valgrind
4. Confirm norminette compliance

### Priority 3: Enhancements (Future Work)
1. Add runtime BVH toggle key handler (Press 'B')
2. Add BVH statistics to HUD (node count, max depth)
3. Optimize BVH for very large scenes (1000+ objects)
4. Add BVH visualization mode for debugging

---

## Conclusion

All research questions have been resolved. The root cause is a simple configuration issue (wrong default flag value) combined with a logic error (early return preventing fallback). The fix is minimal and low-risk:

**Changes Required**: 2 lines in render_state.c, 6 lines in trace.c  
**Risk Level**: Low (changes are localized, well-understood)  
**Testing**: Manual with existing scene files  
**Compliance**: Fully compliant with 42 School constraints  

Ready to proceed to implementation phase.
