# API Contracts: BVH Integration

**Feature**: Fix BVH Integration for Scene Rendering  
**Date**: 2026-01-15  
**Status**: No External API Changes

## Overview

This document describes the function contracts for BVH integration. **No external API changes are required** for this fix. All existing function signatures remain unchanged. This document serves as reference for understanding the contracts and guarantees.

---

## Public Functions (No Changes)

### 1. trace_ray()

**Signature**:
```c
t_color trace_ray(t_scene *scene, t_ray *ray);
```

**Purpose**: Trace ray through scene and return pixel color

**Parameters**:
- `scene`: Scene containing objects, lights, and render state (non-NULL)
- `ray`: Ray to trace with origin and direction (non-NULL)

**Returns**: 
- RGB color (0-255 per channel)
- Black `{0, 0, 0}` if ray misses all objects

**Preconditions**:
- `scene` must be initialized and validated
- `ray->direction` must be normalized
- `scene->render_state` must be initialized

**Postconditions**:
- Return value represents lit color if object hit
- Return value is black if no intersection found
- No side effects (const-safe)

**Contract Changes**: None (implementation only)

**Behavior Before Fix**:
```c
If BVH enabled:
    Call bvh_intersect()
    If hit: return lit color
    If miss: return BLACK (early return - BUG)
Else:
    Check all objects (never reached if BVH enabled)
```

**Behavior After Fix**:
```c
If BVH enabled:
    Call bvh_intersect()
    If hit: set hit_found flag
If !hit_found:
    Check all objects (fallback path)
If hit_found:
    Return lit color
Else:
    Return BLACK
```

---

### 2. bvh_intersect()

**Signature**:
```c
int bvh_intersect(t_bvh *bvh, t_ray ray, t_hit_record *hit, void *scene);
```

**Purpose**: Find closest ray-object intersection using BVH acceleration

**Parameters**:
- `bvh`: BVH tree structure (can be NULL)
- `ray`: Ray to test (pass by value, no side effects)
- `hit`: Output parameter for intersection result (non-NULL)
- `scene`: Scene context for object lookup (non-NULL, cast to t_scene*)

**Returns**:
- `1` if intersection found (updates `hit` with closest intersection)
- `0` if no intersection found or BVH invalid

**Preconditions**:
- `hit->distance` should be initialized to `INFINITY` or previous closest distance
- `scene` must contain valid object arrays

**Postconditions**:
- If returns 1: `hit` contains valid intersection data
- If returns 0: `hit` is unchanged
- No memory allocation or deallocation

**Contract Guarantees**:
- Finds same intersections as brute force (correctness)
- Returns closest intersection if multiple hits
- Safe with NULL `bvh` pointer (returns 0)
- Safe with NULL `bvh->root` (returns 0)
- Safe with `bvh->enabled == 0` (returns 0)

**Contract Changes**: None

---

### 3. scene_build_bvh()

**Signature**:
```c
void scene_build_bvh(t_scene *scene);
```

**Purpose**: Build BVH tree for scene objects

**Parameters**:
- `scene`: Scene to build BVH for (non-NULL)

**Returns**: void (stores result in `scene->render_state.bvh`)

**Preconditions**:
- `scene` is initialized and validated
- `scene->render_state.bvh_enabled` determines whether to build
- Object arrays are allocated and populated

**Postconditions**:
- If `bvh_enabled == 0`: No BVH built, `scene->render_state.bvh` unchanged
- If `bvh_enabled == 1` and objects exist: BVH built, `scene->render_state.bvh` valid
- If `bvh_enabled == 1` and no objects: No BVH built (saves memory)
- No memory leaks (temporary arrays freed)

**Contract Changes**: None (but behavior changes due to default flag)

**Behavior Before Fix**:
- `bvh_enabled` defaults to 0 → BVH never built

**Behavior After Fix**:
- `bvh_enabled` defaults to 1 → BVH built for most scenes

---

### 4. render_state_init()

**Signature**:
```c
void render_state_init(t_render_state *state);
```

