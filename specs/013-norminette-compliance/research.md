# Norminette Compliance Refactoring Research

**Feature**: Complete Norminette Compliance  
**Branch**: `013-norminette-compliance`  
**Date**: 2026-01-03  
**Phase**: 0 (Research & Planning)

## Research Summary

This document consolidates research findings for eliminating all 77 norminette errors through systematic refactoring. Research focused on five key areas: file splitting strategies, function extraction techniques, parameter structure design, validation methodology, and build system updates.

---

## 1. File Splitting Strategy

### Decision
**Split by feature concern with maximum 5 functions per file, maintaining logical cohesion**

### Rationale
- **Semantic Grouping**: Functions that operate on related data or implement related functionality should stay together (e.g., all FPS calculation functions, all text rendering functions)
- **Clear Ownership**: Each file has a clear purpose stated in its name (e.g., `metrics_tracking.c` for metrics collection, `metrics_display.c` for metrics rendering)
- **Minimal Dependencies**: Splitting by feature concern naturally minimizes cross-file dependencies compared to arbitrary splits
- **42 School Compliance**: Directly addresses TOO_MANY_FUNCS errors (53 out of 77 total errors)
- **Maintainability**: Future developers can locate functionality intuitively by feature domain

### Alternatives Considered
1. **Alphabetical Splitting**: Split files by function name alphabetically
   - **Rejected**: Destroys semantic relationships, creates artificial coupling, makes code navigation difficult
2. **Size-Based Splitting**: Split when file reaches certain line count
   - **Rejected**: Line count doesn't correlate with logical cohesion, can split mid-feature
3. **One Function Per File**: Extreme splitting to minimize violations
   - **Rejected**: Excessive file count (would create 100+ files), impractical navigation, violates principle of cohesion

### Implementation Pattern

**For metrics.c (8 functions → 3 files)**:
```
metrics.c (8 functions) SPLIT INTO:
├── metrics_tracking.c (5 functions)
│   ├── init_metrics()
│   ├── update_metrics()
│   ├── reset_metrics()
│   ├── calculate_average()
│   └── store_sample()
├── metrics_display.c (3 functions)
│   ├── render_metrics_overlay()
│   ├── format_metrics_text()
│   └── draw_metrics_bar()
└── metrics_fps.c (2 functions) - merged with tracking if needed
    ├── calculate_fps()
    └── smooth_fps()
```

**For window.c (7 functions → 3 files)**:
```
window.c (7 functions) SPLIT INTO:
├── window_init.c (5 functions)
│   ├── create_window()
│   ├── init_mlx_connection()
│   ├── setup_window_properties()
│   ├── allocate_image_buffer()
│   └── configure_color_settings()
├── window_events.c (4 functions)
│   ├── handle_key_press()
│   ├── handle_key_release()
│   ├── handle_mouse_event()
│   └── handle_window_close()
└── window_hooks.c (2 functions)
    ├── register_mlx_hooks()
    └── setup_event_handlers()
```

### Header Organization
**One header per source file** pattern:
- `src/core/metrics_tracking.c` → `includes/metrics_tracking.h`
- `src/ui/window_init.c` → `includes/window_init.h`
- Each header contains only declarations for its corresponding source file
- Minimizes include complexity, clear 1:1 mapping

---

## 2. Function Extraction Techniques

### Decision
**Extract calculation blocks, conditional branches, and initialization sequences into named helper functions with clear purpose**

### Rationale
- **Readability**: Long functions become high-level orchestration with clear steps
- **Testability**: Extracted helpers can be unit tested independently
- **Performance**: Modern compilers inline simple helpers; use `static inline` for hot paths
- **Compliance**: Directly addresses TOO_MANY_LINES errors (5 violations)
- **Debugging**: Smaller functions easier to step through and understand

### Alternatives Considered
1. **Macro-Based Approaches**: Use macros to reduce line count
   - **Rejected**: Debugging difficulty, type safety issues, norminette may still count expanded lines
2. **Aggressive Inlining**: Force inline everything to avoid function call overhead
   - **Rejected**: Compiler handles this automatically, premature optimization
3. **Code Duplication**: Duplicate code to avoid helper functions
   - **Rejected**: Violates DRY principle, maintenance nightmare

