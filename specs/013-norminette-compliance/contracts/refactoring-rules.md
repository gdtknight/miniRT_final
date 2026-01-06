# Refactoring Contracts: Norminette Compliance

**Feature**: Complete Norminette Compliance  
**Branch**: `013-norminette-compliance`  
**Date**: 2026-01-03  
**Phase**: 1 (Design & Contracts)

## Overview

This document defines the contracts (rules and guarantees) that MUST be maintained during norminette compliance refactoring. Since this is a refactoring task rather than new feature development, contracts focus on **behavior preservation**, **performance preservation**, and **build compatibility** rather than API design.

All refactoring work MUST adhere to these contracts to ensure the codebase remains functional and performant after achieving norminette compliance.

---

## Contract 1: Function Signature Preservation

### Rule
**Public function signatures MUST NOT change during refactoring**

### Rationale
External callers (other modules, test code, main program) depend on existing function signatures. Changing public signatures would break compilation and require extensive updates across the codebase.

### Allowed Transformations

#### ✅ Extract Private Helper Functions
```c
// BEFORE: Long function (35 lines)
void render_scene(t_scene *scene, t_camera *camera)
{
    // 10 lines of setup
    // 15 lines of rendering logic
    // 10 lines of post-processing
}

// AFTER: Extracted helpers
void render_scene(t_scene *scene, t_camera *camera)
{
    setup_rendering(scene, camera);
    execute_rendering(scene, camera);
    post_process_rendering(scene, camera);
}

static void setup_rendering(t_scene *scene, t_camera *camera)
{
    // 10 lines of setup
}

static void execute_rendering(t_scene *scene, t_camera *camera)
{
    // 15 lines of rendering logic
}

static void post_process_rendering(t_scene *scene, t_camera *camera)
{
    // 10 lines of post-processing
}
```

#### ✅ Move Functions Between Files (Same Signature)
```c
// BEFORE: In metrics.c (8 functions)
void update_fps(t_metrics *metrics, double delta_time);

// AFTER: Moved to metrics_fps.c (maintains signature)
void update_fps(t_metrics *metrics, double delta_time);
```

### Prohibited Transformations

#### ❌ Change Public Function Parameters
```c
// BEFORE: Public function
void render_scene(t_scene *scene, t_camera *camera);

// AFTER: VIOLATES CONTRACT - changed signature
void render_scene(t_render_params *params);  // BREAKS EXTERNAL CALLERS
```

#### ❌ Change Return Types
```c
// BEFORE
int initialize_window(int width, int height);

// AFTER: VIOLATES CONTRACT
t_window *initialize_window(int width, int height);  // BREAKS CALLERS
```

### Validation
- **Compile Test**: All existing code that calls public functions must compile without changes
- **Link Test**: Program must link successfully without undefined reference errors
- **Execution Test**: Program must run without segmentation faults or assertion failures

---

## Contract 2: Behavior Equivalence

### Rule
**Refactored code MUST produce identical output for identical input**

### Rationale
Refactoring should not change program behavior. Users expect the same rendering results, same exit codes, same error messages, and same performance characteristics after refactoring.

### Equivalence Criteria

#### ✅ Identical Output
```c
// BEFORE: Complex calculation
t_color calculate_color(t_ray *ray, t_scene *scene)
{
    t_color result;
    result.r = /* complex calculation */;
    result.g = /* complex calculation */;
    result.b = /* complex calculation */;
    return result;
}

// AFTER: Extracted helpers - MUST return identical result
t_color calculate_color(t_ray *ray, t_scene *scene)
{
    t_color result;
    result.r = calculate_red_component(ray, scene);
    result.g = calculate_green_component(ray, scene);
    result.b = calculate_blue_component(ray, scene);
    return result;
}
```

#### ✅ Reorder Independent Statements
```c
// BEFORE
void setup(void)
{
    init_camera();
    init_lighting();
    init_objects();
}

// AFTER: Reordered - OK if truly independent
void setup(void)
{
    init_lighting();  // No dependencies on camera
    init_camera();    // No dependencies on lighting
    init_objects();   // No dependencies on above
}
```

### Prohibited Transformations

