# Performance Metrics API Contract

**Module**: Performance Metrics Collection  
**Purpose**: Real-time performance monitoring and display  
**Language**: C

## Core Functions

### Metrics Initialization

#### `metrics_init`
```c
/**
 * @brief Initializes performance metrics tracking structure
 * 
 * @return Initialized metrics structure with zeroed counters
 * 
 * @post All counters set to 0
 * @post Ring buffers zeroed
 * @post buffer_index = 0
 * @post display_enabled = false
 * 
 * @example
 * t_perf_metrics metrics = metrics_init();
 */
t_perf_metrics metrics_init(void);
```

#### `metrics_destroy`
```c
/**
 * @brief Cleans up metrics structure (currently no-op, for future extensions)
 * 
 * @param metrics Metrics structure to clean up
 * 
 * @note Currently no dynamic allocation, so this is a no-op
 * @note Included for API completeness and future-proofing
 * 
 * @example
 * metrics_destroy(&metrics);
 */
void metrics_destroy(t_perf_metrics *metrics);
```

---

### Frame Timing

#### `metrics_frame_start`
```c
/**
 * @brief Marks the start of a new frame, resets per-frame counters
 * 
 * @param metrics Metrics structure to update
 * 
 * @pre metrics != NULL
 * @post frame_start_time = current time (microseconds)
 * @post ray_count = 0
 * @post intersection_count = 0
 * @post bvh_node_visits = 0
 * @post frame_count++
 * 
 * @note Uses clock_gettime(CLOCK_MONOTONIC) for high-resolution timing
 * 
 * @example
 * metrics_frame_start(&metrics);
 * // Render frame...
 * metrics_frame_end(&metrics);
 */
void metrics_frame_start(t_perf_metrics *metrics);
```

#### `metrics_frame_end`
```c
/**
 * @brief Marks the end of a frame, computes and stores frame time
 * 
 * @param metrics Metrics structure to update
 * 
 * @pre metrics != NULL
 * @pre metrics_frame_start was called this frame
 * 
 * @post frame_times[buffer_index] = current_time - frame_start_time
 * @post buffer_index = (buffer_index + 1) % 60
 * @post Frame time stored in microseconds
 * 
 * @example
 * metrics_frame_start(&metrics);
 * render_scene(scene, camera);
 * metrics_frame_end(&metrics);
 */
void metrics_frame_end(t_perf_metrics *metrics);
```

---

### BVH Traversal Timing

#### `metrics_bvh_start`
```c
/**
 * @brief Marks the start of BVH traversal timing
 * 
 * @param metrics Metrics structure to update
 * 
 * @pre metrics != NULL
 * @post bvh_traverse_start = current time (microseconds)
 * 
 * @example
 * metrics_bvh_start(&metrics);
 * bvh_intersect(bvh_root, &ray, &hit, &metrics);
 * metrics_bvh_end(&metrics);
 */
void metrics_bvh_start(t_perf_metrics *metrics);
```

#### `metrics_bvh_end`
```c
/**
 * @brief Marks the end of BVH traversal, computes and stores duration
 * 
 * @param metrics Metrics structure to update
 * 
 * @pre metrics != NULL
 * @pre metrics_bvh_start was called
 * 
 * @post bvh_traverse_times[buffer_index] = current_time - bvh_traverse_start
 * @post Duration stored in microseconds
 * 
 * @example
 * metrics_bvh_start(&metrics);
 * // ... BVH traversal ...
 * metrics_bvh_end(&metrics);
 */
void metrics_bvh_end(t_perf_metrics *metrics);
```

---

### Counter Updates

#### `metrics_add_ray`
```c
/**
 * @brief Increments ray counter for current frame
 * 
 * @param metrics Metrics structure to update
 * 
 * @pre metrics != NULL
 * @post metrics->ray_count++
 * 
 * @note Called once per ray cast (primary, shadow, reflection)
 * 
 * @example
 * metrics_add_ray(&metrics);
 * bvh_intersect(bvh_root, &ray, &hit, &metrics);
 */
void metrics_add_ray(t_perf_metrics *metrics);
```

#### `metrics_add_intersection`
```c
/**
 * @brief Increments intersection test counter for current frame
 * 
 * @param metrics Metrics structure to update
 * @param count Number of intersection tests performed
 * 
 * @pre metrics != NULL
 * @pre count > 0
 * @post metrics->intersection_count += count
 * 
 * @note Called by intersection test functions
 * @note Can add multiple tests at once (e.g., leaf with 4 objects)
 * 
 * @example
 * metrics_add_intersection(&metrics, leaf->object_count);
 */
void metrics_add_intersection(t_perf_metrics *metrics, int count);
```

#### `metrics_add_bvh_visit`
```c
/**
 * @brief Increments BVH node visit counter for current frame
 * 
 * @param metrics Metrics structure to update
 * 
 * @pre metrics != NULL
 * @post metrics->bvh_node_visits++
 * 
 * @note Called once per BVH node visited during traversal
 * 
 * @example
 * // In BVH traversal loop
 * metrics_add_bvh_visit(&metrics);
 * if (aabb_intersect_ray(node->bbox, ray, t_min, t_max))
 *     // ... visit children ...
 */
void metrics_add_bvh_visit(t_perf_metrics *metrics);
```

