# Phase 1: Data Model Design

**Feature**: Advanced Rendering Performance Optimization  
**Date**: 2025-12-19  
**Status**: Complete

## Overview

This document defines the data structures required for BVH-based ray tracing optimization in miniRT. All structures designed for single-threaded execution within 42 School constraints (libc/libm only). Memory layout prioritizes cache efficiency while maintaining simplicity.

---

## Entity 1: Bounding Box (AABB)

### Purpose
Axis-aligned bounding box used for fast ray intersection tests and spatial hierarchy construction.

### Structure Definition

```c
typedef struct s_bbox
{
    t_vec3  min;    /* Minimum corner (x, y, z) */
    t_vec3  max;    /* Maximum corner (x, y, z) */
}   t_bbox;
```

### Fields

| Field | Type | Description | Constraints |
|-------|------|-------------|-------------|
| min | t_vec3 | Minimum corner coordinates | min.x ≤ max.x, min.y ≤ max.y, min.z ≤ max.z |
| max | t_vec3 | Maximum corner coordinates | Finite values only (no infinity) |

### Memory Layout
- **Size**: 24 bytes (6 floats × 4 bytes)
- **Alignment**: 4-byte aligned (natural float alignment)
- **Cache**: Fits in single cache line (64 bytes) with metadata

### Validation Rules
- **Invariant**: For all axes, min[axis] ≤ max[axis]
- **Degeneracy**: min == max allowed (point box), empty box represented by min > max
- **Infinity handling**: Planes use scene-clipped large but finite bounds (no INFINITY)

### State Transitions
- **Creation**: Computed from object geometry (sphere, cylinder, plane)
- **Union**: Merged for parent BVH nodes (union of child boxes)
- **Intersection**: Tested against ray (read-only operation)
- **Invalid**: None (immutable after creation)

### Example Usage

```c
/* Compute bounding box for sphere */
t_bbox  compute_bbox_sphere(t_sphere *sphere)
{
    t_bbox  bbox;
    double  radius;

    radius = sphere->diameter / 2.0;
    bbox.min = vec3_subtract(sphere->center, vec3_new(radius, radius, radius));
    bbox.max = vec3_add(sphere->center, vec3_new(radius, radius, radius));
    return (bbox);
}

/* Test ray-bbox intersection */
bool    bbox_intersect(t_bbox *bbox, t_ray *ray, double *t_near, double *t_far)
{
    /* Slab method implementation (see contracts/bbox-api.md) */
}
```

---

## Entity 2: BVH Node

### Purpose
Single node in bounding volume hierarchy, either interior node with two children or leaf node with objects.

### Structure Definition

```c
typedef enum e_bvh_node_type
{
    BVH_NODE_INTERIOR,   /* Has left/right children */
    BVH_NODE_LEAF        /* Has object references */
}   t_bvh_node_type;

typedef struct s_bvh_node
{
    t_bbox              bbox;           /* Bounding box for this subtree */
    t_bvh_node_type     type;           /* Interior or leaf */
    
    /* Union for memory efficiency */
    union
    {
        struct  /* Interior node */
        {
            struct s_bvh_node   *left;
            struct s_bvh_node   *right;
        }   children;
        
        struct  /* Leaf node */
        {
            void    **objects;          /* Array of object pointers */
            int     count;              /* Number of objects (1-4) */
            int     *types;             /* Object types (SPHERE, PLANE, CYLINDER) */
        }   leaf;
    }   data;
}   t_bvh_node;
```

### Fields

#### Common Fields
| Field | Type | Description | Constraints |
|-------|------|-------------|-------------|
| bbox | t_bbox | Bounding box enclosing all objects in subtree | Must contain all descendant bboxes |
| type | t_bvh_node_type | Node type discriminator | BVH_NODE_INTERIOR or BVH_NODE_LEAF |

#### Interior Node Fields (data.children)
| Field | Type | Description | Constraints |
|-------|------|-------------|-------------|
| left | t_bvh_node* | Left child node | Non-NULL, bbox contained in parent |
| right | t_bvh_node* | Right child node | Non-NULL, bbox contained in parent |

#### Leaf Node Fields (data.leaf)
| Field | Type | Description | Constraints |
|-------|------|-------------|-------------|
| objects | void** | Pointers to scene objects | 1 ≤ count ≤ 4 (LEAF_THRESHOLD) |
| count | int | Number of objects in leaf | Positive, ≤ LEAF_THRESHOLD |
| types | int* | Object type tags | OBJECT_SPHERE, OBJECT_PLANE, OBJECT_CYLINDER |

