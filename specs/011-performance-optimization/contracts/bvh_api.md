# BVH API Contract

**Module**: BVH Acceleration Structure  
**Purpose**: Public interface for BVH construction, traversal, and management  
**Language**: C

## Core Functions

### BVH Construction

#### `bvh_build`
```c
/**
 * @brief Builds a BVH tree from scene objects using Surface Area Heuristic
 * 
 * @param objects Array of object pointers to organize in BVH
 * @param object_count Number of objects in array
 * @param allocator Memory allocator with budget tracking
 * @param build_info Construction parameters (max depth, SAH bins, etc.)
 * @return Pointer to root BVH node, or NULL if construction fails
 * 
 * @pre objects != NULL && object_count > 0
 * @pre allocator != NULL && allocator->max_budget == 512MB
 * @pre build_info != NULL && build_info->max_depth == 64
 * 
 * @post Returns valid BVH root with depth <= max_depth
 * @post allocator->total_allocated <= allocator->max_budget (or returns NULL)
 * @post All leaf nodes contain >= 1 object
 * @post All internal nodes have exactly 2 children
 * 
 * @note If memory limit exceeded, returns NULL and sets allocator->limit_exceeded
 * @note Construction time: O(n log²n) for n objects
 * @note Memory usage: ~64 bytes per node, ~2n nodes for n objects
 * 
 * @example
 * t_bvh_allocator alloc = {0, 512 * 1024 * 1024, 0, false};
 * t_bvh_build_info info = {64, 4, 16, 1.0, 10.0};
 * t_bvh_node *root = bvh_build(scene->objects, scene->count, &alloc, &info);
 * if (!root)
 *     ft_putstr_fd("BVH construction failed, using fallback\n", 2);
 */
t_bvh_node *bvh_build(
    t_object **objects,
    int object_count,
    t_bvh_allocator *allocator,
    t_bvh_build_info *build_info
);
```

#### `bvh_destroy`
```c
/**
 * @brief Frees all memory associated with BVH tree
 * 
 * @param root Root node of BVH tree to destroy
 * 
 * @pre root can be NULL (no-op) or valid BVH root
 * @post All nodes recursively freed (bottom-up)
 * @post root pointer invalidated (caller must set to NULL)
 * 
 * @note Traverses tree in post-order to free children before parents
 * @note Does NOT free referenced objects (only BVH structure)
 * 
 * @example
 * bvh_destroy(bvh_root);
 * bvh_root = NULL;
 */
void bvh_destroy(t_bvh_node *root);
```

---

### BVH Traversal

#### `bvh_intersect`
```c
/**
 * @brief Finds closest ray-object intersection using BVH acceleration
 * 
 * @param root Root node of BVH tree
 * @param ray Ray to test for intersections
 * @param hit_record Output parameter for intersection details
 * @param metrics Performance metrics to update (can be NULL)
 * @return true if intersection found, false otherwise
 * 
 * @pre root != NULL (valid BVH tree)
 * @pre ray != NULL && ray->direction is normalized
 * @pre hit_record != NULL
 * @pre ray->t_min >= 0 && ray->t_max > ray->t_min
 * 
 * @post If return true: hit_record contains closest intersection in [t_min, t_max]
 * @post If return false: hit_record unchanged
 * @post If metrics != NULL: ray_count++, intersection_count updated, bvh_node_visits updated
 * 
 * @note Uses iterative stack-based traversal (64-entry stack)
 * @note Tests nodes in front-to-back order based on ray direction
 * @note Early termination when closer hit found
 * @note Average complexity: O(log n) for n objects in balanced tree
 * 
 * @example
 * t_hit_record hit;
 * if (bvh_intersect(bvh_root, &ray, &hit, &metrics))
 * {
 *     // Ray hit object at distance hit.t
 *     color = shade_hit_point(&hit, scene);
 * }
 */
bool bvh_intersect(
    t_bvh_node *root,
    t_ray *ray,
    t_hit_record *hit_record,
    t_perf_metrics *metrics
);
```

