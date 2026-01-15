# Data Model: Complex Scene Performance Optimization

**Feature**: 011-performance-optimization  
**Date**: 2026-01-01  
**Purpose**: Define data structures for BVH acceleration, performance metrics, and optimized intersection tests

## Core Entities

### 1. BVH Node (t_bvh_node)

**Purpose**: Represents a single node in the Bounding Volume Hierarchy tree

**Fields**:
```c
typedef struct s_bvh_node
{
    t_aabb          bbox;           // Axis-aligned bounding box for this node
    struct s_bvh_node *left;        // Left child (NULL for leaf)
    struct s_bvh_node *right;       // Right child (NULL for leaf)
    t_object        **objects;      // Array of object pointers (leaf only)
    int             object_count;   // Number of objects in leaf
    bool            is_leaf;        // Flag: true if leaf node
    bool            is_dirty;       // Flag: true if needs rebuild
    int             depth;          // Depth in tree (root = 0)
} t_bvh_node;
```

**Relationships**:
- Each node has 0-2 child nodes (binary tree)
- Leaf nodes have 1+ object references
- Internal nodes have 2 children and no objects
- Root node contains all scene objects

**Validation Rules**:
- `left` and `right` must both be NULL (leaf) or both be non-NULL (internal)
- `objects` array must be NULL for internal nodes
- `object_count` must be 0 for internal nodes, >0 for leaves
- `depth` must not exceed 64 (MAX_BVH_DEPTH)
- `bbox` must fully contain all descendant objects

**State Transitions**:
- **Construction**: NULL → Internal Node → Leaf (recursive splitting)
- **Dynamic update**: Clean → Dirty (object transform) → Clean (refit/rebuild)
- **Destruction**: Leaf/Internal → NULL (bottom-up deallocation)

---

### 2. AABB (t_aabb)

**Purpose**: Axis-aligned bounding box for fast spatial queries

**Fields**:
```c
typedef struct s_aabb
{
    t_vec3  min;    // Minimum corner (x, y, z)
    t_vec3  max;    // Maximum corner (x, y, z)
} t_aabb;
```

**Relationships**:
- Each BVH node has exactly one AABB
- Each scene object has a precomputed AABB
- Parent node AABB fully contains child AABBs

**Validation Rules**:
- `min.x <= max.x`, `min.y <= max.y`, `min.z <= max.z`
- All values must be finite (no NaN, no infinity)
- Surface area must be >= 0 (degenerate boxes allowed)

**Operations**:
- `aabb_union(a, b)`: Returns AABB containing both a and b
- `aabb_surface_area(a)`: Returns 2 × (width×height + width×depth + height×depth)
- `aabb_intersect_ray(a, ray, t_min, t_max)`: Returns true if ray hits box in interval [t_min, t_max]
- `aabb_from_object(obj)`: Computes tight AABB for object (sphere/plane/cylinder)

---

### 3. Performance Metrics (t_perf_metrics)

**Purpose**: Collects and aggregates real-time rendering performance data

**Fields**:
```c
typedef struct s_perf_metrics
{
    uint64_t    frame_times[60];        // Ring buffer: frame durations (μs)
    uint64_t    bvh_traverse_times[60]; // Ring buffer: BVH traversal durations (μs)
    int         buffer_index;           // Current position in ring buffer
    uint64_t    frame_start_time;       // Timestamp at frame start (μs)
    uint64_t    bvh_traverse_start;     // Timestamp at traversal start (μs)
    int         ray_count;              // Total rays cast this frame
    int         intersection_count;     // Total intersection tests this frame
    int         bvh_node_visits;        // Total BVH nodes visited this frame
    int         frame_count;            // Total frames rendered since start
    bool        display_enabled;        // Flag: show metrics HUD
} t_perf_metrics;
```

**Relationships**:
- One metrics instance per renderer instance
- Updated once per frame
- Read by HUD display code

**Validation Rules**:
- `buffer_index` must be in range [0, 59]
- All time values must be monotonically increasing
- Counters (ray_count, etc.) reset to 0 at frame start
- `frame_times[i]` must be > 0 (non-zero frame duration)