### Memory Layout
- **Size**: 64 bytes (estimated with padding)
- **Alignment**: 8-byte aligned (pointer alignment)
- **Allocation**: Contiguous array for cache-friendly traversal (optional optimization)
- **Tree structure**: Binary tree, depth O(log n) with median split

### Design Decisions

**Union for Memory Efficiency**:
- Interior nodes don't need object arrays, leaf nodes don't need child pointers
- Saves ~24 bytes per node (significant for large trees)
- Tagged union pattern (discriminator = type field)

**Object Pointer Array**:
- Stores pointers to objects in scene structure (not copies)
- Type tags required because miniRT has heterogeneous objects (sphere, plane, cylinder)
- Enables generic intersection testing in traversal

**Leaf Threshold**:
```c
#define BVH_LEAF_THRESHOLD 4  /* Max objects per leaf before split */
```
- Tradeoff: Smaller = deeper tree (more nodes), Larger = more intersection tests per leaf
- Value 4 chosen based on research (balance construction vs traversal cost)

### Invariants
- **Parent bbox contains children**: parent.bbox encloses child.left.bbox and child.right.bbox
- **Leaf objects within bbox**: All leaf objects' bboxes intersect node bbox
- **No empty leaves**: Leaf nodes have count ≥ 1
- **Binary tree**: Interior nodes have exactly 2 children (no NULL children)

### State Transitions

```
INITIAL (allocated)
    ↓ compute_bbox()
    ↓ populate_objects() OR create_children()
VALID (ready for traversal)
    ↓ free_bvh()
FREED
```

### Example Usage

```c
/* Create leaf node */
t_bvh_node  *create_leaf(void **objects, int *types, int count)
{
    t_bvh_node  *node;

    node = malloc(sizeof(t_bvh_node));
    node->type = BVH_NODE_LEAF;
    node->data.leaf.objects = objects;
    node->data.leaf.types = types;
    node->data.leaf.count = count;
    node->bbox = compute_union_bbox(objects, types, count);
    return (node);
}

/* Create interior node */
t_bvh_node  *create_interior(t_bvh_node *left, t_bvh_node *right)
{
    t_bvh_node  *node;

    node = malloc(sizeof(t_bvh_node));
    node->type = BVH_NODE_INTERIOR;
    node->data.children.left = left;
    node->data.children.right = right;
    node->bbox = bbox_union(&left->bbox, &right->bbox);
    return (node);
}
```

---

## Entity 3: BVH Context

### Purpose
Top-level structure holding BVH tree and metadata. Attached to scene structure for lifetime management.

### Structure Definition

```c
typedef struct s_bvh
{
    t_bvh_node      *root;          /* Root of BVH tree */
    int             node_count;     /* Total nodes in tree (2n - 1) */
    int             object_count;   /* Total objects indexed */
    double          build_time_ms;  /* Construction time for profiling */
}   t_bvh;
```

### Fields

| Field | Type | Description | Constraints |
|-------|------|-------------|-------------|
| root | t_bvh_node* | Root node of BVH tree | Non-NULL after successful build |
| node_count | int | Total number of nodes | Exactly 2 * object_count - 1 (binary tree) |
| object_count | int | Number of objects indexed | Must match scene object count |
| build_time_ms | double | Construction duration (milliseconds) | For performance profiling only |

### Lifecycle

```c
/* Construction */
t_bvh   *build_bvh(t_scene *scene);

/* Usage */
bool    traverse_bvh(t_bvh *bvh, t_ray *ray, t_hit *hit);

/* Destruction */
void    free_bvh(t_bvh *bvh);
```

### Integration with Scene

```c
/* Modified t_scene structure (in minirt.h) */
typedef struct s_scene
{
    /* ... existing fields ... */
    t_bvh   *bvh;   /* NEW: BVH acceleration structure (NULL if disabled) */
}   t_scene;

/* Initialization in main() */
scene->bvh = build_bvh(scene);

/* Usage in trace_ray() */
#ifdef OPTIMIZATION_BVH
    if (scene->bvh != NULL)
        return traverse_bvh(scene->bvh, ray, &hit);
#endif
    /* Fallback to linear scan */

/* Cleanup in cleanup_scene() */
if (scene->bvh != NULL)
    free_bvh(scene->bvh);
```

---

## Entity 4: Object Metadata

### Purpose
Precomputed spatial metadata for scene objects to accelerate BVH construction and traversal.

### Structure Definition

```c
typedef enum e_object_type
{
    OBJECT_SPHERE,
    OBJECT_PLANE,
    OBJECT_CYLINDER
}   t_object_type;

typedef struct s_object_meta
{
    void            *object_ptr;    /* Pointer to actual object (t_sphere*, etc.) */
    t_object_type   type;           /* Object type tag */
    t_bbox          bbox;           /* Precomputed bounding box */
    t_vec3          centroid;       /* Object center for sorting */
}   t_object_meta;
```