#### `bvh_intersect_any`
```c
/**
 * @brief Tests if ray intersects ANY object (early exit, no closest hit)
 * 
 * @param root Root node of BVH tree
 * @param ray Ray to test for intersections
 * @param max_distance Maximum distance to consider (for shadow rays)
 * @return true if any intersection found in [ray->t_min, max_distance]
 * 
 * @pre root != NULL
 * @pre ray != NULL
 * @pre max_distance > ray->t_min
 * 
 * @post Returns immediately on first intersection (no closest hit computed)
 * @note Optimized for shadow rays (visibility tests)
 * @note Faster than bvh_intersect for occlusion queries
 * 
 * @example
 * // Check if point is in shadow
 * t_ray shadow_ray = create_shadow_ray(hit_point, light_pos);
 * float distance_to_light = vec3_length(light_pos - hit_point);
 * bool in_shadow = bvh_intersect_any(bvh_root, &shadow_ray, distance_to_light);
 */
bool bvh_intersect_any(
    t_bvh_node *root,
    t_ray *ray,
    float max_distance
);
```

---

### BVH Updates (Dynamic Scenes)

#### `bvh_mark_dirty`
```c
/**
 * @brief Marks BVH nodes as needing update after object transform
 * 
 * @param root Root node of BVH tree
 * @param object Pointer to transformed object
 * 
 * @pre root != NULL
 * @pre object != NULL
 * 
 * @post All nodes containing object marked with is_dirty = true
 * @note Traverses tree to find affected nodes
 * @note Does NOT rebuild immediately (see bvh_update)
 * 
 * @example
 * // After moving/rotating object
 * object->transform = new_transform;
 * bvh_mark_dirty(bvh_root, object);
 */
void bvh_mark_dirty(t_bvh_node *root, t_object *object);
```

#### `bvh_update`
```c
/**
 * @brief Updates BVH tree for dirty nodes (refit or rebuild)
 * 
 * @param root Root node of BVH tree
 * @param allocator Memory allocator for potential rebuilds
 * @param build_info Construction parameters for rebuilds
 * @return Updated root node (may be different if full rebuild occurred)
 * 
 * @pre root != NULL
 * @pre allocator != NULL
 * @pre build_info != NULL
 * 
 * @post All dirty flags cleared
 * @post AABBs refitted or subtrees rebuilt based on dirty percentage
 * @note Strategy: refit if <10% dirty, rebuild if >=10% dirty
 * @note Refit: O(log n) per dirty object
 * @note Rebuild: O(m log m) for m dirty objects
 * 
 * @example
 * // After frame with object movements
 * bvh_root = bvh_update(bvh_root, &allocator, &build_info);
 */
t_bvh_node *bvh_update(
    t_bvh_node *root,
    t_bvh_allocator *allocator,
    t_bvh_build_info *build_info
);
```

---

### AABB Operations

#### `aabb_from_object`
```c
/**
 * @brief Computes tight axis-aligned bounding box for object
 * 
 * @param object Object to compute AABB for
 * @return AABB that tightly bounds the object
 * 
 * @pre object != NULL
 * @post Returned AABB.min <= AABB.max on all axes
 * @post AABB fully contains object geometry
 * 
 * @note Sphere: trivial (center ± radius)
 * @note Plane: infinite AABB (uses large but finite bounds)
 * @note Cylinder: includes body + caps, oriented by cylinder axis
 * 
 * @example
 * t_aabb box = aabb_from_object(sphere);
 * // box.min = sphere.center - vec3(radius, radius, radius)
 * // box.max = sphere.center + vec3(radius, radius, radius)
 */
t_aabb aabb_from_object(t_object *object);
```

#### `aabb_union`
```c
/**
 * @brief Computes AABB that contains both input AABBs
 * 
 * @param a First AABB
 * @param b Second AABB
 * @return Smallest AABB containing both a and b
 * 
 * @post result.min = min(a.min, b.min) on each axis
 * @post result.max = max(a.max, b.max) on each axis
 * 
 * @example
 * t_aabb parent = aabb_union(left_child->bbox, right_child->bbox);
 */
t_aabb aabb_union(t_aabb a, t_aabb b);
```

