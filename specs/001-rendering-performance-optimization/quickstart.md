# Quick Start: Rendering Performance Optimization

**Feature**: Advanced Rendering Performance Optimization  
**Date**: 2025-12-19  
**Audience**: Developers implementing or maintaining BVH optimization

## Overview

This guide provides step-by-step instructions for integrating BVH-based ray tracing optimization into miniRT. Includes compilation, testing, and troubleshooting.

---

## Prerequisites

- ✅ miniRT baseline implementation working (renders scenes correctly)
- ✅ gcc or clang compiler with C99/C11 support
- ✅ MiniLibX library installed
- ✅ make build system configured
- ✅ Test scenes in `scenes/` directory

---

## Step 1: Enable Optimization (Compile-Time Flag)

### Option A: Makefile Configuration (Recommended)

```makefile
# In Makefile, add optimization flags
CFLAGS += -DOPTIMIZATION_BVH       # Enable BVH acceleration
CFLAGS += -DOPTIMIZATION_CACHE     # Enable intersection caching (optional)

# Add new source files
SRCS += src/optimization/bvh.c \
        src/optimization/bvh_traverse.c \
        src/optimization/bbox.c \
        src/optimization/spatial_sort.c \
        src/math/bbox_math.c

# Optional: Add cache implementation
# SRCS += src/optimization/intersection_cache.c

# Add new header path
INCLUDES += -Iincludes
```

### Option B: Manual Compilation

```bash
gcc -DOPTIMIZATION_BVH \
    src/main.c \
    src/render/*.c \
    src/optimization/*.c \
    src/math/*.c \
    -Iincludes \
    -lm -lmlx -lX11 -lXext \
    -o miniRT
```

---

## Step 2: Update Header Files

### Create `includes/optimization.h`

```c
#ifndef OPTIMIZATION_H
# define OPTIMIZATION_H

# include "minirt.h"
# include "vec3.h"
# include "ray.h"

/* BVH node types */
typedef enum e_bvh_node_type
{
    BVH_NODE_INTERIOR,
    BVH_NODE_LEAF
}   t_bvh_node_type;

/* Bounding box (AABB) */
typedef struct s_bbox
{
    t_vec3  min;
    t_vec3  max;
}   t_bbox;

/* BVH node */
typedef struct s_bvh_node
{
    t_bbox              bbox;
    t_bvh_node_type     type;
    
    union
    {
        struct
        {
            struct s_bvh_node   *left;
            struct s_bvh_node   *right;
        }   children;
        
        struct
        {
            void    **objects;
            int     count;
            int     *types;
        }   leaf;
    }   data;
}   t_bvh_node;

/* BVH context */
typedef struct s_bvh
{
    t_bvh_node  *root;
    int         node_count;
    int         object_count;
    double      build_time_ms;
}   t_bvh;

/* BVH API */
t_bvh   *build_bvh(t_scene *scene);
bool    traverse_bvh(t_bvh *bvh, t_ray *ray, t_hit *hit);
void    free_bvh(t_bvh *bvh);

/* Bounding box API */
t_bbox  compute_bbox_sphere(t_sphere *sphere);
t_bbox  compute_bbox_plane(t_plane *plane, t_bbox *scene_bounds);
t_bbox  compute_bbox_cylinder(t_cylinder *cylinder);
bool    bbox_intersect(t_bbox *bbox, t_ray *ray, double *t_near, double *t_far);
t_bbox  bbox_union(t_bbox *a, t_bbox *b);

#endif
```

### Modify `includes/minirt.h`

```c
/* Add after other includes */
#ifdef OPTIMIZATION_BVH
# include "optimization.h"
#endif

/* Modify t_scene structure */
typedef struct s_scene
{
    /* ... existing fields ... */
    
#ifdef OPTIMIZATION_BVH
    t_bvh   *bvh;  /* NEW: BVH acceleration structure */
#endif
}   t_scene;
```

---

## Step 3: Integrate into Main

### Modify `src/main.c`

```c
#include "minirt.h"
#ifdef OPTIMIZATION_BVH
# include "optimization.h"
#endif

int main(int argc, char **argv)
{
    t_scene *scene;
    
    /* Parse scene file */
    scene = parse_scene(argv[1]);
    if (scene == NULL)
        return (print_error("Failed to parse scene"));
    
#ifdef OPTIMIZATION_BVH
    /* Build BVH after scene parsing */
    scene->bvh = build_bvh(scene);
    if (scene->bvh == NULL)
    {
        printf("Warning: BVH construction failed, using linear scan\n");
    }
    else
    {
        printf("BVH built: %d objects, %d nodes (%.2fms)\n",
               scene->bvh->object_count,
               scene->bvh->node_count,
               scene->bvh->build_time_ms);
    }
#else
    scene->bvh = NULL;
#endif
    
    /* ... rest of main ... */
}
```

---

## Step 4: Integrate into Render Pipeline

### Modify `src/render/trace.c`