### Extraction Patterns

**Pattern 1: Calculation Block Extraction**
```c
// BEFORE: 35-line function with complex calculation
void render_pixel(t_scene *scene, int x, int y)
{
    t_ray ray;
    t_color color;
    double u = (double)x / scene->width;
    double v = (double)y / scene->height;
    
    // 10 lines of ray generation calculation
    ray.origin = scene->camera.position;
    ray.direction.x = /* complex calculation */;
    ray.direction.y = /* complex calculation */;
    ray.direction.z = /* complex calculation */;
    normalize_vector(&ray.direction);
    
    // 15 lines of color calculation
    if (trace_ray(&ray, scene, &color))
    {
        // lighting calculations...
        // shadow calculations...
        // reflection calculations...
    }
    
    set_pixel(scene, x, y, color);
}

// AFTER: 20-line orchestration function + 2 helpers
void render_pixel(t_scene *scene, int x, int y)
{
    t_ray ray;
    t_color color;
    
    generate_camera_ray(scene, x, y, &ray);
    calculate_pixel_color(&ray, scene, &color);
    set_pixel(scene, x, y, color);
}

static void generate_camera_ray(t_scene *scene, int x, int y, t_ray *ray)
{
    double u = (double)x / scene->width;
    double v = (double)y / scene->height;
    
    ray->origin = scene->camera.position;
    ray->direction.x = /* calculation */;
    ray->direction.y = /* calculation */;
    ray->direction.z = /* calculation */;
    normalize_vector(&ray->direction);
}

static void calculate_pixel_color(t_ray *ray, t_scene *scene, t_color *color)
{
    if (trace_ray(ray, scene, color))
    {
        apply_lighting(color, scene);
        apply_shadows(color, scene);
        apply_reflections(color, scene);
    }
}
```

**Pattern 2: Conditional Branch Extraction**
```c
// BEFORE: 30-line function with nested conditionals
void handle_event(t_event *event)
{
    if (event->type == KEY_PRESS)
    {
        if (event->key == 'w') /* 5 lines of handling */
        else if (event->key == 's') /* 5 lines of handling */
        else if (event->key == 'a') /* 5 lines of handling */
        else if (event->key == 'd') /* 5 lines of handling */
    }
    else if (event->type == MOUSE_MOVE)
    {
        /* 8 lines of mouse handling */
    }
}

// AFTER: 15-line dispatch function + specialized handlers
void handle_event(t_event *event)
{
    if (event->type == KEY_PRESS)
        handle_key_press(event->key);
    else if (event->type == MOUSE_MOVE)
        handle_mouse_move(event);
}

static void handle_key_press(int key)
{
    if (key == 'w')
        move_camera_forward();
    else if (key == 's')
        move_camera_backward();
    else if (key == 'a')
        move_camera_left();
    else if (key == 'd')
        move_camera_right();
}

static void handle_mouse_move(t_event *event)
{
    update_camera_rotation(event->mouse_x, event->mouse_y);
}
```

**Pattern 3: Initialization Sequence Extraction**
```c
// BEFORE: 40-line initialization function
void init_scene(t_scene *scene)
{
    // 10 lines of camera initialization
    scene->camera.position = (t_vector){0, 0, -10};
    scene->camera.direction = (t_vector){0, 0, 1};
    scene->camera.fov = 60.0;
    // ... more camera setup
    
    // 10 lines of lighting initialization
    scene->ambient_light.color = (t_color){255, 255, 255};
    scene->ambient_light.intensity = 0.2;
    // ... more lighting setup
    
    // 10 lines of object initialization
    scene->objects = malloc(sizeof(t_object) * MAX_OBJECTS);
    scene->object_count = 0;
    // ... more object setup
    
    // 10 lines of rendering state initialization
    scene->samples_per_pixel = 1;
    scene->max_depth = 5;
    // ... more state setup
}

// AFTER: 20-line orchestration + 4 specialized initializers
void init_scene(t_scene *scene)
{
    init_camera(&scene->camera);
    init_lighting(scene);
    init_objects(scene);
    init_render_state(scene);
}

static void init_camera(t_camera *camera)
{
    camera->position = (t_vector){0, 0, -10};
    camera->direction = (t_vector){0, 0, 1};
    camera->fov = 60.0;
    calculate_camera_basis(camera);
}

static void init_lighting(t_scene *scene)
{
    scene->ambient_light.color = (t_color){255, 255, 255};
    scene->ambient_light.intensity = 0.2;
    scene->light_count = 0;
}

// ... other initializers
```

