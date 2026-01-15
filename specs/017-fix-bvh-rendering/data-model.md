# Data Model: BVH Integration

**Feature**: Fix BVH Integration for Scene Rendering  
**Date**: 2026-01-15  
**Status**: No Changes Required

## Overview

This document describes the data structures involved in BVH integration. **No data model changes are required** for this fix - all existing structures are correctly defined. This document serves as reference for understanding the relationships between structures.

---

## Core Entities

### 1. t_render_state

**Purpose**: Manages rendering configuration and state including BVH status

**Location**: `includes/render_state.h`

**Structure**:
```c
typedef struct s_render_state
{
    t_quality_mode          quality;           // Current quality mode
    t_quality_mode          target_quality;    // Target quality after interaction
    t_interaction_state     interaction;       // User interaction tracking
    t_progressive_state     progressive;       // Progressive render state
    t_metrics               metrics;           // Performance metrics
    t_bvh                   *bvh;              // BVH tree pointer (NULL if not built)
    int                     adaptive_enabled;  // Adaptive quality flag
    int                     bvh_enabled;       // BVH acceleration flag [FIX HERE]
    int                     show_info;         // HUD display flag
} t_render_state;
```

**Key Fields for BVH**:
- `bvh`: Pointer to BVH tree structure, NULL if not initialized
- `bvh_enabled`: Flag controlling whether to build and use BVH (currently defaults to 0)

**Validation Rules**:
- If `bvh_enabled == 1`, `scene_build_bvh()` will build BVH during scene load
- If `bvh_enabled == 0`, BVH is not built (saves memory)
- `bvh` pointer can be NULL even if `bvh_enabled == 1` (build failed or empty scene)

**State Transitions**:
```
Initial State: bvh_enabled=0, bvh=NULL
     ↓
[FIX] Set bvh_enabled=1 in render_state_init()
     ↓
Scene Load → scene_build_bvh()
     ↓
BVH Built: bvh_enabled=1, bvh!=NULL, bvh->enabled=1
     ↓
[Optional] User toggles BVH off: bvh->enabled=0
     ↓
BVH Disabled: bvh_enabled=1, bvh!=NULL, bvh->enabled=0
```

---

### 2. t_bvh

**Purpose**: Root structure for Bounding Volume Hierarchy tree

**Location**: `includes/spatial.h`

**Structure**:
```c
typedef struct s_bvh
{
    t_bvh_node  *root;         // Root node of BVH tree
    int         enabled;       // Runtime enable/disable flag
    int         total_nodes;   // Total nodes in tree (for stats)
    int         max_depth;     // Maximum tree depth (for stats)
    int         visualize;     // Visualization mode flag
} t_bvh;
```

**Key Fields**:
- `root`: Pointer to root node, NULL if tree is empty
- `enabled`: Runtime flag to toggle BVH without rebuilding (0 = disabled, 1 = enabled)

**Relationships**:
- Referenced by `t_render_state.bvh`
- Owns tree of `t_bvh_node` structures
- Destroyed on scene cleanup via `bvh_destroy()`

**Validation Rules**:
- `root` must be non-NULL for BVH to be usable
- `enabled` can be toggled at runtime without rebuilding tree
- `total_nodes` and `max_depth` are statistics, not used in traversal

---

### 3. t_bvh_node

**Purpose**: Node in BVH tree, either internal node with children or leaf node with objects

**Location**: `includes/spatial.h`

**Structure**:
```c
typedef struct s_bvh_node
{
    t_aabb              bounds;        // Bounding box for this node
    struct s_bvh_node   *left;         // Left child (NULL if leaf)
    struct s_bvh_node   *right;        // Right child (NULL if leaf)
    t_object_ref        *objects;      // Array of object refs (only for leaf)
    int                 object_count;  // Number of objects (0 for internal)
    int                 depth;         // Depth in tree (for stats)
} t_bvh_node;
```

**Node Types**:
1. **Internal Node**: `object_count == 0`, `left != NULL`, `right != NULL`
2. **Leaf Node**: `object_count > 0`, `left == NULL`, `right == NULL`, `objects != NULL`

**Validation Rules**:
- Internal nodes MUST have both left and right children
- Leaf nodes MUST have objects array and count > 0
- Node's bounding box MUST contain all objects in subtree

**Traversal Logic**:
```c
if (!aabb_intersect(node->bounds, ray))
    return 0;  // Early out if ray misses bounding box

if (node->object_count > 0)
    return bvh_leaf_intersect(node, ray, hit, scene);  // Leaf: test objects
else
    return check_children(node->left, node->right, ray, hit, scene);  // Internal: recurse
```

---

### 4. t_hit / t_hit_record

**Purpose**: Stores ray-object intersection result

**Location**: `includes/ray.h`

**Structure**:
```c
typedef struct s_hit
{
    bool    hit;        // Whether intersection occurred
    double  distance;   // Distance from ray origin to hit point
    t_vec3  point;      // 3D coordinates of hit point
    t_vec3  normal;     // Surface normal at hit point
    t_color color;      // Object color at hit point
} t_hit;

typedef t_hit t_hit_record;  // Alias for compatibility
```