```c
t_color trace_ray(t_scene *scene, t_ray *ray)
{
    t_hit   hit;
    
    hit.distance = INFINITY;
    
#ifdef OPTIMIZATION_BVH
    /* Use BVH if available */
    if (scene->bvh != NULL)
    {
        if (traverse_bvh(scene->bvh, ray, &hit))
            return apply_lighting(scene, &hit);
        else
            return (t_color){0, 0, 0};  /* Background */
    }
#endif
    
    /* Fallback: original linear scan */
    for (int i = 0; i < scene->sphere_count; i++)
        intersect_sphere(ray, &scene->spheres[i], &hit);
    
    for (int i = 0; i < scene->plane_count; i++)
        intersect_plane(ray, &scene->planes[i], &hit);
    
    for (int i = 0; i < scene->cylinder_count; i++)
        intersect_cylinder(ray, &scene->cylinders[i], &hit);
    
    if (hit.distance < INFINITY)
        return apply_lighting(scene, &hit);
    else
        return (t_color){0, 0, 0};
}
```

### Modify `src/utils/cleanup.c`

```c
void    cleanup_scene(t_scene *scene)
{
    if (scene == NULL)
        return;
    
#ifdef OPTIMIZATION_BVH
    /* Free BVH if present */
    if (scene->bvh != NULL)
    {
        free_bvh(scene->bvh);
        scene->bvh = NULL;
    }
#endif
    
    /* ... rest of cleanup ... */
}
```

---

## Step 5: Build and Test

### Build

```bash
make re
```

### Expected Output

```
gcc -Wall -Wextra -Werror -DOPTIMIZATION_BVH ...
Compiling src/optimization/bvh.c
Compiling src/optimization/bvh_traverse.c
Compiling src/optimization/bbox.c
Linking miniRT
```

### Test with Simple Scene

```bash
./miniRT scenes/simple.rt
```

### Expected Console Output

```
BVH built: 5 objects, 9 nodes (2.34ms)
Rendering scene...
```

### Visual Validation
- Rendered image should be **pixel-perfect identical** to non-optimized version
- Camera movement should feel more responsive

---

## Step 6: Performance Measurement

### Create Performance Test Script

```bash
#!/bin/bash
# test_performance.sh

echo "Testing rendering performance..."

for scene in scenes/optimization/*.rt; do
    echo "Scene: $scene"
    time ./miniRT "$scene" > /dev/null 2>&1
done
```

### Run Performance Tests

```bash
chmod +x test_performance.sh
./test_performance.sh
```

### Expected Results

| Scene | Objects | Without BVH | With BVH | Improvement |
|-------|---------|------------|----------|-------------|
| simple_5.rt | 5 | 2.5s | 1.2s | 52% faster |
| medium_20.rt | 20 | 8.3s | 2.1s | 75% faster |
| complex_50.rt | 50 | 35.2s | 5.8s | 83% faster |

---

## Step 7: Validation

### Visual Quality Check

```bash
# Render with BVH
./miniRT scenes/test.rt

# Disable BVH (edit Makefile, remove -DOPTIMIZATION_BVH)
make re
./miniRT scenes/test.rt

# Compare output images (should be identical)
```

### Memory Leak Check

```bash
valgrind --leak-check=full --show-leak-kinds=all ./miniRT scenes/test.rt
```

### Expected Output

```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap usage: X allocs, X frees, Y bytes allocated
==12345==
==12345== All heap blocks were freed -- no leaks are possible
```

### Norminette Check

```bash
norminette src/optimization/*.c includes/optimization.h
```

---

## Troubleshooting

### Issue: BVH Construction Fails (returns NULL)

**Symptoms**:
```
Warning: BVH construction failed, using linear scan
```

**Possible Causes**:
1. malloc failure (out of memory)
2. Scene has 0 objects
3. Invalid object geometry (e.g., sphere diameter = 0)

**Debug Steps**:
```c
/* In build_bvh(), add debug prints */
printf("Building BVH for %d objects\n", total_count);
if (metadata == NULL)
    printf("ERROR: Failed to allocate metadata\n");
```

**Fix**: Check scene file validity, verify available memory

---

### Issue: Visual Artifacts (Different Output with BVH)

**Symptoms**: Rendered image differs from linear scan version

**Possible Causes**:
1. Bbox computation incorrect (objects missing from hierarchy)
2. Traversal order bug (skipping valid intersections)
3. Floating point precision issue in bbox_intersect

**Debug Steps**:
```c
/* In traverse_bvh(), count intersection tests */
static int test_count = 0;
printf("BVH tests: %d, Linear tests: %d\n", test_count, object_count);
```

**Fix**: Verify bbox computation with unit tests, check epsilon values

---

### Issue: Performance Worse Than Linear Scan

**Symptoms**: BVH rendering slower than without optimization

**Possible Causes**:
1. Scene too simple (overhead > benefit for <5 objects)
2. BVH poorly balanced (all objects in one subtree)
3. Bbox computation expensive (e.g., plane bbox calculation)