### Performance Considerations
- **Static Functions**: All helpers should be `static` to enable compiler optimization
- **Inline Hints**: For functions called in tight loops (e.g., ray tracing inner loop), consider `static inline` or `__attribute__((always_inline))`
- **Struct Passing**: Pass large structures by pointer, not by value
- **Measurement**: Profile before and after to verify no significant regression

---

## 3. Parameter Structure Design

### Decision
**Group related parameters by domain (rendering params, state params, config params) into structured types**

### Rationale
- **Signature Simplicity**: Reduces visual complexity of function signatures
- **Extensibility**: Easy to add new parameters without changing signatures
- **Semantic Clarity**: Parameter groups communicate relationships (e.g., all rendering config together)
- **Compliance**: Directly addresses TOO_MANY_ARGS errors (6 violations)
- **Type Safety**: Structures are strongly typed, unlike variadic approaches

### Alternatives Considered
1. **Global State**: Use global variables instead of passing parameters
   - **Rejected**: Makes testing impossible, creates hidden dependencies, violates 42 best practices
2. **Variadic Functions**: Use `va_list` for variable parameters
   - **Rejected**: Type-unsafe, difficult to debug, norminette may prohibit
3. **Keep All Parameters**: Accept >4 parameters, use mlx exception pattern
   - **Rejected**: Only acceptable for external library functions (mlx), not custom code

### Design Patterns

**Pattern 1: Rendering Configuration Structure**
```c
// BEFORE: 6 parameters
void render_scene(t_scene *scene, int width, int height, 
                  int samples, int max_depth, double gamma)
{
    // rendering logic
}

// AFTER: 2 parameters (scene + config)
typedef struct s_render_config
{
    int     width;
    int     height;
    int     samples_per_pixel;
    int     max_ray_depth;
    double  gamma_correction;
}   t_render_config;

void render_scene(t_scene *scene, t_render_config *config)
{
    // rendering logic using config->width, config->height, etc.
}
```

**Pattern 2: State Update Structure**
```c
// BEFORE: 5 parameters
void update_state(t_state *state, double delta_time, int key_pressed,
                  int mouse_x, int mouse_y)
{
    // state update logic
}

// AFTER: 2 parameters (state + update_info)
typedef struct s_state_update
{
    double  delta_time;
    int     key_pressed;
    int     mouse_x;
    int     mouse_y;
}   t_state_update;

void update_state(t_state *state, t_state_update *update)
{
    // state update logic
}
```

**Pattern 3: BVH Traversal Context**
```c
// BEFORE: TOO_MANY_ARGS in bvh_traverse.c line 58
void traverse_bvh(t_bvh_node *node, t_ray *ray, double t_min, 
                  double t_max, t_hit_record *rec, int depth)
{
    // traversal logic
}

// AFTER: Using traversal context
typedef struct s_bvh_traversal
{
    t_ray           *ray;
    double          t_min;
    double          t_max;
    t_hit_record    *rec;
    int             depth;
}   t_bvh_traversal;

void traverse_bvh(t_bvh_node *node, t_bvh_traversal *ctx)
{
    // traversal logic using ctx->ray, ctx->t_min, etc.
}
```

### Memory and Performance Considerations
- **Stack Allocation**: Structures should be allocated on stack, not heap
  ```c
  t_render_config config = {800, 600, 1, 5, 2.2};
  render_scene(scene, &config);
  ```
- **Structure Size**: Keep structures small (< 64 bytes) to fit in cache lines
- **Padding**: Compiler may add padding; consider alignment for performance-critical structures
- **Measurement**: Verify no performance regression through benchmarking

---

## 4. Validation Strategy

### Decision
**Multi-layered validation: Automated image diff + exit code verification + manual smoke testing + performance benchmarking**

### Rationale
- **Comprehensive Coverage**: Multiple validation layers catch different types of regressions
- **Fast Feedback**: Automated checks provide immediate feedback during development
- **Objective Metrics**: Image diff and performance measurements are quantitative
- **Confidence**: Combination of automated and manual checks provides high confidence in refactoring correctness

