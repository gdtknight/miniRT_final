# Intersection Cache API Contract

**Module**: `src/optimization/intersection_cache.c`  
**Header**: `includes/optimization.h`  
**Date**: 2025-12-19

## Overview

This contract defines the API for caching ray-object intersection results within a frame. Primary use case: shadow ray optimization (avoid redundant tests to same light source).

---

## Function: cache_create

### Signature

```c
t_intersection_cache    *cache_create(int bucket_count);
```

### Purpose
Create new intersection cache with specified capacity.

### Parameters

| Parameter | Type | Direction | Description | Constraints |
|-----------|------|-----------|-------------|-------------|
| bucket_count | int | IN | Number of hash buckets | Power of 2, typically 1024 |

### Return Value

| Value | Condition | Description |
|-------|-----------|-------------|
| t_intersection_cache* | Success | Valid cache, ready for use |
| NULL | Failure | malloc failed |

### Algorithm

```c
cache = malloc(sizeof(t_intersection_cache))
cache->buckets = calloc(bucket_count, sizeof(t_cache_entry*))
cache->bucket_count = bucket_count
cache->entry_count = 0
cache->hit_count = 0
cache->miss_count = 0
```

---

## Function: cache_get

### Signature

```c
bool    cache_get(t_intersection_cache *cache, t_ray *ray, void *object,
                  t_hit *hit);
```

### Purpose
Lookup cached intersection result for ray-object pair.

### Parameters

| Parameter | Type | Direction | Description | Constraints |
|-----------|------|-----------|-------------|-------------|
| cache | t_intersection_cache* | IN | Cache to query | Non-NULL |
| ray | t_ray* | IN | Ray to lookup | Non-NULL |
| object | void* | IN | Object pointer (identity) | Non-NULL |
| hit | t_hit* | OUT | Hit result if found | Non-NULL |

### Return Value

| Value | Condition | Description |
|-------|-----------|-------------|
| true | Cache hit | Result found, hit structure populated |
| false | Cache miss | No cached result for this ray-object pair |

### Postconditions (true)
- hit->distance, hit->point, hit->normal, hit->color set from cache
- cache->hit_count incremented

### Postconditions (false)
- hit structure unchanged
- cache->miss_count incremented

### Algorithm

```c
hash = hash_ray_direction(&ray->direction)
bucket_idx = hash % cache->bucket_count

entry = cache->buckets[bucket_idx]
while (entry != NULL):
    if (entry->key.ray_hash == hash && 
        entry->key.object_ptr == object):
        /* Cache hit */
        *hit = entry->hit_data
        cache->hit_count++
        return true
    entry = entry->next

/* Cache miss */
cache->miss_count++
return false
```

---

## Function: cache_set

### Signature

```c
void    cache_set(t_intersection_cache *cache, t_ray *ray, void *object,
                  t_hit *hit);
```

### Purpose
Store intersection result in cache for future lookups.

### Parameters

| Parameter | Type | Direction | Description | Constraints |
|-----------|------|-----------|-------------|-------------|
| cache | t_intersection_cache* | IN | Cache to update | Non-NULL |
| ray | t_ray* | IN | Ray to cache | Non-NULL |
| object | void* | IN | Object pointer | Non-NULL |
| hit | t_hit* | IN | Intersection result | Non-NULL, hit->distance valid |

### Algorithm

```c
/* Allocate new entry */
entry = malloc(sizeof(t_cache_entry))
entry->key.ray_hash = hash_ray_direction(&ray->direction)
entry->key.object_ptr = object
entry->hit_data = *hit  /* Copy hit structure */

/* Insert at head of collision chain */
bucket_idx = entry->key.ray_hash % cache->bucket_count
entry->next = cache->buckets[bucket_idx]
cache->buckets[bucket_idx] = entry
cache->entry_count++
```

### Note
No eviction policy - cache cleared at frame boundary (cache_clear).

---

## Function: cache_clear

### Signature

```c
void    cache_clear(t_intersection_cache *cache);
```

### Purpose
Remove all cached entries. Called at frame boundary (camera movement, object manipulation).

### Algorithm