**Debug Steps**:
```bash
# Profile with gprof
gcc -pg -DOPTIMIZATION_BVH ...
./miniRT scenes/test.rt
gprof miniRT gmon.out
```

**Fix**: Disable BVH for scenes with <10 objects, check tree balance

---

### Issue: Memory Usage Too High

**Symptoms**: Program crashes or system slows down

**Possible Causes**:
1. Memory leak in BVH construction
2. Large scenes (>1000 objects)
3. Cache enabled with excessive entries

**Debug Steps**:
```bash
# Monitor memory usage
/usr/bin/time -v ./miniRT scenes/test.rt
```

**Fix**: Run valgrind, disable cache, check free_bvh() implementation

---

## Performance Tuning

### Adjust Leaf Threshold

```c
/* In optimization.h */
#define BVH_LEAF_THRESHOLD 4  /* Default: 4 objects per leaf */

/* Try different values:
 * 2 = deeper tree, more nodes, faster traversal
 * 8 = shallower tree, fewer nodes, more leaf tests
 * Optimal value depends on scene complexity
 */
```

### Experiment with Split Axis Selection

```c
/* In bvh.c, function choose_split_axis() */

/* Current: Longest axis */
axis = bbox_longest_axis(&node_bbox);

/* Alternative: Cycle through axes */
axis = (depth % 3);

/* Alternative: Always split X (fastest build) */
axis = 0;
```

---

## Scene File Recommendations

### Good Scenes for BVH

```
# Many small objects clustered in space
A 0.2 255,255,255
C 0,0,-10 0,0,1 70
L 0,10,0 0.7 255,255,255

sp 0,0,0 2 255,0,0
sp 2,0,0 2 0,255,0
sp 4,0,0 2 0,0,255
# ... 20+ more spheres ...
```

### Poor Scenes for BVH (Overhead Not Worth It)

```
# Only 2 objects - linear scan faster
A 0.2 255,255,255
C 0,0,-10 0,0,1 70
L 0,10,0 0.7 255,255,255

sp 0,0,0 2 255,0,0
pl 0,-1,0 0,1,0 200,200,200
```

---

## Advanced: Profiling and Metrics

### Enable Instrumentation

```c
/* In optimization.h */
typedef struct s_bvh_stats
{
    int     traversals;          /* Total traverse_bvh calls */
    int     bbox_tests;          /* Total bbox_intersect calls */
    int     bbox_hits;           /* Bbox intersections */
    int     leaf_tests;          /* Leaf object tests */
    int     object_hits;         /* Actual intersections */
}   t_bvh_stats;

/* Add to t_bvh structure */
typedef struct s_bvh
{
    /* ... existing fields ... */
    t_bvh_stats stats;  /* NEW */
}   t_bvh;
```

### Print Statistics

```c
/* After rendering frame */
printf("BVH Stats:\n");
printf("  Traversals: %d\n", bvh->stats.traversals);
printf("  Bbox tests: %d (%.1f%% hit rate)\n",
       bvh->stats.bbox_tests,
       100.0 * bvh->stats.bbox_hits / bvh->stats.bbox_tests);
printf("  Object tests: %d (%.1f%% reduction vs linear)\n",
       bvh->stats.leaf_tests,
       100.0 * (1.0 - (double)bvh->stats.leaf_tests / 
                (bvh->stats.traversals * bvh->object_count)));
```

---

## Next Steps

After BVH integration complete:

1. ✅ **Validate**: Run visual regression tests on all scenes
2. ✅ **Benchmark**: Measure performance on 10, 20, 50, 100 object scenes
3. ✅ **Document**: Update README with performance results
4. → **Optional**: Implement intersection cache for shadow rays
5. → **Optional**: Add adaptive sampling for complex regions
6. → **Phase 2**: Generate detailed task breakdown with `/speckit.tasks`

---

## Quick Reference

### Compile Flags

| Flag | Purpose | Required |
|------|---------|----------|
| -DOPTIMIZATION_BVH | Enable BVH acceleration | Yes |
| -DOPTIMIZATION_CACHE | Enable intersection cache | Optional |

### Key Files

| File | Purpose |
|------|---------|
| `src/optimization/bvh.c` | BVH construction |
| `src/optimization/bvh_traverse.c` | BVH traversal |
| `src/optimization/bbox.c` | Bounding box operations |
| `includes/optimization.h` | Public API |

### Performance Targets (FR Requirements)

| Metric | Target | Measurement |
|--------|--------|-------------|
| Camera response | <100ms | Key press → pixel change |
| Object manipulation | <200ms | Position update → render |
| Complex scene FPS | 5+ FPS | 50 objects, continuous motion |
| Intersection reduction | 80% | Counter instrumentation |

---

## Support

- **Specification**: `specs/001-rendering-performance-optimization/spec.md`
- **Data Model**: `specs/001-rendering-performance-optimization/data-model.md`
- **API Contracts**: `specs/001-rendering-performance-optimization/contracts/*.md`
- **Research**: `specs/001-rendering-performance-optimization/research.md`

**Quick Start Complete**: Ready for implementation.
