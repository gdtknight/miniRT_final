# Bounding Box API Contract

**Module**: `src/optimization/bbox.c`, `src/math/bbox_math.c`  
**Header**: `includes/optimization.h`  
**Date**: 2025-12-19

## Overview

This contract defines the API for axis-aligned bounding box (AABB) operations: computation from geometric primitives and fast ray-box intersection testing.

---

## Function: compute_bbox_sphere

### Signature

```c
t_bbox  compute_bbox_sphere(t_sphere *sphere);
```

### Purpose
Compute tight AABB enclosing sphere primitive.

### Parameters

| Parameter | Type | Direction | Description | Constraints |
|-----------|------|-----------|-------------|-------------|
| sphere | t_sphere* | IN | Sphere object | Non-NULL, diameter > 0 |

### Return Value
t_bbox with min/max corners enclosing sphere

### Algorithm

```c
radius = sphere->diameter / 2.0
bbox.min = sphere->center - (radius, radius, radius)
bbox.max = sphere->center + (radius, radius, radius)
```

### Example

```c
t_sphere sphere = {.center = {0, 0, 0}, .diameter = 4.0};
t_bbox bbox = compute_bbox_sphere(&sphere);
/* bbox.min = (-2, -2, -2), bbox.max = (2, 2, 2) */
```

---

## Function: compute_bbox_plane

### Signature

```c
t_bbox  compute_bbox_plane(t_plane *plane, t_bbox *scene_bounds);
```

### Purpose
Compute scene-clipped AABB for infinite plane. Uses scene bounds to avoid infinite extents.

### Parameters

| Parameter | Type | Direction | Description | Constraints |
|-----------|------|-----------|-------------|-------------|
| plane | t_plane* | IN | Plane object | Non-NULL, normal normalized |
| scene_bounds | t_bbox* | IN | Bounding box of all scene objects | Non-NULL |

### Return Value
t_bbox representing plane clipped to scene extents

### Algorithm

```c
/* Project scene corners onto plane, find min/max extents */
bbox = scene_bounds  /* Start with scene box */
/* Adjust along plane normal to include plane */
```

### Notes
- Planes are infinite, but BVH requires finite bounds
- Use scene bounding box as conservative estimate
- Alternative: Use large but finite box (e.g., [-10000, 10000])

---

## Function: compute_bbox_cylinder

### Signature

```c
t_bbox  compute_bbox_cylinder(t_cylinder *cylinder);
```

### Purpose
Compute tight AABB enclosing finite cylinder primitive.

### Parameters

| Parameter | Type | Direction | Description | Constraints |
|-----------|------|-----------|-------------|-------------|
| cylinder | t_cylinder* | IN | Cylinder object | Non-NULL, diameter > 0, height > 0 |

### Return Value
t_bbox with min/max corners enclosing cylinder

### Algorithm

```c
radius = cylinder->diameter / 2.0
/* Compute cylinder axis endpoints */
p0 = cylinder->center - cylinder->axis * (cylinder->height / 2.0)
p1 = cylinder->center + cylinder->axis * (cylinder->height / 2.0)

/* Expand bbox to include circular cross-sections at both ends */
/* This is conservative (box larger than actual cylinder) */
for each endpoint p:
    bbox = union(bbox, sphere_bbox(p, radius))
```

### Example

```c
/* Cylinder along Y axis, centered at origin */
t_cylinder cyl = {
    .center = {0, 0, 0},
    .axis = {0, 1, 0},
    .diameter = 2.0,
    .height = 10.0
};
t_bbox bbox = compute_bbox_cylinder(&cyl);
/* bbox.min ≈ (-1, -5, -1), bbox.max ≈ (1, 5, 1) */
```

---

## Function: bbox_intersect

### Signature

```c
bool    bbox_intersect(t_bbox *bbox, t_ray *ray, double *t_near, double *t_far);
```

### Purpose
Fast ray-AABB intersection test using slab method. Returns parametric t values if intersection exists.

### Parameters

| Parameter | Type | Direction | Description | Constraints |
|-----------|------|-----------|-------------|-------------|
| bbox | t_bbox* | IN | Bounding box to test | Non-NULL, min ≤ max |
| ray | t_ray* | IN | Ray to test | Non-NULL, direction normalized |
| t_near | double* | OUT | Near intersection distance | Can be NULL if not needed |
| t_far | double* | OUT | Far intersection distance | Can be NULL if not needed |

### Return Value