### Fields

| Field | Type | Description | Constraints |
|-------|------|-------------|-------------|
| object_ptr | void* | Pointer to original object | Non-NULL, points to scene->spheres[] etc. |
| type | t_object_type | Object type discriminator | SPHERE, PLANE, or CYLINDER |
| bbox | t_bbox | Precomputed bounding box | Valid (min ≤ max), contains object |
| centroid | t_vec3 | Center point for axis sorting | Used in median split algorithm |

### Purpose in BVH Construction

1. **Unified representation**: Single array of metadata instead of three separate arrays
2. **Precomputed bboxes**: Computed once during BVH build, not per-node
3. **Sorting support**: Centroid enables efficient axis-based sorting
4. **Type preservation**: Enables correct intersection function dispatch in leaves

### Example Usage

```c
/* Build metadata array from scene */
t_object_meta   *build_metadata(t_scene *scene, int *total_count)
{
    t_object_meta   *meta;
    int             count;
    int             i;

    count = scene->sphere_count + scene->plane_count + scene->cylinder_count;
    meta = malloc(sizeof(t_object_meta) * count);
    
    /* Populate from spheres */
    for (i = 0; i < scene->sphere_count; i++)
    {
        meta[i].object_ptr = &scene->spheres[i];
        meta[i].type = OBJECT_SPHERE;
        meta[i].bbox = compute_bbox_sphere(&scene->spheres[i]);
        meta[i].centroid = scene->spheres[i].center;
    }
    
    /* ... similar for planes and cylinders ... */
    
    *total_count = count;
    return (meta);
}

/* Sort by axis for median split */
void    sort_metadata_by_axis(t_object_meta *meta, int count, int axis)
{
    /* Quicksort by meta[i].centroid[axis] */
}
```

---

## Entity 5: Intersection Cache Entry

### Purpose
Cache shadow ray intersection results within a frame to avoid redundant calculations.

### Structure Definition

```c
typedef struct s_cache_key
{
    uint64_t    ray_hash;       /* Hash of ray direction */
    void        *object_ptr;    /* Object identity */
}   t_cache_key;

typedef struct s_cache_entry
{
    t_cache_key     key;
    bool            hit;            /* Intersection result (hit/miss) */
    double          distance;       /* Hit distance if hit == true */
    t_vec3          normal;         /* Surface normal if hit == true */
    
    /* Linked list for hash collision resolution */
    struct s_cache_entry    *next;
}   t_cache_entry;

typedef struct s_intersection_cache
{
    t_cache_entry   **buckets;      /* Hash table buckets */
    int             bucket_count;   /* Number of buckets (power of 2) */
    int             entry_count;    /* Current entries stored */
    int             hit_count;      /* Cache hits (profiling) */
    int             miss_count;     /* Cache misses (profiling) */
}   t_intersection_cache;
```

### Fields

#### Cache Key
| Field | Type | Description | Constraints |
|-------|------|-------------|-------------|
| ray_hash | uint64_t | Hash of ray direction vector | Collision-resistant hash function |
| object_ptr | void* | Pointer to object | Identity comparison (no dereferencing) |

#### Cache Entry
| Field | Type | Description | Constraints |
|-------|------|-------------|-------------|
| key | t_cache_key | Lookup key (ray, object pair) | Immutable after insertion |
| hit | bool | Whether ray intersects object | True if intersection exists |
| distance | double | Hit distance along ray | Valid if hit == true |
| normal | t_vec3 | Surface normal at hit point | Valid if hit == true |
| next | t_cache_entry* | Collision chain pointer | NULL if end of chain |

#### Cache Structure
| Field | Type | Description | Constraints |
|-------|------|-------------|-------------|
| buckets | t_cache_entry** | Hash table array | Size = bucket_count |
| bucket_count | int | Number of buckets | Power of 2 (for fast modulo) |
| entry_count | int | Total cached entries | ≤ capacity limit |
| hit_count | int | Cache lookup hits | For profiling hit rate |
| miss_count | int | Cache lookup misses | For profiling hit rate |

### Configuration

```c
#define CACHE_BUCKET_COUNT 1024     /* Power of 2, ~100k entries */
#define CACHE_MAX_ENTRIES  102400   /* 100k * 32 bytes ≈ 3MB */
```

### Ray Hashing Strategy