#### `aabb_surface_area`
```c
/**
 * @brief Computes surface area of AABB
 * 
 * @param box AABB to compute area for
 * @return Surface area in square units
 * 
 * @pre box.min <= box.max on all axes
 * @post result >= 0
 * 
 * @note Formula: 2 × (width×height + width×depth + height×depth)
 * @note Used by SAH cost evaluation
 * 
 * @example
 * float area = aabb_surface_area(node->bbox);
 * float probability = area / parent_area;
 */
float aabb_surface_area(t_aabb box);
```

#### `aabb_intersect_ray`
```c
/**
 * @brief Tests if ray intersects AABB in given interval
 * 
 * @param box AABB to test
 * @param ray Ray to test
 * @param t_min Minimum intersection distance
 * @param t_max Maximum intersection distance
 * @return true if ray intersects box in [t_min, t_max]
 * 
 * @pre ray->direction is normalized
 * @pre t_min >= 0 && t_max > t_min
 * 
 * @note Uses slab method (3 axis tests)
 * @note Cost: 6 divisions, 6 multiplies, 12 comparisons
 * @note Handles ray parallel to axis via infinity arithmetic
 * 
 * @example
 * if (aabb_intersect_ray(node->bbox, &ray, ray.t_min, ray.t_max))
 *     // Visit this BVH node
 */
bool aabb_intersect_ray(t_aabb box, t_ray *ray, float t_min, float t_max);
```

---

## Error Handling

### Error Conditions

| Function | Error Condition | Return Value | Side Effect |
|----------|----------------|--------------|-------------|
| `bvh_build` | NULL objects | NULL | None |
| `bvh_build` | object_count <= 0 | NULL | None |
| `bvh_build` | Memory limit exceeded | NULL | Sets allocator->limit_exceeded |
| `bvh_build` | Allocation failure | NULL | Partial tree freed |
| `bvh_intersect` | NULL root | false | None |
| `bvh_intersect` | NULL ray | false | None |
| `bvh_intersect_any` | NULL root | false | None |
| `bvh_mark_dirty` | NULL root | N/A | No-op |
| `bvh_update` | Rebuild fails | Original root | Prints warning |

### Fallback Strategy

When BVH construction fails (memory limit, allocation error):
1. Function returns NULL
2. Caller detects NULL and prints warning to stderr
3. Renderer falls back to brute-force O(n) intersection
4. Visual output remains correct (slower but functional)

```c
// Example fallback implementation
t_bvh_node *bvh = bvh_build(objects, count, &allocator, &info);
if (!bvh)
{
    ft_putstr_fd("Warning: BVH construction failed, using O(n) fallback\n", 2);
    scene->use_bvh = false;  // Disable BVH, use linear search
}
```

---

## Performance Guarantees

### Time Complexity

| Operation | Best Case | Average Case | Worst Case |
|-----------|-----------|--------------|------------|
| `bvh_build` | O(n log n) | O(n log²n) | O(n²) (degenerate) |
| `bvh_intersect` | O(log n) | O(log n) | O(n) (degenerate) |
| `bvh_intersect_any` | O(1) | O(log n) | O(n) |
| `bvh_mark_dirty` | O(log n) | O(log n) | O(n) (many copies) |
| `bvh_update` (refit) | O(1) | O(k log n) | O(n) (all dirty) |
| `bvh_update` (rebuild) | O(k log k) | O(k log²k) | O(n log²n) |

Where:
- n = total objects in scene
- k = number of dirty objects

### Space Complexity

| Structure | Per-instance | Total (n objects) |
|-----------|-------------|-------------------|
| BVH Node | 64 bytes | ~128n bytes (2n nodes) |
| AABB | 24 bytes | Included in node |
| Stack (traversal) | 512 bytes | 64 pointers × 8 bytes |

**Memory limit**: 512 MB enforced by allocator

---

## Thread Safety

**None**: All BVH functions are **not thread-safe**. This is intentional due to 42 School constraints prohibiting pthread usage.