| Value | Condition | Description |
|-------|-----------|-------------|
| true | Intersection | Ray intersects box, t_near/t_far set if non-NULL |
| false | No intersection | Ray misses box |

### Algorithm (Slab Method)

```c
t_min = 0.0
t_max = INFINITY

for axis in [X, Y, Z]:
    /* Compute intersection with slab perpendicular to axis */
    inv_dir = 1.0 / ray->direction[axis]
    t0 = (bbox->min[axis] - ray->origin[axis]) * inv_dir
    t1 = (bbox->max[axis] - ray->origin[axis]) * inv_dir
    
    /* Swap if ray direction negative */
    if inv_dir < 0:
        swap(t0, t1)
    
    /* Update interval */
    t_min = max(t_min, t0)
    t_max = min(t_max, t1)
    
    /* Early exit: interval empty */
    if t_min > t_max:
        return false

/* Intersection exists if t_max > 0 (in front of ray) */
if t_near != NULL:
    *t_near = t_min
if t_far != NULL:
    *t_far = t_max

return t_max > 0.0
```

### Performance
- **Time complexity**: O(1) (3 axis tests, no loops)
- **Typical execution**: 8-12ns on modern CPU
- **Branch prediction**: Consistent performance with sorted rays

### Edge Cases

| Case | Handling |
|------|----------|
| Ray parallel to axis | inv_dir = ±∞, handled by min/max logic |
| Ray origin inside box | t_min < 0, t_max > 0 → return true |
| Ray pointing away from box | t_max < 0 → return false |
| Degenerate box (min == max) | Still works (point intersection) |

### Example Usage

```c
t_bbox bbox = {{-1, -1, -1}, {1, 1, 1}};
t_ray ray = {.origin = {0, 0, -5}, .direction = {0, 0, 1}};
double t_near, t_far;

if (bbox_intersect(&bbox, &ray, &t_near, &t_far))
{
    printf("Ray enters box at t=%.2f, exits at t=%.2f\n", t_near, t_far);
    /* Output: Ray enters box at t=4.00, exits at t=6.00 */
}
```

---

## Function: bbox_union

### Signature

```c
t_bbox  bbox_union(t_bbox *a, t_bbox *b);
```

### Purpose
Compute bounding box that encloses both input boxes (component-wise min/max).

### Parameters

| Parameter | Type | Direction | Description | Constraints |
|-----------|------|-----------|-------------|-------------|
| a | t_bbox* | IN | First bounding box | Non-NULL |
| b | t_bbox* | IN | Second bounding box | Non-NULL |

### Return Value
t_bbox enclosing both input boxes

### Algorithm

```c
result.min.x = fmin(a->min.x, b->min.x)
result.min.y = fmin(a->min.y, b->min.y)
result.min.z = fmin(a->min.z, b->min.z)

result.max.x = fmax(a->max.x, b->max.x)
result.max.y = fmax(a->max.y, b->max.y)
result.max.z = fmax(a->max.z, b->max.z)
```

### Example

```c
t_bbox a = {{0, 0, 0}, {1, 1, 1}};
t_bbox b = {{0.5, 0.5, 0.5}, {2, 2, 2}};
t_bbox u = bbox_union(&a, &b);
/* u.min = (0, 0, 0), u.max = (2, 2, 2) */
```

---

## Helper Function: bbox_surface_area

### Signature

```c
double  bbox_surface_area(t_bbox *bbox);
```

### Purpose
Compute surface area of bounding box. Used in SAH (Surface Area Heuristic) for advanced BVH construction.

### Algorithm

```c
dx = bbox->max.x - bbox->min.x
dy = bbox->max.y - bbox->min.y
dz = bbox->max.z - bbox->min.z

surface_area = 2.0 * (dx*dy + dy*dz + dz*dx)
```

### Note
Not required for basic BVH (median split), but useful for future SAH optimization.

---

## Helper Function: bbox_longest_axis

### Signature

```c
int bbox_longest_axis(t_bbox *bbox);
```

### Purpose
Determine which axis has the greatest extent (used for BVH split axis selection).

### Return Value

| Value | Condition | Description |
|-------|-----------|-------------|
| 0 | X longest | bbox.max.x - bbox.min.x is largest |
| 1 | Y longest | bbox.max.y - bbox.min.y is largest |
| 2 | Z longest | bbox.max.z - bbox.min.z is largest |

### Algorithm

```c
dx = bbox->max.x - bbox->min.x
dy = bbox->max.y - bbox->min.y
dz = bbox->max.z - bbox->min.z

if (dx >= dy && dx >= dz)
    return 0  /* X */
else if (dy >= dz)
    return 1  /* Y */
else
    return 2  /* Z */
```