### Alternatives Considered
1. **Manual Testing Only**: Rely on human verification
   - **Rejected**: Slow, error-prone, doesn't catch subtle pixel differences
2. **Unit Tests Only**: Write extensive unit tests
   - **Rejected**: Time-consuming to write, may not catch integration issues
3. **No Validation**: Trust the refactoring is correct
   - **Rejected**: Unacceptable risk for this project scale

### Validation Workflow

**Step 1: Establish Baseline (Before Refactoring)**
```bash
# 1. Norminette baseline
norminette src/ includes/ 2>&1 | tee norminette_baseline.txt
grep "Error:" norminette_baseline.txt | wc -l  # Should output: 77

# 2. Build baseline
make clean && make
echo $?  # Should be 0

# 3. Functional baseline - render all test scenes
mkdir -p baseline_renders/
for scene in scenes/*.rt; do
    scene_name=$(basename "$scene" .rt)
    ./miniRT "$scene" > "baseline_renders/${scene_name}.ppm"
    echo "Rendered: $scene_name, Exit code: $?"
done

# 4. Performance baseline
time ./miniRT scenes/complex_scene.rt > /dev/null
# Record the "real" time value
```

**Step 2: Incremental Validation (After Each File Split)**
```bash
# 1. Build check
make
if [ $? -ne 0 ]; then
    echo "BUILD FAILED - Fix before proceeding"
    exit 1
fi

# 2. Norminette check on modified files
norminette src/core/metrics*.c includes/metrics*.h
# Verify: No errors in newly split files

# 3. Quick smoke test
./miniRT scenes/simple.rt > test_render.ppm
if [ $? -ne 0 ]; then
    echo "EXECUTION FAILED - Fix before proceeding"
    exit 1
fi

# 4. Visual verification (manual)
# Open test_render.ppm and baseline_renders/simple.ppm
# Verify they look identical
```

**Step 3: Batch Validation (After Multiple Changes)**
```bash
# 1. Full norminette check
norminette src/ includes/ 2>&1 | tee norminette_current.txt
current_errors=$(grep "Error:" norminette_current.txt | wc -l)
echo "Errors remaining: $current_errors"

# 2. Full build test
make clean && make && make bonus

# 3. Full scene testing with image diff
for scene in scenes/*.rt; do
    scene_name=$(basename "$scene" .rt)
    ./miniRT "$scene" > "current_renders/${scene_name}.ppm"
    
    # Compare with baseline
    diff "baseline_renders/${scene_name}.ppm" "current_renders/${scene_name}.ppm"
    if [ $? -eq 0 ]; then
        echo "✓ $scene_name: IDENTICAL"
    else
        echo "✗ $scene_name: DIFFERS - Manual verification needed"
    fi
done

# 4. Performance comparison
time ./miniRT scenes/complex_scene.rt > /dev/null
# Compare with baseline time - should be within 5%
```

**Step 4: Final Validation (After All Refactoring)**
```bash
# 1. Zero errors verification
norminette src/ includes/ | grep "Error:"
# Should output nothing (0 errors)

# 2. Comprehensive build
make clean
make
make bonus
# All should succeed

# 3. All test scenes
./scripts/run_all_test_scenes.sh
# All should pass

# 4. Performance benchmark
./scripts/benchmark.sh scenes/complex_scene.rt
# Compare with baseline, verify ≤5% regression

# 5. Memory leak check
valgrind --leak-check=full --error-exitcode=1 ./miniRT scenes/simple.rt
# Should exit with code 0 (no leaks)
```

### Automated Image Comparison

**Simple PPM Diff Approach**:
```bash
#!/bin/bash
# compare_ppm.sh - Simple binary comparison for PPM files

if cmp -s "$1" "$2"; then
    echo "Images are identical"
    exit 0
else
    echo "Images differ"
    # Show first difference
    cmp -l "$1" "$2" | head -n 1
    exit 1
fi
```

**Visual Diff Tool** (if ImageMagick available):
```bash
# Generate visual diff highlighting differences
compare baseline.ppm current.ppm diff.ppm
# If diff.ppm is all black, images are identical
```