**Purpose**: Initialize rendering state with default values

**Parameters**:
- `state`: Render state structure to initialize (non-NULL)

**Returns**: void (initializes fields in place)

**Preconditions**:
- `state` points to allocated memory

**Postconditions**:
- All fields initialized to sensible defaults
- `bvh` pointer initialized to NULL
- `bvh_enabled` set to default value

**Contract Changes**: None (default value changes)

**Default Values Before Fix**:
```c
state->bvh_enabled = 0;  // BVH disabled by default
```

**Default Values After Fix**:
```c
state->bvh_enabled = 1;  // BVH enabled by default
```

---

## Internal Functions (No Changes)

### bvh_node_intersect()

**Signature**:
```c
int bvh_node_intersect(t_bvh_node *node, t_ray ray, t_hit_record *hit, void *scene);
```

**Purpose**: Recursively traverse BVH tree node

**Contract**:
- Returns 1 if intersection found in node's subtree
- Tests bounding box first, early out if miss
- Leaf nodes: test contained objects
- Internal nodes: recursively test children

**Contract Changes**: None

---

### check_sphere_intersections()

**Signature**:
```c
int check_sphere_intersections(t_scene *scene, t_ray *ray, t_hit *hit);
```

**Purpose**: Test ray against all spheres in scene

**Contract**:
- Returns 1 if any sphere hit (updates hit with closest)
- Returns 0 if no spheres hit (hit unchanged)
- Always tests all spheres (brute force)

**Contract Changes**: None

**Usage Change**: Now called as fallback when BVH misses or disabled

---

### check_plane_intersections()

**Signature**:
```c
int check_plane_intersections(t_scene *scene, t_ray *ray, t_hit *hit);
```

**Purpose**: Test ray against all planes in scene

**Contract**: Same as `check_sphere_intersections()` but for planes

**Contract Changes**: None

---

### check_cylinder_intersections()

**Signature**:
```c
int check_cylinder_intersections(t_scene *scene, t_ray *ray, t_hit *hit);
```

**Purpose**: Test ray against all cylinders in scene

**Contract**: Same as `check_sphere_intersections()` but for cylinders

**Contract Changes**: None

---

## Data Structure Contracts

### t_hit / t_hit_record

**Invariants**:
- `distance` is positive or INFINITY
- `normal` is normalized (unit length)
- `point` is computed as `ray.origin + ray.direction * distance`

**Usage Contract**:
- Initialize `distance = INFINITY` before intersection tests
- Compare distances to find closest hit
- Only update if new hit is closer

---

### t_bvh

**Invariants**:
- If `root == NULL`, tree is empty (no objects)
- If `enabled == 0`, BVH exists but not used
- `total_nodes` and `max_depth` are statistics only

**Lifecycle**:
```
1. bvh_create() → Allocate structure, set defaults
2. bvh_build() → Construct tree, set root
3. bvh_intersect() → Use tree (can be called many times)
4. bvh_destroy() → Free all nodes and structure
```

---

### t_render_state

**Invariants**:
- If `bvh_enabled == 0`, BVH may or may not exist
- If `bvh_enabled == 1`, BVH should be built (but can fail)
- `bvh` pointer can be NULL even if `bvh_enabled == 1`

**State Diagram**:
```
[Init] bvh_enabled=1, bvh=NULL
   ↓
[Build] scene_build_bvh() called
   ↓
[Ready] bvh_enabled=1, bvh!=NULL, bvh->enabled=1
   ↓ (optional)
[Toggle Off] bvh->enabled=0 (tree exists but not used)
   ↓
[Toggle On] bvh->enabled=1 (reuse existing tree)
```

---

## Behavioral Contracts (Correctness)

### Visual Output Parity

**Contract**: For the same scene and ray, output must be identical whether BVH is enabled or disabled.