#### ❌ Change Calculation Order (With Side Effects)
```c
// BEFORE: Order matters
void update_state(t_state *state)
{
    state->value = calculate_new_value(state);
    state->previous = state->value;  // Uses updated value
}

// AFTER: VIOLATES CONTRACT - wrong order
void update_state(t_state *state)
{
    state->previous = state->value;  // Uses OLD value - WRONG
    state->value = calculate_new_value(state);
}
```

#### ❌ Remove Error Checks
```c
// BEFORE: Validates input
int parse_scene(const char *file)
{
    if (!file)
        return (ERROR);
    // parsing logic
}

// AFTER: VIOLATES CONTRACT - removed validation
int parse_scene(const char *file)
{
    // parsing logic - NO NULL CHECK - WRONG
}
```

### Validation
- **Image Diff**: Rendered images must be bit-identical to pre-refactoring renders
- **Exit Code**: Program must exit with same codes for same inputs
- **Error Messages**: Error messages must be identical for invalid inputs
- **Automated Test**: All test scenes must produce identical output

---

## Contract 3: Performance Preservation

### Rule
**Performance regression MUST be ≤5% of baseline for rendering workloads**

### Rationale
miniRT is a real-time ray tracing application where performance is critical. While some overhead from additional function calls is acceptable, significant regression would impact user experience.

### Performance Budget
- **Acceptable Regression**: ≤5% increase in rendering time
- **Target**: 0% regression (function call overhead should be negligible with optimization)
- **Measurement**: Average rendering time across 5 runs of complex scene

### Allowed Patterns

#### ✅ Static Functions (Compiler Can Inline)
```c
// Helper functions should be static to enable inlining
static double calculate_distance(t_vector *a, t_vector *b)
{
    // Simple calculation - compiler will likely inline
}
```

#### ✅ Inline Hints for Hot Paths
```c
// For functions called in tight loops
static inline t_vector normalize(t_vector v)
{
    double len = vector_length(&v);
    return (t_vector){v.x / len, v.y / len, v.z / len};
}
```

#### ✅ Minimize Function Call Depth
```c
// GOOD: Two-level call hierarchy
void render_scene(t_scene *scene)
{
    for (each pixel)
        render_pixel(scene, x, y);  // Direct call to leaf function
}

// AVOID: Deep nesting
void render_scene(t_scene *scene)
{
    process_rendering(scene);  // Adds unnecessary indirection
}

void process_rendering(t_scene *scene)
{
    for (each pixel)
        render_pixel(scene, x, y);
}
```

### Monitoring

#### Performance Baseline
```bash
# Before refactoring
time ./miniRT scenes/complex_scene.rt > /dev/null

# Record: real 5.234s, user 5.180s, sys 0.042s
```

#### Performance Measurement
```bash
# After refactoring
time ./miniRT scenes/complex_scene.rt > /dev/null

# Record: real 5.412s, user 5.350s, sys 0.048s
# Regression: (5.412 - 5.234) / 5.234 = 3.4% ✅ WITHIN BUDGET
```

### Validation
- **Benchmark**: Run complex scene 5 times, compute average
- **Comparison**: Calculate percentage change from baseline
- **Threshold**: Fail if regression >5%
- **Profiling**: If regression detected, profile to identify hot spots

---

## Contract 4: Build Compatibility

### Rule
**All build targets MUST succeed after refactoring**

### Rationale
The project must continue to build successfully on all supported platforms (Linux, macOS) with all targets (main binary, bonus features).

### Build Requirements

#### ✅ Clean Build Success
```bash
make clean && make
# Exit code: 0
# No compiler errors
# No compiler warnings (with -Wall -Wextra -Werror)
```

#### ✅ All Targets Build
```bash
make clean
make              # Main target
make bonus        # Bonus features
make re           # Rebuild all
# All exit with code 0
```

#### ✅ Updated Makefile
```makefile
# When splitting metrics.c into 3 files:

# BEFORE
SRCS = src/core/metrics.c ...

# AFTER - All new files added
SRCS = src/core/metrics_tracking.c \
       src/core/metrics_display.c \
       src/core/metrics_fps.c \
       ...
```

### Header File Requirements