### Performance Measurement

**Benchmark Script**:
```bash
#!/bin/bash
# benchmark.sh - Measure rendering performance

scene="$1"
iterations=5

echo "Benchmarking: $scene"
total_time=0

for i in $(seq 1 $iterations); do
    start=$(date +%s.%N)
    ./miniRT "$scene" > /dev/null
    end=$(date +%s.%N)
    
    time=$(echo "$end - $start" | bc)
    total_time=$(echo "$total_time + $time" | bc)
    echo "  Iteration $i: ${time}s"
done

avg_time=$(echo "scale=3; $total_time / $iterations" | bc)
echo "Average time: ${avg_time}s"
```

**Performance Comparison**:
```bash
# Compare baseline vs. current
baseline_time=5.234  # from baseline benchmark
current_time=5.412   # from current benchmark

regression=$(echo "scale=2; ($current_time - $baseline_time) / $baseline_time * 100" | bc)
echo "Performance change: ${regression}%"

if (( $(echo "$regression > 5.0" | bc -l) )); then
    echo "WARNING: Performance regression exceeds 5% threshold"
    exit 1
fi
```

### Exit Code Verification

**All scenes should exit with code 0**:
```bash
for scene in scenes/*.rt; do
    ./miniRT "$scene" > /dev/null
    exit_code=$?
    if [ $exit_code -ne 0 ]; then
        echo "ERROR: $scene exited with code $exit_code"
    fi
done
```

---

## 5. Makefile Update Strategy

### Decision
**Explicit source file listing organized by directory with comments for each feature area**

### Rationale
- **Clear Visibility**: All source files explicitly listed, easy to see what's included
- **Supports Partial Builds**: Make's dependency tracking works well with explicit lists
- **Avoids Accidental Inclusion**: Wildcard patterns (`*.c`) might include test files or backups
- **Readable**: Organized by directory with comments makes structure clear
- **42 School Standard**: Matches typical 42 project Makefile patterns

### Alternatives Considered
1. **Wildcard Source Discovery**: `SRCS = $(wildcard src/**/*.c)`
   - **Rejected**: May include unwanted files, less explicit control
2. **Single Long List**: All sources in one unsorted list
   - **Rejected**: Difficult to maintain, hard to find specific files
3. **Per-Directory Variables**: Separate variables per directory, then combine
   - **Accepted as enhancement**: Can use this pattern for better organization

### Makefile Organization Pattern