**Key Insight**: `t_hit_record` is just an alias for `t_hit`, they are the **same type**.

**Field Usage**:
- `hit`: Not used in current BVH code, kept for compatibility
- `distance`: Used to find closest intersection (initialized to INFINITY)
- `point`: Hit point coordinates for lighting calculations
- `normal`: Surface normal for lighting calculations
- `color`: Object's diffuse color

**Validation Rules**:
- `distance` must be positive (in front of ray origin)
- `normal` must be normalized (unit length)
- When comparing hits, choose one with smallest `distance`

---

### 5. t_object_ref

**Purpose**: Reference to any object in scene (sphere, plane, or cylinder)

**Location**: `includes/spatial.h`

**Structure**:
```c
typedef struct s_object_ref
{
    int type;   // Object type: OBJ_SPHERE, OBJ_PLANE, OBJ_CYLINDER
    int index;  // Index into scene's sphere/plane/cylinder array
} t_object_ref;
```

**Usage**:
- BVH stores array of object refs in leaf nodes
- Avoids duplicating object data in tree
- Allows type-safe lookup via `intersect_object()` function

**Type Constants** (from `includes/objects.h`):
```c
#define OBJ_SPHERE   1
#define OBJ_PLANE    2
#define OBJ_CYLINDER 3
```

**Resolution Logic**:
```c
if (ref.type == OBJ_SPHERE)
    return intersect_sphere(&ray, &scene->spheres[ref.index], hit);
else if (ref.type == OBJ_PLANE)
    return intersect_plane(&ray, &scene->planes[ref.index], hit);
else if (ref.type == OBJ_CYLINDER)
    return intersect_cylinder(&ray, &scene->cylinders[ref.index], hit);
```

---

### 6. t_aabb (Axis-Aligned Bounding Box)

**Purpose**: Rectangular box for fast ray intersection testing

**Location**: `includes/spatial.h`

**Structure**:
```c
typedef struct s_aabb
{
    t_vec3 min;  // Minimum corner (x, y, z)
    t_vec3 max;  // Maximum corner (x, y, z)
} t_aabb;
```

**Validation Rules**:
- `min.x <= max.x`, `min.y <= max.y`, `min.z <= max.z` (valid box)
- Empty box can have `min > max` (no objects)

**Operations**:
- `aabb_intersect()`: Test if ray intersects box (fast)
- `aabb_merge()`: Create box containing two boxes
- `aabb_for_sphere()`: Create box around sphere
- `aabb_for_cylinder()`: Create box around cylinder
- `aabb_for_plane()`: Create large box for infinite plane

---

## Entity Relationships

```
t_scene
    └── render_state (t_render_state)
            └── bvh (t_bvh*)
                    └── root (t_bvh_node*)
                            ├── bounds (t_aabb)
                            ├── left (t_bvh_node*)
                            ├── right (t_bvh_node*)
                            └── objects[] (t_object_ref[])
                                    └── resolves to sphere/plane/cylinder in scene

trace_ray() function:
    Input: scene, ray
    Output: color
    Uses: t_hit/t_hit_record for storing intersection results
```

---

## Data Flow

### Scene Loading Flow
```
1. parse_scene() reads .rt file
2. Allocates spheres, planes, cylinders arrays
3. render_state_init() sets bvh_enabled = 1 [AFTER FIX]
4. validate_scene() checks required elements
5. scene_build_bvh() called:
   a. Checks bvh_enabled flag → proceeds [AFTER FIX]
   b. Creates t_object_ref array for all objects
   c. Calls bvh_build() to construct tree
   d. Stores tree in render_state.bvh
```

### Ray Tracing Flow (After Fix)
```
1. trace_ray(scene, ray) called
2. Initialize hit.distance = INFINITY
3. If bvh && bvh->enabled:
   a. Call bvh_intersect(bvh, ray, &hit, scene)
   b. Returns 1 if hit found (updates hit), 0 otherwise
   c. Set hit_found flag if successful
4. If !hit_found (BVH disabled or found nothing):
   a. Call check_sphere_intersections(scene, ray, &hit)
   b. Call check_plane_intersections(scene, ray, &hit)
   c. Call check_cylinder_intersections(scene, ray, &hit)
5. If hit_found:
   a. Return apply_lighting(scene, &hit)
6. Else:
   a. Return black {0, 0, 0}
```

---

## Configuration Changes

**Only one field default value changes**:

```c
// File: src/render/render_state.c
// Function: render_state_init()

// Before (Line 37):
state->bvh_enabled = 0;  // BVH disabled by default

// After (Line 37):
state->bvh_enabled = 1;  // BVH enabled by default
```

**Impact**:
- BVH will be built during scene load (if objects exist)
- Memory usage increases slightly (BVH tree nodes)
- Rendering should be 2-10x faster on complex scenes
- No changes to external data structures or APIs

---

## Conclusion

**No data model changes required**. All structures are correctly defined. The fix involves only:
1. Changing a default flag value (1 line)
2. Fixing control flow logic (6 lines)

All existing structures, relationships, and APIs remain unchanged.