**Verification**:
```c
// Test each pixel with BVH on and off
t_color color_bvh = trace_ray_with_bvh(scene, ray);
t_color color_brute = trace_ray_without_bvh(scene, ray);
assert(color_bvh.r == color_brute.r);
assert(color_bvh.g == color_brute.g);
assert(color_bvh.b == color_brute.b);
```

**Guarantees**:
- Same objects detected
- Same closest object selected
- Same lighting applied
- No visual artifacts or differences

---

### Performance Contract

**Contract**: BVH must not be slower than brute force (excluding build time).

**Expected Performance**:
- Small scenes (<10 objects): 0.8x - 2x (overhead may dominate)
- Medium scenes (10-50 objects): 2x - 5x faster
- Large scenes (50-200 objects): 5x - 10x faster
- Very large scenes (200+ objects): 10x+ faster

**Measurement**:
```
Frame time = Total time for all rays in one frame
Speedup = Frame time (brute force) / Frame time (BVH)
```

---

### Memory Safety Contract

**Contract**: No memory leaks, no use-after-free, no buffer overflows.

**Guarantees**:
- All `malloc()` has corresponding `free()`
- BVH destroyed properly on scene cleanup
- No dangling pointers
- No out-of-bounds array access

**Verification**:
```bash
valgrind --leak-check=full ./miniRT scene.rt
# Should report 0 leaks
```

---

## Error Handling Contracts

### NULL Pointer Handling

**Contract**: All functions handle NULL pointers gracefully without crashing.

**Examples**:
```c
bvh_intersect(NULL, ray, hit, scene)     → Returns 0
bvh_intersect(bvh, ray, NULL, scene)     → Undefined (precondition violation)
scene_build_bvh(NULL)                    → Undefined (precondition violation)
```

---

### Empty Scene Handling

**Contract**: Empty scenes (0 objects) render as black without crashing.

**Behavior**:
```c
scene->sphere_count == 0 &&
scene->plane_count == 0 &&
scene->cylinder_count == 0
→ scene_build_bvh() returns early, no BVH built
→ trace_ray() finds no intersections, returns black
```

---

### Fallback Guarantee

**Contract**: If BVH fails or is disabled, rendering falls back to brute force.

**Failure Modes Handled**:
1. BVH disabled (`bvh->enabled == 0`) → Use brute force
2. BVH NULL (`bvh == NULL`) → Use brute force
3. BVH empty (`bvh->root == NULL`) → Use brute force
4. BVH finds no hit → Try brute force (after fix)

---

## Testing Contracts

### Test Scenarios (from spec)

**TS-001**: Single sphere scene renders correctly
- Input: `scenes/simple_sphere.rt`
- Expected: Sphere visible with correct color
- BVH mode: Both enabled and disabled
- Result: Identical output

**TS-002**: Complex scene renders faster with BVH
- Input: Scene with 50+ objects
- Expected: Frame time reduced 2x minimum
- Measurement: Compare HUD frame time
- Result: BVH faster

**TS-003**: Empty scene doesn't crash
- Input: Scene with 0 objects
- Expected: Black screen, no crash
- BVH mode: Both modes
- Result: Graceful handling

**TS-004**: Memory leak free
- Input: Any scene
- Tool: valgrind
- Expected: 0 bytes leaked
- Result: Clean shutdown

---

## Compatibility Contracts

### Backward Compatibility

**Contract**: Existing scene files continue to work without modification.

**Guarantees**:
- Scene file format unchanged
- No new required fields
- Rendering behavior visually identical
- Only performance improved

---

### Forward Compatibility

**Contract**: Future enhancements can extend BVH without breaking existing code.

**Extension Points**:
- Add new object types → Update `intersect_object()` switch
- Add BVH statistics → Use existing `total_nodes`, `max_depth` fields
- Add visualization → Use existing `visualize` flag
- Add runtime toggle → Use existing `bvh->enabled` field

---

## Summary

**API Changes**: None  
**Behavior Changes**: BVH now enabled by default, fallback works correctly  
**Contract Guarantees**: All existing contracts maintained  
**New Contracts**: None  

This is an internal fix with no external API impact.