```makefile
# ============================================
# miniRT - Ray Tracing Project Makefile
# ============================================

NAME = miniRT

# Compiler and Flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -O3 -I includes -I lib/mlx
LDFLAGS = -L lib/mlx -lmlx -framework OpenGL -framework AppKit -lm

# Directories
SRC_DIR = src
OBJ_DIR = build
INC_DIR = includes

# ============================================
# Source Files (organized by feature)
# ============================================

# Core - Main entry, ray tracing, rendering
SRCS_CORE = \
	$(SRC_DIR)/core/main.c \
	$(SRC_DIR)/core/trace.c \
	$(SRC_DIR)/core/metrics_tracking.c \
	$(SRC_DIR)/core/metrics_display.c \
	$(SRC_DIR)/core/metrics_fps.c \
	$(SRC_DIR)/core/metrics_calc.c \
	$(SRC_DIR)/core/render_init.c \
	$(SRC_DIR)/core/render_loop.c \
	$(SRC_DIR)/core/render_state.c \
	$(SRC_DIR)/core/render_state_update.c \
	$(SRC_DIR)/core/timer_control.c \
	$(SRC_DIR)/core/timer_metrics.c

# Geometry - Camera, vectors
SRCS_GEOMETRY = \
	$(SRC_DIR)/geometry/camera.c \
	$(SRC_DIR)/geometry/vector.c \
	$(SRC_DIR)/geometry/vector_ops.c

# UI - Window management, HUD, key guide
SRCS_UI = \
	$(SRC_DIR)/ui/window_init.c \
	$(SRC_DIR)/ui/window_events.c \
	$(SRC_DIR)/ui/window_hooks.c \
	$(SRC_DIR)/ui/hud/hud_render.c \
	$(SRC_DIR)/ui/hud/hud_text.c \
	$(SRC_DIR)/ui/hud/hud_metrics.c \
	$(SRC_DIR)/ui/hud/hud_controls.c \
	$(SRC_DIR)/ui/keyguide_cleanup.c \
	$(SRC_DIR)/ui/keyguide_init.c \
	$(SRC_DIR)/ui/keyguide_render.c

# Utils - Parser, error handling, utilities
SRCS_UTILS = \
	$(SRC_DIR)/utils/parser.c \
	$(SRC_DIR)/utils/parse_elements.c \
	$(SRC_DIR)/utils/parse_objects.c \
	$(SRC_DIR)/utils/parse_validation.c \
	$(SRC_DIR)/utils/error.c \
	$(SRC_DIR)/utils/cleanup.c \
	$(SRC_DIR)/utils/ft_atof.c \
	$(SRC_DIR)/utils/ft_atoi.c

# Lighting - Shadows, lighting calculations
SRCS_LIGHTING = \
	$(SRC_DIR)/lighting/lighting.c \
	$(SRC_DIR)/lighting/shadow_test.c \
	$(SRC_DIR)/lighting/shadow_config.c \
	$(SRC_DIR)/lighting/shadow_calc.c \
	$(SRC_DIR)/lighting/shadow_attenuation.c

# BVH - Acceleration structure
SRCS_BVH = \
	$(SRC_DIR)/bvh/bvh_init.c \
	$(SRC_DIR)/bvh/bounds.c \
	$(SRC_DIR)/bvh/aabb_basic.c \
	$(SRC_DIR)/bvh/aabb_advanced.c \
	$(SRC_DIR)/bvh/bvh_traverse.c \
	$(SRC_DIR)/bvh/bvh_build_core.c \
	$(SRC_DIR)/bvh/bvh_build_helpers.c

# Combine all sources
SRCS = $(SRCS_CORE) $(SRCS_GEOMETRY) $(SRCS_UI) $(SRCS_UTILS) $(SRCS_LIGHTING) $(SRCS_BVH)

# Object files (automatic from sources)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ============================================
# Build Rules
# ============================================

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "✓ miniRT built successfully"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
```

### Benefits of This Organization
1. **Easy to Add Files**: Just add line to appropriate SRCS_* variable
2. **Clear Structure**: Comments and grouping make organization obvious
3. **Incremental Updates**: Can build per-section if needed during development
4. **Automatic Obj Directory**: `mkdir -p $(dir $@)` handles nested directories

### During Refactoring Process
When splitting a file, update Makefile immediately:
```makefile
# Before splitting metrics.c:
SRCS_CORE = \
	$(SRC_DIR)/core/metrics.c \
	...

# After splitting metrics.c into 3 files:
SRCS_CORE = \
	$(SRC_DIR)/core/metrics_tracking.c \
	$(SRC_DIR)/core/metrics_display.c \
	$(SRC_DIR)/core/metrics_fps.c \
	...
```

---

## Research Conclusions

### All Decisions Finalized
✅ **File Splitting**: Feature concern grouping, max 5 functions per file  
✅ **Function Extraction**: Calculation blocks, branches, initialization sequences  
✅ **Parameter Structures**: Domain-grouped structures for >4 parameter functions  
✅ **Validation**: Multi-layer automated + manual validation workflow  
✅ **Makefile**: Explicit listing organized by directory with comments

### No Outstanding Questions
All "NEEDS CLARIFICATION" items from Technical Context have been resolved through research.

### Ready for Phase 1
Research findings provide concrete patterns and guidelines for:
- Data model documentation (existing entities + new structures)
- Contract definition (refactoring rules)
- Quickstart guide implementation
- Agent context updates

### Risk Assessment After Research
**High risks mitigated**:
- File splitting strategy defined with clear boundaries
- Validation workflow ensures behavior preservation
- Performance measurement strategy in place

**Medium risks addressed**:
- Parameter structure design minimizes memory overhead
- Makefile organization supports maintainability
- Function extraction patterns preserve readability

**Low risks acceptable**:
- Formatting changes in isolated commits
- Documentation updates part of workflow
- Code review supported by clear guidelines

---

**Phase 0 Complete**: All research complete, ready to proceed to Phase 1 (Design & Contracts).