#### ✅ One Header Per Source File
```
src/core/metrics_tracking.c  →  includes/metrics_tracking.h
src/core/metrics_display.c   →  includes/metrics_display.h
src/core/metrics_fps.c       →  includes/metrics_fps.h
```

#### ✅ Header Guards
```c
// includes/metrics_tracking.h
#ifndef METRICS_TRACKING_H
# define METRICS_TRACKING_H

// Function declarations
void init_metrics(t_metrics *metrics);
void update_metrics(t_metrics *metrics, double delta);

#endif
```

#### ✅ Proper Includes
```c
// src/core/metrics_tracking.c
#include "metrics_tracking.h"   // Own header first
#include "structures.h"          // Then project headers
#include <stdlib.h>              // Then system headers
```

### Prohibited Patterns

#### ❌ Missing Makefile Updates
```makefile
# After creating new files, forgetting to add them:
SRCS = src/core/metrics.c ...  # OLD - missing new files

# Result: Linker errors (undefined reference)
```

#### ❌ Missing Headers
```c
// src/core/metrics_display.c calls function from metrics_tracking.c
// But doesn't include metrics_tracking.h

void render_metrics(void)
{
    update_metrics(...);  // ERROR: undefined reference
}
```

#### ❌ Circular Dependencies
```c
// metrics_tracking.h includes metrics_display.h
// metrics_display.h includes metrics_tracking.h
// Result: Compilation failure or infinite recursion
```

### Validation
- **Build Test**: `make clean && make && make bonus` all succeed
- **Compiler Flags**: Build with `-Wall -Wextra -Werror` (no warnings allowed)
- **Platform Test**: Build on both Linux and macOS if available
- **Dependency Check**: Verify no circular header dependencies

---

## Contract 5: Code Quality Standards

### Rule
**All refactored code MUST maintain 42 School coding standards and norminette compliance**

### Rationale
The entire purpose of this refactoring is to achieve norminette compliance. New code must not introduce new violations.

### Norminette Requirements

#### ✅ File Function Count
```c
// Each file MUST have ≤5 functions
// metrics_tracking.c: 5 functions ✅
void init_metrics(t_metrics *metrics);
void reset_metrics(t_metrics *metrics);
void update_metrics(t_metrics *metrics, double delta);
double calculate_average(t_metrics *metrics);
void store_sample(t_metrics *metrics, double value);
```

#### ✅ Function Line Count
```c
// Each function MUST be ≤25 lines (from { to })
void init_metrics(t_metrics *metrics)
{
    // Line 1
    metrics->fps = 0.0;
    // ... (max 25 lines total)
    // Line 25
}
```

#### ✅ Parameter Count
```c
// Functions MUST have ≤4 parameters
void render_scene(t_scene *scene, t_camera *camera);  // 2 params ✅

// If >4 needed, use parameter structure
void render_scene(t_scene *scene, t_render_config *config);  // 2 params ✅
```

#### ✅ Local Variable Count
```c
// Functions MUST declare ≤5 local variables
void calculate_color(t_ray *ray)
{
    t_color result;     // var 1
    double  intensity;  // var 2
    int     i;          // var 3
    double  distance;   // var 4
    t_light *light;     // var 5
    // Max 5 variables ✅
}
```

#### ✅ Header Formatting
```c
// Headers MUST use tabs for indentation, no trailing spaces
#ifndef METRICS_TRACKING_H
# define METRICS_TRACKING_H

typedef struct s_metrics
{
→   double→   fps;        // Tabs, no trailing spaces ✅
→   double→   frame_time;
}→   t_metrics;

#endif
```

### Documentation Requirements

#### ✅ 42 Header in All Files
```c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics_tracking.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: <user> <email>                             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: YYYY/MM/DD HH:MM:SS by <user>       #+#    #+#                 */
/*   Updated: YYYY/MM/DD HH:MM:SS by <user>      ###   ########.fr           */
/*                                                                            */
/* ************************************************************************** */
```

#### ✅ Function Comments
```c
/**
 * @brief Initializes metrics structure with default values
 * @param metrics Pointer to metrics structure to initialize
 * @return None
 */
void init_metrics(t_metrics *metrics)
{
    // Implementation
}
```