---

### Display Control

#### `metrics_toggle_display`
```c
/**
 * @brief Toggles metrics HUD display on/off
 * 
 * @param metrics Metrics structure to update
 * 
 * @pre metrics != NULL
 * @post metrics->display_enabled = !metrics->display_enabled
 * 
 * @note Typically called in response to 'I' key press
 * 
 * @example
 * // In key handler
 * if (key == 'I')
 *     metrics_toggle_display(&metrics);
 */
void metrics_toggle_display(t_perf_metrics *metrics);
```

#### `metrics_is_display_enabled`
```c
/**
 * @brief Checks if metrics display is currently enabled
 * 
 * @param metrics Metrics structure to query
 * @return true if display enabled, false otherwise
 * 
 * @pre metrics != NULL
 * 
 * @example
 * if (metrics_is_display_enabled(&metrics))
 *     metrics_render_hud(&metrics, image);
 */
bool metrics_is_display_enabled(t_perf_metrics *metrics);
```

---

### Computed Metrics

#### `metrics_get_fps_current`
```c
/**
 * @brief Computes current FPS from most recent frame time
 * 
 * @param metrics Metrics structure to query
 * @return Current FPS (frames per second)
 * 
 * @pre metrics != NULL
 * @pre frame_count > 0
 * @post result >= 0
 * 
 * @note Formula: 1,000,000 / frame_times[buffer_index]
 * @note Returns 0 if frame time is 0 (avoid division by zero)
 * 
 * @example
 * float fps = metrics_get_fps_current(&metrics);
 * printf("FPS: %.1f\n", fps);
 */
float metrics_get_fps_current(t_perf_metrics *metrics);
```

#### `metrics_get_fps_average`
```c
/**
 * @brief Computes average FPS from ring buffer (last 60 frames)
 * 
 * @param metrics Metrics structure to query
 * @return Average FPS over last 60 frames
 * 
 * @pre metrics != NULL
 * @post result >= 0
 * 
 * @note Smooths out frame-to-frame variance
 * @note More stable than current FPS for display
 * 
 * @example
 * float avg_fps = metrics_get_fps_average(&metrics);
 * printf("Avg FPS: %.1f\n", avg_fps);
 */
float metrics_get_fps_average(t_perf_metrics *metrics);
```

#### `metrics_get_frame_ms_current`
```c
/**
 * @brief Gets most recent frame time in milliseconds
 * 
 * @param metrics Metrics structure to query
 * @return Current frame time in milliseconds
 * 
 * @pre metrics != NULL
 * @post result >= 0
 * 
 * @note Converts from microseconds to milliseconds
 * 
 * @example
 * float frame_ms = metrics_get_frame_ms_current(&metrics);
 * printf("Frame: %.2fms\n", frame_ms);
 */
float metrics_get_frame_ms_current(t_perf_metrics *metrics);
```

#### `metrics_get_frame_ms_average`
```c
/**
 * @brief Gets average frame time in milliseconds (last 60 frames)
 * 
 * @param metrics Metrics structure to query
 * @return Average frame time in milliseconds
 * 
 * @pre metrics != NULL
 * @post result >= 0
 * 
 * @example
 * float avg_ms = metrics_get_frame_ms_average(&metrics);
 * printf("Avg Frame: %.2fms\n", avg_ms);
 */
float metrics_get_frame_ms_average(t_perf_metrics *metrics);
```

#### `metrics_get_bvh_percent`
```c
/**
 * @brief Computes percentage of frame time spent in BVH traversal
 * 
 * @param metrics Metrics structure to query
 * @return BVH traversal time as percentage of total frame time
 * 
 * @pre metrics != NULL
 * @post result >= 0 && result <= 100
 * 
 * @note Returns 0 if frame time is 0 (avoid division by zero)
 * 
 * @example
 * float bvh_pct = metrics_get_bvh_percent(&metrics);
 * printf("BVH: %.1f%%\n", bvh_pct);
 */
float metrics_get_bvh_percent(t_perf_metrics *metrics);
```

---

### HUD Rendering

#### `metrics_render_hud`
```c
/**
 * @brief Renders performance metrics HUD overlay on image
 * 
 * @param metrics Metrics structure to display
 * @param image MLX image to render on
 * @param mlx MLX instance for text rendering
 * 
 * @pre metrics != NULL
 * @pre image != NULL
 * @pre mlx != NULL
 * @pre metrics->display_enabled == true
 * 
 * @post Metrics text drawn at top-left corner of image
 * @post Background semi-transparent box drawn behind text
 * 
 * @note Display format:
 *   FPS: 4.2 (avg: 3.8)
 *   Frame: 237ms (avg: 263ms)
 *   Rays: 1,228,800
 *   BVH Traverse: 89ms (37%)
 *   Intersections: 3,421 (99.7% reduction)
 *   BVH Node Visits: 1,247
 * 
 * @note Uses mlx_string_put for text rendering (white text)
 * @note Renders background box with 50% opacity for readability
 * 
 * @example
 * if (metrics_is_display_enabled(&metrics))
 *     metrics_render_hud(&metrics, scene->image, scene->mlx);
 */
void metrics_render_hud(
    t_perf_metrics *metrics,
    void *image,
    void *mlx
);
```