---

## Testing Requirements

### Unit Tests

```c
/* Test: Sphere bbox computation */
void test_bbox_sphere()
{
    t_sphere sphere = {.center = {1, 2, 3}, .diameter = 4.0};
    t_bbox bbox = compute_bbox_sphere(&sphere);
    
    assert(bbox.min.x == -1.0 && bbox.min.y == 0.0 && bbox.min.z == 1.0);
    assert(bbox.max.x == 3.0 && bbox.max.y == 4.0 && bbox.max.z == 5.0);
}

/* Test: Ray-box intersection (hit) */
void test_bbox_intersect_hit()
{
    t_bbox bbox = {{-1, -1, -1}, {1, 1, 1}};
    t_ray ray = {.origin = {0, 0, -5}, .direction = {0, 0, 1}};
    double t_near, t_far;
    
    bool hit = bbox_intersect(&bbox, &ray, &t_near, &t_far);
    
    assert(hit == true);
    assert(fabs(t_near - 4.0) < 0.01);
    assert(fabs(t_far - 6.0) < 0.01);
}

/* Test: Ray-box intersection (miss) */
void test_bbox_intersect_miss()
{
    t_bbox bbox = {{-1, -1, -1}, {1, 1, 1}};
    t_ray ray = {.origin = {10, 0, 0}, .direction = {0, 1, 0}};
    
    bool hit = bbox_intersect(&bbox, &ray, NULL, NULL);
    
    assert(hit == false);
}

/* Test: Ray origin inside box */
void test_bbox_intersect_inside()
{
    t_bbox bbox = {{-1, -1, -1}, {1, 1, 1}};
    t_ray ray = {.origin = {0, 0, 0}, .direction = {1, 0, 0}};
    double t_near, t_far;
    
    bool hit = bbox_intersect(&bbox, &ray, &t_near, &t_far);
    
    assert(hit == true);
    assert(t_near < 0);  /* Behind ray origin */
    assert(t_far > 0);   /* In front */
}
```

---

## Performance Characteristics

### Bbox Computation (per object)

| Operation | Time Complexity | Typical Time |
|-----------|----------------|--------------|
| compute_bbox_sphere | O(1) | ~5ns |
| compute_bbox_cylinder | O(1) | ~15ns |
| compute_bbox_plane | O(1) | ~10ns |

### Ray-Box Intersection

| Scenario | Typical Time | Branch Mispredictions |
|----------|-------------|----------------------|
| Miss (early exit) | 6-8ns | Low |
| Hit (all axes tested) | 10-12ns | Low |
| Ray parallel to axis | 8-10ns | Medium |

---

## Integration with BVH

```c
/* In BVH construction (bvh.c) */
t_bbox  compute_node_bbox(t_object_meta *objects, int count)
{
    t_bbox result = compute_bbox_for_object(&objects[0]);
    
    for (int i = 1; i < count; i++)
    {
        t_bbox obj_bbox = compute_bbox_for_object(&objects[i]);
        result = bbox_union(&result, &obj_bbox);
    }
    
    return result;
}

t_bbox  compute_bbox_for_object(t_object_meta *meta)
{
    if (meta->type == OBJECT_SPHERE)
        return compute_bbox_sphere(meta->object_ptr);
    else if (meta->type == OBJECT_CYLINDER)
        return compute_bbox_cylinder(meta->object_ptr);
    else  /* OBJECT_PLANE */
        return compute_bbox_plane(meta->object_ptr, &scene_bounds);
}

/* In BVH traversal (bvh_traverse.c) */
bool    traverse_node(t_bvh_node *node, t_ray *ray, t_hit *hit)
{
    /* Early rejection using bbox intersection */
    if (!bbox_intersect(&node->bbox, ray, NULL, NULL))
        return false;
    
    /* ... rest of traversal ... */
}
```

---

## Compliance Summary

| Requirement | Compliance | Evidence |
|-------------|-----------|----------|
| FR-006 (bounding volumes) | ✅ | AABB computed for all object types |
| FR-010 (bbox before intersection) | ✅ | bbox_intersect used in traversal |
| FR-017 (memory efficiency) | ✅ | 24 bytes per bbox (minimal) |
| FR-021 (42 functions only) | ✅ | Only fmin, fmax, fabs from libm |
| FR-022 (no pthread) | ✅ | Pure computation, no concurrency |

**API Contract Complete**: Ready for implementation.