### Validation
- **Norminette Check**: Run `norminette src/ includes/` - must report 0 errors
- **Style Check**: Visual inspection of new code for consistency
- **Documentation Check**: Verify all new functions have comments
- **Header Check**: Verify 42 header in all new files

---

## Contract 6: Testing and Validation

### Rule
**All refactoring changes MUST be validated through automated and manual testing**

### Rationale
Behavior preservation cannot be assumed - it must be proven through comprehensive testing at each stage of refactoring.

### Testing Requirements

#### Level 1: Unit Testing (Per File Split)
```bash
# After splitting each file:
# 1. Build test
make clean && make

# 2. Norminette test
norminette src/core/metrics*.c includes/metrics*.h

# 3. Quick smoke test
./miniRT scenes/simple.rt > test_output.ppm
echo $?  # Should be 0
```

#### Level 2: Integration Testing (Per Batch)
```bash
# After completing a batch of file splits:
# 1. All test scenes
for scene in scenes/*.rt; do
    ./miniRT "$scene" > "current_renders/$(basename $scene .rt).ppm"
done

# 2. Image comparison
for scene in scenes/*.rt; do
    name=$(basename "$scene" .rt)
    diff "baseline_renders/${name}.ppm" "current_renders/${name}.ppm"
done
```

#### Level 3: Regression Testing (Final Validation)
```bash
# After all refactoring complete:
# 1. Zero errors
norminette src/ includes/ | grep "Error:" | wc -l  # Must be 0

# 2. Full build
make clean && make && make bonus  # All must succeed

# 3. All scenes identical
./scripts/validate_all_scenes.sh  # Must pass

# 4. Performance within budget
./scripts/benchmark.sh  # Must be ≤5% regression
```

### Test Coverage Requirements
- **Simple Scenes**: Basic shapes, single light (sanity check)
- **Complex Scenes**: Multiple objects, multiple lights, shadows (stress test)
- **Edge Cases**: Empty scenes, large scenes, invalid inputs (robustness)

### Validation Checklist
```
Per File Split:
[ ] make succeeds
[ ] norminette reports no new errors
[ ] Program runs on simple scene
[ ] Exit code is 0

Per Batch (5-10 files):
[ ] All test scenes render
[ ] Image diff shows no changes
[ ] No new compiler warnings
[ ] Performance measured

Final (All refactoring):
[ ] norminette reports exactly 0 errors
[ ] All build targets succeed
[ ] All test scenes identical to baseline
[ ] Performance within ±5% of baseline
[ ] Memory leaks: 0 (valgrind check)
[ ] Code review: Improved readability
```

---

## Contract Enforcement

### Pre-Commit Checks
Before committing any refactoring changes:
1. ✅ Run norminette on changed files (0 errors)
2. ✅ Build succeeds (make clean && make)
3. ✅ Smoke test passes (simple scene renders)
4. ✅ Git diff reviewed (verify only intended changes)

### Pre-Merge Checks
Before merging refactoring branch:
1. ✅ All contracts validated (Contracts 1-6)
2. ✅ Full test suite passes
3. ✅ Performance benchmarked (≤5% regression)
4. ✅ Code review completed
5. ✅ Documentation updated

### Rollback Procedure
If any contract is violated:
```bash
# Immediate rollback
git reset --hard HEAD~1

# Or revert specific file
git checkout HEAD -- src/path/to/file.c

# Rebuild and retest
make clean && make
./miniRT scenes/simple.rt
```

---

## Contract Summary Table

| Contract | Rule | Validation Method | Acceptable Violation |
|----------|------|-------------------|---------------------|
| 1. Signature Preservation | Public functions unchanged | Compilation test | None |
| 2. Behavior Equivalence | Identical output | Image diff, exit codes | None |
| 3. Performance Preservation | ≤5% regression | Benchmark comparison | ≤5% slower |
| 4. Build Compatibility | All targets build | make clean && make && make bonus | None |
| 5. Code Quality | Norminette compliant | norminette check | 0 errors |
| 6. Testing & Validation | Comprehensive testing | Test suite | None |

---

**Phase 1 Contracts Complete**: All refactoring rules defined, ready for implementation.