---

## Data Types

### Performance Metrics Structure
```c
typedef struct s_perf_metrics
{
    uint64_t    frame_times[60];        // Ring buffer: frame durations (μs)
    uint64_t    bvh_traverse_times[60]; // Ring buffer: BVH times (μs)
    int         buffer_index;           // Current position in ring buffer [0-59]
    uint64_t    frame_start_time;       // Timestamp at frame start (μs)
    uint64_t    bvh_traverse_start;     // Timestamp at BVH start (μs)
    int         ray_count;              // Rays cast this frame
    int         intersection_count;     // Intersection tests this frame
    int         bvh_node_visits;        // BVH nodes visited this frame
    int         frame_count;            // Total frames since start
    bool        display_enabled;        // HUD display toggle
} t_perf_metrics;
```

---

## Error Handling

### Null Pointer Checks
All functions that take `t_perf_metrics *metrics` parameter:
- Check for NULL and return early (no-op) if NULL
- No error messages printed (graceful degradation)

### Division by Zero
Functions that compute FPS or percentages:
- Return 0.0 if denominator is 0
- No error messages (handled gracefully)

---

## Performance Overhead

### Measurement Overhead
- `clock_gettime`: ~20-50 nanoseconds per call
- Counter increments: ~1 nanosecond per increment
- Ring buffer write: ~5 nanoseconds per write

**Total overhead per frame**:
- 2 × clock_gettime (frame start/end): ~100 ns
- 2 × clock_gettime (BVH start/end): ~100 ns
- N × counter increments (N = rays cast): ~N ns
- Total: ~200 ns + N ns ≈ 0.0002% overhead for N=1000 rays

**Negligible impact**: <0.01% of frame time for typical scenes

---

## Usage Examples

### Complete Metrics Workflow
```c
// 1. Initialize metrics at program start
t_perf_metrics metrics = metrics_init();

// 2. Main render loop
while (running)
{
    metrics_frame_start(&metrics);
    
    // Render pixels
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            t_ray ray = create_camera_ray(camera, x, y);
            metrics_add_ray(&metrics);
            
            metrics_bvh_start(&metrics);
            t_hit_record hit;
            bool hit_found = bvh_intersect(bvh_root, &ray, &hit, &metrics);
            metrics_bvh_end(&metrics);
            
            if (hit_found)
            {
                t_color color = shade_hit_point(&hit, scene, &metrics);
                put_pixel(image, x, y, color);
            }
        }
    }
    
    metrics_frame_end(&metrics);
    
    // Display HUD if enabled
    if (metrics_is_display_enabled(&metrics))
        metrics_render_hud(&metrics, image, mlx);
    
    mlx_put_image_to_window(mlx, win, image, 0, 0);
}

// 3. Cleanup
metrics_destroy(&metrics);
```

### Key Handler Integration
```c
int key_hook(int keycode, t_scene *scene)
{
    if (keycode == KEY_I)  // 'I' key
    {
        metrics_toggle_display(&scene->metrics);
        return 0;
    }
    // ... other key handlers ...
}
```

### Debug Output
```c
// Print metrics to console every 60 frames
if (metrics.frame_count % 60 == 0)
{
    printf("=== Performance Metrics ===\n");
    printf("FPS: %.1f (avg: %.1f)\n",
           metrics_get_fps_current(&metrics),
           metrics_get_fps_average(&metrics));
    printf("Frame: %.2fms (avg: %.2fms)\n",
           metrics_get_frame_ms_current(&metrics),
           metrics_get_frame_ms_average(&metrics));
    printf("Rays: %d\n", metrics.ray_count);
    printf("Intersections: %d\n", metrics.intersection_count);
    printf("BVH Nodes: %d\n", metrics.bvh_node_visits);
    printf("BVH Time: %.1f%%\n", metrics_get_bvh_percent(&metrics));
}
```

---

## Dependencies

### Required Headers
```c
#include "miniRT.h"      // Core types
#include <time.h>        // clock_gettime
#include <stdint.h>      // uint64_t
#include <stdbool.h>     // bool
#include <stdio.h>       // printf (debug output)
```

### Required Functions (42 School Compliant)
- ✅ `clock_gettime(CLOCK_MONOTONIC, ...)` - High-resolution timing
- ✅ `mlx_string_put` - Text rendering for HUD
- ✅ Standard math operations (addition, division)

### Prohibited Functions
- ❌ No threading/parallelization
- ❌ No external profiling libraries

---

## Thread Safety

**Not thread-safe**: All functions assume single-threaded access. This is compliant with 42 School constraints (no pthread).

---

## Version & Compatibility

**API Version**: 1.0  
**Compatible with**: miniRT project (42 School)  
**Language Standard**: C99  
**Platform**: Linux (CLOCK_MONOTONIC required), macOS