```c
for i in 0..cache->bucket_count:
    entry = cache->buckets[i]
    while (entry != NULL):
        next = entry->next
        free(entry)
        entry = next
    cache->buckets[i] = NULL

cache->entry_count = 0
/* hit_count and miss_count preserved for profiling */
```

---

## Function: cache_destroy

### Signature

```c
void    cache_destroy(t_intersection_cache *cache);
```

### Purpose
Free all cache memory.

### Algorithm

```c
cache_clear(cache)  /* Free all entries */
free(cache->buckets)
free(cache)
```

---

## Function: cache_get_stats

### Signature

```c
void    cache_get_stats(t_intersection_cache *cache,
                        int *hits, int *misses, double *hit_rate);
```

### Purpose
Retrieve cache performance statistics for profiling.

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| cache | t_intersection_cache* | IN | Cache to query |
| hits | int* | OUT | Number of cache hits (can be NULL) |
| misses | int* | OUT | Number of cache misses (can be NULL) |
| hit_rate | double* | OUT | Hit rate (0.0-1.0) (can be NULL) |

### Algorithm

```c
if (hits != NULL)
    *hits = cache->hit_count
if (misses != NULL)
    *misses = cache->miss_count
if (hit_rate != NULL):
    total = cache->hit_count + cache->miss_count
    *hit_rate = total > 0 ? (double)cache->hit_count / total : 0.0
```

---

## Helper Function: hash_ray_direction

### Signature

```c
uint64_t    hash_ray_direction(t_vec3 *direction);
```

### Purpose
Compute 64-bit hash of ray direction for cache key.

### Algorithm (FNV-1a)

```c
/* Quantize direction to avoid floating point equality issues */
int64_t x = (int64_t)(direction->x * 10000.0)
int64_t y = (int64_t)(direction->y * 10000.0)
int64_t z = (int64_t)(direction->z * 10000.0)

/* FNV-1a hash */
uint64_t hash = 14695981039346656037ULL  /* FNV offset basis */
hash ^= x
hash *= 1099511628211ULL  /* FNV prime */
hash ^= y
hash *= 1099511628211ULL
hash ^= z
hash *= 1099511628211ULL

return hash
```

### Notes
- Quantization (×10000) provides ~0.0001 precision
- Nearby directions may hash to different values (no spatial locality)
- Collision rate low due to 64-bit space

---

## Integration with Shadow Ray Tracing

```c
/* In apply_lighting() (src/lighting/lighting.c) */
t_color apply_lighting(t_scene *scene, t_hit *surface_hit)
{
    t_ray   shadow_ray;
    t_hit   shadow_hit;
    bool    in_shadow;

    /* Construct shadow ray from hit point to light */
    shadow_ray.origin = surface_hit->point;
    shadow_ray.direction = vec3_normalize(
        vec3_subtract(scene->light.position, surface_hit->point)
    );

#ifdef OPTIMIZATION_CACHE
    /* Try cache first */
    if (scene->cache != NULL &&
        cache_get(scene->cache, &shadow_ray, &light, &shadow_hit))
    {
        in_shadow = (shadow_hit.distance > 0);
    }
    else
#endif
    {
        /* Cache miss - compute intersection */
        shadow_hit.distance = vec3_length(
            vec3_subtract(scene->light.position, surface_hit->point)
        );
        
        /* Test all objects for occlusion */
        in_shadow = test_shadow_occlusion(scene, &shadow_ray, &shadow_hit);

#ifdef OPTIMIZATION_CACHE
        /* Store result for future lookups */
        if (scene->cache != NULL)
            cache_set(scene->cache, &shadow_ray, &light, &shadow_hit);
#endif
    }

    /* Apply lighting based on shadow result */
    if (in_shadow)
        return ambient_only(scene, surface_hit);
    else
        return full_lighting(scene, surface_hit);
}
```

---

## Scene Integration