- Multiple threads **must not** access the same BVH tree simultaneously
- Multiple threads **must not** call BVH functions concurrently
- Single-threaded rendering required

---

## Dependencies

### Required Headers
```c
#include "miniRT.h"      // Core types (t_vec3, t_object, etc.)
#include "bvh.h"         // BVH types and function declarations
#include <stdlib.h>      // malloc, free
#include <math.h>        // fminf, fmaxf
#include <stdbool.h>     // bool type
```

### Allowed Functions (42 School Compliant)
- `malloc`, `free` - Memory management
- `fminf`, `fmaxf` - Math operations
- `ft_putstr_fd` - Error messages (from libft)

### Prohibited Functions
- ❌ `pthread_*` - No multithreading
- ❌ SIMD intrinsics - No vectorization libraries
- ❌ External libraries - Only standard C and MiniLibX

---

## Usage Examples

### Complete BVH Workflow
```c
// 1. Initialize allocator and build info
t_bvh_allocator allocator = {
    .total_allocated = 0,
    .max_budget = 512 * 1024 * 1024,
    .node_count = 0,
    .limit_exceeded = false
};

t_bvh_build_info build_info = {
    .max_depth = 64,
    .min_objects_leaf = 4,
    .sah_bins = 16,
    .traversal_cost = 1.0,
    .intersection_cost = 10.0
};

// 2. Build BVH from scene objects
t_bvh_node *bvh_root = bvh_build(
    scene->objects,
    scene->object_count,
    &allocator,
    &build_info
);

if (!bvh_root)
{
    // Fallback to brute-force
    scene->use_bvh = false;
    return;
}

// 3. Ray tracing with BVH
t_ray ray = create_camera_ray(camera, pixel_x, pixel_y);
t_hit_record hit;

if (bvh_intersect(bvh_root, &ray, &hit, &metrics))
{
    // Shade the hit point
    t_color color = compute_lighting(scene, &hit, &ray);
    put_pixel(image, pixel_x, pixel_y, color);
}

// 4. Dynamic scene update (after object moved)
bvh_mark_dirty(bvh_root, moved_object);
bvh_root = bvh_update(bvh_root, &allocator, &build_info);

// 5. Cleanup at program exit
bvh_destroy(bvh_root);
```

### Shadow Ray Optimization
```c
// Optimized shadow ray test (early exit)
t_vec3 light_dir = vec3_normalize(light_pos - hit_point);
t_ray shadow_ray = {
    .origin = hit_point + normal * 0.001,  // Offset to avoid self-intersection
    .direction = light_dir,
    .t_min = 0.0,
    .t_max = INFINITY,
    .type = RAY_SHADOW,
    .depth = 0
};

float distance_to_light = vec3_length(light_pos - hit_point);
bool in_shadow = bvh_intersect_any(bvh_root, &shadow_ray, distance_to_light);

if (!in_shadow)
{
    // Add light contribution
    color += compute_diffuse(light, hit_normal, light_dir);
}
```

---

## Validation & Testing

### Unit Tests Required
- ✅ AABB construction for all object types
- ✅ AABB union correctness
- ✅ AABB-ray intersection (hit/miss cases)
- ✅ BVH build with various object counts
- ✅ BVH memory limit enforcement
- ✅ BVH traversal correctness vs brute-force
- ✅ BVH mark dirty and update

### Integration Tests Required
- ✅ Full scene rendering with BVH vs without (pixel-identical)
- ✅ Performance improvement measurement (FPS increase)
- ✅ Memory usage validation (under 512 MB)
- ✅ Fallback behavior on construction failure

### Performance Benchmarks
- ✅ BVH construction time for test_complex_scene.rt (<2s)
- ✅ FPS improvement for test_complex_scene.rt (3-5 FPS target)
- ✅ Intersection test reduction (60%+ reduction target)

---

## Version & Compatibility

**API Version**: 1.0  
**Compatible with**: miniRT project (42 School)  
**Language Standard**: C99  
**Platform**: Linux, macOS