**State Transitions**:
- **Frame start**: Reset counters, capture `frame_start_time`
- **BVH traversal start**: Capture `bvh_traverse_start`
- **BVH traversal end**: Compute duration, store in ring buffer
- **Frame end**: Compute total frame time, store in ring buffer, increment `buffer_index`
- **Display toggle**: `display_enabled` ↔ false/true (via 'I' key)

**Derived metrics**:
```c
// Computed from ring buffer data
float fps_current = 1000000.0 / frame_times[buffer_index];
float fps_average = 1000000.0 / average(frame_times);
float frame_ms_current = frame_times[buffer_index] / 1000.0;
float frame_ms_average = average(frame_times) / 1000.0;
float bvh_percent = (bvh_traverse_times[buffer_index] * 100.0) / frame_times[buffer_index];
```

---

### 4. Ray (t_ray) - Extended

**Purpose**: Represents a light ray with metadata for BVH traversal

**Fields** (existing + new):
```c
typedef struct s_ray
{
    t_vec3      origin;         // Ray starting point
    t_vec3      direction;      // Ray direction (normalized)
    float       t_min;          // Minimum intersection distance
    float       t_max;          // Maximum intersection distance
    t_ray_type  type;           // PRIMARY, SHADOW, or REFLECTION (new)
    int         depth;          // Recursion depth (for reflections) (new)
} t_ray;

typedef enum e_ray_type
{
    RAY_PRIMARY,        // Camera to scene
    RAY_SHADOW,         // Surface to light
    RAY_REFLECTION      // Secondary bounce
} t_ray_type;
```

**Relationships**:
- Generated by camera (primary rays)
- Generated by shading code (shadow/reflection rays)
- Used by BVH traversal

**Validation Rules**:
- `direction` must be normalized (length = 1.0)
- `t_min >= 0` (no negative intersections)
- `t_max > t_min` (valid interval)
- `depth >= 0` and `depth <= MAX_RECURSION_DEPTH` (typically 5)

**State Transitions**:
- **Primary ray**: Generated from camera → Traverses BVH → Hits object → Generates shadow/reflection rays
- **Shadow ray**: Generated from hit point → Traverses BVH → Hits/misses light
- **Reflection ray**: Generated from hit point → Traverses BVH → Recursion depth check

---

### 5. Intersection Record (t_hit_record) - Extended

**Purpose**: Stores detailed information about a ray-object intersection

**Fields** (existing + new):
```c
typedef struct s_hit_record
{
    float       t;              // Distance along ray to hit point
    t_vec3      point;          // 3D hit point coordinates
    t_vec3      normal;         // Surface normal at hit point
    t_object    *object;        // Pointer to hit object
    t_material  *material;      // Pointer to object's material
    bool        front_face;     // True if ray hit front face (new)
    int         test_count;     // Number of intersection tests performed (new)
} t_hit_record;
```

**Relationships**:
- One record per ray (closest hit only)
- References scene object and material
- Used by shading/lighting calculations

**Validation Rules**:
- `t > 0` (valid hit distance)
- `point` must be on object surface (within floating-point epsilon)
- `normal` must be normalized (length = 1.0)
- `object` and `material` must be non-NULL for valid hit
- `test_count > 0` for valid hit

**State Transitions**:
- **No hit**: All fields undefined
- **Hit found**: Fields populated, `t` = closest distance
- **Closer hit found**: Fields updated with closer intersection

---

### 6. BVH Allocator (t_bvh_allocator)

**Purpose**: Tracks memory usage during BVH construction to enforce 512 MB limit

**Fields**:
```c
typedef struct s_bvh_allocator
{
    size_t  total_allocated;    // Total bytes allocated for BVH
    size_t  max_budget;         // Maximum allowed bytes (512 MB)
    int     node_count;         // Total BVH nodes allocated
    bool    limit_exceeded;     // Flag: true if budget exceeded
} t_bvh_allocator;
```

**Relationships**:
- One allocator per BVH construction process
- Used by `allocate_node()` function
- Monitored during construction

**Validation Rules**:
- `total_allocated <= max_budget` (enforced at allocation time)
- `max_budget` = 512 × 1024 × 1024 (constant)
- `node_count` = `total_allocated / sizeof(t_bvh_node)` (approximately)