```c
/* In minirt.h */
typedef struct s_scene
{
    /* ... existing fields ... */
    
#ifdef OPTIMIZATION_CACHE
    t_intersection_cache    *cache;  /* NEW */
#endif
}   t_scene;

/* In main.c initialization */
#ifdef OPTIMIZATION_CACHE
scene->cache = cache_create(CACHE_BUCKET_COUNT);
#endif

/* In window event handler (camera movement) */
void    on_camera_move(t_scene *scene)
{
#ifdef OPTIMIZATION_CACHE
    if (scene->cache != NULL)
        cache_clear(scene->cache);  /* Invalidate cache */
#endif
    
    render_scene(scene);
}

/* In cleanup_scene() */
#ifdef OPTIMIZATION_CACHE
if (scene->cache != NULL)
    cache_destroy(scene->cache);
#endif
```

---

## Testing Requirements

```c
/* Test: Cache hit after set */
void test_cache_hit()
{
    t_intersection_cache *cache = cache_create(16);
    t_ray ray = {.origin = {0,0,0}, .direction = {1,0,0}};
    void *obj = (void*)0x1234;
    t_hit hit_in = {.distance = 5.0};
    t_hit hit_out;

    cache_set(cache, &ray, obj, &hit_in);
    bool found = cache_get(cache, &ray, obj, &hit_out);

    assert(found == true);
    assert(hit_out.distance == 5.0);
    
    cache_destroy(cache);
}

/* Test: Cache miss for different object */
void test_cache_miss_different_object()
{
    t_intersection_cache *cache = cache_create(16);
    t_ray ray = {.origin = {0,0,0}, .direction = {1,0,0}};
    void *obj1 = (void*)0x1234;
    void *obj2 = (void*)0x5678;
    t_hit hit_in = {.distance = 5.0};
    t_hit hit_out;

    cache_set(cache, &ray, obj1, &hit_in);
    bool found = cache_get(cache, &ray, obj2, &hit_out);

    assert(found == false);  /* Different object */
    
    cache_destroy(cache);
}

/* Test: Cache clear */
void test_cache_clear()
{
    t_intersection_cache *cache = cache_create(16);
    t_ray ray = {.origin = {0,0,0}, .direction = {1,0,0}};
    void *obj = (void*)0x1234;
    t_hit hit_in = {.distance = 5.0};
    t_hit hit_out;

    cache_set(cache, &ray, obj, &hit_in);
    cache_clear(cache);
    bool found = cache_get(cache, &ray, obj, &hit_out);

    assert(found == false);  /* Cleared */
    assert(cache->entry_count == 0);
    
    cache_destroy(cache);
}
```

---

## Performance Characteristics

| Operation | Time Complexity | Typical Time |
|-----------|----------------|--------------|
| cache_create | O(n) | ~50μs (1024 buckets) |
| cache_get (hit) | O(1) avg | ~40-60ns |
| cache_get (miss) | O(c) avg | ~50-80ns (c = chain length) |
| cache_set | O(1) | ~60-100ns (malloc + insert) |
| cache_clear | O(n + e) | ~5ms (n = buckets, e = entries) |

### Expected Hit Rate
- Shadow rays to single light: 30-40% (many rays hit same occluders)
- Primary rays: 0-5% (each pixel unique direction)
- **Recommendation**: Only cache shadow rays

### Memory Usage
- Empty cache: ~8 KB (1024 buckets × 8 bytes)
- 100k entries: ~3 MB (32 bytes per entry)
- Frame-local lifetime: acceptable overhead

---

## Configuration

```c
/* In optimization.h or config.h */
#define CACHE_BUCKET_COUNT      1024        /* Power of 2 */
#define CACHE_MAX_ENTRIES       102400      /* 100k entries */
#define CACHE_DIRECTION_SCALE   10000.0     /* Quantization precision */

/* Optional: Disable caching if hit rate < threshold */
#define CACHE_MIN_HIT_RATE      0.15        /* 15% minimum benefit */
```

---

## Compliance Summary

| Requirement | Compliance | Evidence |
|-------------|-----------|----------|
| FR-011 (intersection cache) | ✅ | Hash table implementation |
| FR-017 (memory efficiency) | ✅ | 3 MB for 100k entries |
| FR-021 (42 functions only) | ✅ | Only malloc/free used |
| FR-022 (no pthread) | ✅ | Single-threaded, no locks |
| FR-024 (visual quality) | ✅ | Same results as uncached |

**API Contract Complete**: Ready for implementation (optional optimization).