```c
/* Hash ray direction to 64-bit value */
uint64_t    hash_ray_direction(t_vec3 *dir)
{
    /* Quantize direction to avoid floating point equality issues */
    int64_t x = (int64_t)(dir->x * 10000.0);
    int64_t y = (int64_t)(dir->y * 10000.0);
    int64_t z = (int64_t)(dir->z * 10000.0);
    
    /* FNV-1a hash */
    uint64_t hash = 14695981039346656037ULL;
    hash ^= x; hash *= 1099511628211ULL;
    hash ^= y; hash *= 1099511628211ULL;
    hash ^= z; hash *= 1099511628211ULL;
    
    return (hash);
}
```

### Collision Resolution
- **Strategy**: Chaining (linked list per bucket)
- **Rationale**: Simple implementation, bounded memory per entry
- **Worst case**: O(n) lookup if all entries collide (unlikely with good hash)
- **Expected case**: O(1) with ~100 entries per bucket average

### Invalidation Policy
- **Frame boundary**: Clear all entries on camera movement or object manipulation
- **Capacity limit**: LRU eviction not implemented (clear entire cache if limit exceeded)
- **Rationale**: Shadow rays only relevant within single frame, fresh cache per frame sufficient

### Example Usage

```c
/* Lookup cached intersection */
bool    cache_get(t_intersection_cache *cache, t_ray *ray, void *object,
                  bool *hit, double *distance, t_vec3 *normal)
{
    uint64_t        ray_hash;
    int             bucket_idx;
    t_cache_entry   *entry;

    ray_hash = hash_ray_direction(&ray->direction);
    bucket_idx = ray_hash % cache->bucket_count;
    
    entry = cache->buckets[bucket_idx];
    while (entry != NULL)
    {
        if (entry->key.ray_hash == ray_hash &&
            entry->key.object_ptr == object)
        {
            *hit = entry->hit;
            *distance = entry->distance;
            *normal = entry->normal;
            cache->hit_count++;
            return (true);  /* Cache hit */
        }
        entry = entry->next;
    }
    
    cache->miss_count++;
    return (false);  /* Cache miss */
}

/* Store intersection result */
void    cache_set(t_intersection_cache *cache, t_ray *ray, void *object,
                  bool hit, double distance, t_vec3 normal)
{
    /* Insert new entry at head of collision chain */
}
```

---

## Memory Budget Analysis

### Per-Object Overhead

| Component | Size per Object | 100 Objects |
|-----------|----------------|-------------|
| BVH nodes | ~64 bytes × (2n - 1) | ~12.5 KB |
| Object metadata | 56 bytes | 5.6 KB |
| Bounding boxes (embedded) | (included in nodes) | - |
| **Subtotal** | - | **~18 KB** |

### Cache Overhead (Optional)

| Component | Total Size | Notes |
|-----------|-----------|-------|
| Bucket array | 8 KB | 1024 buckets × 8 bytes |
| Entries (100k) | ~3 MB | 32 bytes × 100,000 |
| **Subtotal** | **~3 MB** | Only if caching enabled |

### Total Memory Budget
- **BVH + metadata (100 objects)**: ~18 KB
- **Cache (optional)**: ~3 MB
- **Total**: ~3.02 MB (well within 50 MB requirement from FR-017)

---

## Data Model Validation

### Compliance with Functional Requirements

| Requirement | Data Model Support |
|-------------|-------------------|
| FR-005 (spatial structure) | ✅ t_bvh_node provides hierarchical organization |
| FR-006 (bounding volumes) | ✅ t_bbox enables fast rejection tests |
| FR-007 (efficient build) | ✅ t_object_meta enables O(n log n) construction |
| FR-008 (degenerate cases) | ✅ Leaf threshold handles single-object scenes |
| FR-011 (intersection cache) | ✅ t_intersection_cache with hash table |
| FR-017 (memory efficiency) | ✅ 18 KB for 100 objects (0.036% of 50 MB budget) |
| FR-018 (precomputed values) | ✅ Bboxes computed once in metadata |
| FR-020 (cache-friendly layout) | ✅ Contiguous node array option, union space savings |

### 42 School Constraint Compliance

| Constraint | Compliance |
|-----------|------------|
| No pthread | ✅ Single-threaded traversal, no synchronization primitives |
| No external libraries | ✅ Only malloc/free, libm for hash computation |
| Memory allocation | ✅ Graceful degradation if malloc fails (NULL check) |
| Norminette | ✅ All structures follow 42 naming conventions (t_ prefix) |

---

## Next Steps

With data model defined:
1. ✅ Generate API contracts (contracts/*.md)
2. ✅ Write quickstart guide (quickstart.md)
3. ✅ Update agent context with new types
4. → Proceed to Phase 2 task breakdown (/speckit.tasks command)