**State Transitions**:
- **Initial**: `total_allocated = 0`, `node_count = 0`, `limit_exceeded = false`
- **Allocate node**: `total_allocated += sizeof(t_bvh_node)`, `node_count++`
- **Limit exceeded**: `limit_exceeded = true`, abort construction, trigger fallback

---

### 7. BVH Build Info (t_bvh_build_info)

**Purpose**: Configuration parameters for BVH construction

**Fields**:
```c
typedef struct s_bvh_build_info
{
    int         max_depth;          // Maximum tree depth (64)
    int         min_objects_leaf;   // Minimum objects for leaf (4)
    int         sah_bins;           // Number of bins for SAH (16)
    float       traversal_cost;     // Relative cost of BVH traversal (1.0)
    float       intersection_cost;  // Relative cost of intersection test (10.0)
} t_bvh_build_info;
```

**Relationships**:
- One build info per BVH construction
- Read by SAH evaluation and splitting logic

**Validation Rules**:
- `max_depth` = 64 (constant, prevents stack overflow)
- `min_objects_leaf` >= 1 (at least one object per leaf)
- `sah_bins` = 16 (good quality/speed tradeoff)
- `traversal_cost` > 0 and `intersection_cost` > 0 (positive costs)
- `intersection_cost > traversal_cost` (intersection is more expensive)

---

## Data Flow Diagrams

### BVH Construction Flow
```
Scene Objects → Compute AABBs → Build BVH Tree → Store Root Node
                    ↓
                Allocator (track memory)
                    ↓
            SAH Evaluation (choose splits)
                    ↓
        Recursive Split (until leaf criteria)
```

### Ray Traversal Flow
```
Ray → BVH Root → Test AABB → Hit?
                      ↓            ↓
                     Yes          No
                      ↓            ↓
               Internal Node?   Skip subtree
                      ↓
                 Yes    No (Leaf)
                  ↓         ↓
           Visit children  Test objects → Update hit record
```

### Performance Metrics Flow
```
Frame Start → Reset counters
    ↓
Ray casting → Increment ray_count
    ↓
BVH traversal → Start timer → Traverse → End timer → Store duration
    ↓
Intersection tests → Increment intersection_count
    ↓
Frame End → Compute frame time → Store in ring buffer → Update display
```

---

## Memory Layout Optimization

### BVH Node Cache Optimization
```c
// Organize frequently accessed fields first (cache line = 64 bytes)
struct s_bvh_node {
    t_aabb          bbox;       // 24 bytes (accessed every traversal step)
    t_bvh_node      *left;      // 8 bytes  (accessed for internal nodes)
    t_bvh_node      *right;     // 8 bytes  (accessed for internal nodes)
    // --- 40 bytes (fits in first cache line) ---
    t_object        **objects;  // 8 bytes  (accessed only for leaves)
    int             object_count; // 4 bytes
    bool            is_leaf;    // 1 byte
    bool            is_dirty;   // 1 byte
    int             depth;      // 4 bytes
    // --- 58 bytes total (rounded to 64 for alignment) ---
};
```

**Rationale**: First 40 bytes (bbox + children) accessed on every traversal step. Placing them at start of structure maximizes cache hit rate.

---

## Size Estimates

### Memory Usage
- **BVH Node**: 64 bytes per node
- **Scene with 50 objects**: ~100 BVH nodes = 6.4 KB
- **Scene with 1000 objects**: ~2000 BVH nodes = 128 KB
- **Maximum (512 MB limit)**: 8,388,608 nodes (unrealistic for ray tracer)

### Performance Metrics Memory
- **Ring buffer**: 60 × 8 bytes × 2 = 960 bytes
- **Counters**: ~50 bytes
- **Total**: ~1 KB (negligible)

---

## Summary

**Total entities**: 7 (BVH Node, AABB, Performance Metrics, Ray, Hit Record, Allocator, Build Info)

**Key relationships**:
- BVH Node → AABB (1:1)
- BVH Node → Objects (1:many, leaf only)
- BVH Node → Children (1:2, internal only)
- Ray → Hit Record (1:1 per traversal)

**Critical validation**:
- BVH depth <= 64 (prevent stack overflow)
- Memory usage <= 512 MB (enforce limit)
- All AABBs valid (min <= max on all axes)

**Next steps**: Generate API contracts for BVH operations
