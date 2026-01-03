# Quick Start: Maintaining Clean Code Organization

**Feature**: 012-code-cleanup-refactor  
**Date**: 2026-01-03  
**Audience**: Developers working on miniRT after refactoring

## Overview

This guide provides practical guidelines for maintaining the clean code organization established by the 012-code-cleanup-refactor feature. Follow these practices to keep the codebase maintainable and compliant with 42 School standards.

## Quick Reference

### Adding a New Function

```
1. Identify domain (Math, Object, Ray, etc.)
2. Declare in domain's header (e.g., vec3.h)
3. Implement in domain's source directory (e.g., src/math/)
4. Run norminette on modified files
5. Test compilation and functionality
```

### Removing a Function

```
1. Search for all usages: grep -r "function_name(" src/
2. Verify zero calls (excluding callbacks)
3. Remove implementation from source file
4. Remove declaration from header file
5. Compile and test
```

### Organizing New Code

```
1. Check existing domains - does new code fit?
2. If yes: Add to existing domain
3. If no: Consider if new domain is justified
4. Follow tier rules (lower tiers don't depend on higher)
5. Update documentation if creating new domain
```

---

## Domain Reference

### Quick Domain Lookup

| Domain | Header | Source Dir | For... |
|--------|--------|------------|---------|
| **Math** | `vec3.h` | `src/math/` | Vector operations, geometry |
| **Object** | `objects.h` | *(embedded)* | Scene object definitions |
| **Ray** | `ray.h` | `src/ray/` | Ray structures, ray operations |
| **Spatial** | `spatial.h` | `src/spatial/` | BVH, acceleration structures |
| **Lighting** | `shadow.h` | `src/lighting/` | Lights, shadows, attenuation |
| **Rendering** | `render_state.h` | `src/render/` | Rendering engine, camera |
| **Parser** | `parser.h` | `src/parser/` | Scene file parsing |
| **Window** | `window.h` | `src/window/` | MLX window, events |
| **HUD** | `hud.h` | `src/hud/` | Performance overlay |
| **KeyGuide** | `keyguide.h` | `src/keyguide/` | Key guide display |
| **Metrics** | `metrics.h` | `src/render/metrics*.c` | Performance tracking |
| **Core** | `minirt.h` | `main.c` | Application entry, integration |

### Dependency Quick Check

**Can domain A use domain B?**
1. Find tier of A and B (see tier table below)
2. Rule: Higher tier → may use → Lower tier
3. Rule: Same tier → generally no (except documented peer dependencies)
4. Rule: Lower tier → never → Higher tier

**Tier Table**:
```
Tier 5: Core
Tier 4: Window, HUD, KeyGuide
Tier 3: Rendering, Metrics
Tier 2: Spatial, Lighting, Parser
Tier 1: Object, Ray
Tier 0: Math
```

**Examples**:
- ✅ Rendering (Tier 3) → Math (Tier 0) ✓ Allowed
- ✅ HUD (Tier 4) → Metrics (Tier 3) ✓ Allowed
- ❌ Math (Tier 0) → Rendering (Tier 3) ✗ Forbidden (upward)
- ❌ Parser (Tier 2) → Window (Tier 4) ✗ Forbidden (upward)

---

## 42 Norminette Compliance

### Pre-Flight Checklist

Before committing code:
- [ ] Run `norminette` on all modified files
- [ ] Zero errors, zero warnings
- [ ] Functions ≤25 lines
- [ ] Files ≤5 functions (non-static)
- [ ] Lines ≤80 characters
- [ ] No `#define` in `.c` files

### Common Fixes

**Problem**: Function exceeds 25 lines
```c
// ❌ Bad: 30-line function
int calculate_lighting(t_scene *scene, t_hit *hit) {
    // ... 30 lines of code
}

// ✅ Good: Extract helpers
static t_color get_ambient(t_scene *scene) {
    // 5 lines
}

static t_color get_diffuse(t_light *light, t_hit *hit) {
    // 8 lines
}

int calculate_lighting(t_scene *scene, t_hit *hit) {
    t_color ambient = get_ambient(scene);
    t_color diffuse = get_diffuse(&scene->light, hit);
    return combine_colors(ambient, diffuse);  // 10 lines total
}
```

**Problem**: Line exceeds 80 characters
```c
// ❌ Bad: 95 characters
result = calculate_shadow_with_attenuation(scene, light, hit_point, normal, shadow_ray);

// ✅ Good: Split parameters
result = calculate_shadow_with_attenuation(
    scene, light, hit_point, normal, shadow_ray);

// ✅ Better: Extract intermediate
t_shadow_params params = init_shadow_params(light, hit_point, normal);
result = calculate_shadow(scene, params, shadow_ray);
```

**Problem**: `#define` in source file
```c
// ❌ Bad: In src/lighting/shadow.c
#define SHADOW_BIAS 0.001

// ✅ Good: In includes/shadow.h
#define SHADOW_BIAS 0.001
```

---

## 42 School Function Constraints

### Allowed Functions Only

**Standard C**: `open`, `close`, `read`, `write`, `printf`, `malloc`, `free`, `perror`, `strerror`, `exit`  
**Math**: All from `math.h` (`sqrt`, `pow`, `sin`, `cos`, etc.)  
**MiniLibX**: All `mlx_*` functions  
**Custom**: Your `libft`, `get_next_line`

### Prohibited Functions - Never Use

**Threading**: ❌ `pthread_create`, `pthread_join`, `pthread_mutex_*`  
**Multiprocessing**: ❌ `fork`, `pipe`, `waitpid`  
**External Libs**: ❌ OpenMP, TBB, SIMD intrinsics

**If you need optimization**:
- ✅ Use BVH or spatial acceleration (algorithmic)
- ✅ Cache expensive calculations
- ✅ Optimize math operations
- ❌ Do NOT use multithreading/multiprocessing

---

## Code Organization Patterns

### Pattern 1: Adding Vector Operation

**Scenario**: Need a new vector function `vec3_lerp`

```c
// 1. Declare in includes/vec3.h
t_vec3  vec3_lerp(t_vec3 a, t_vec3 b, double t);

// 2. Implement in src/math/vector_ops.c (or vector.c)
t_vec3  vec3_lerp(t_vec3 a, t_vec3 b, double t)
{
    t_vec3  result;

    result.x = a.x + (b.x - a.x) * t;
    result.y = a.y + (b.y - a.y) * t;
    result.z = a.z + (b.z - a.z) * t;
    return (result);
}

// 3. Run norminette
norminette includes/vec3.h src/math/vector_ops.c

// 4. Test usage
t_vec3 interpolated = vec3_lerp(start, end, 0.5);
```

---

### Pattern 2: Adding Object Intersection

**Scenario**: Need to add cone intersection

```c
// 1. Add cone type to includes/objects.h
typedef struct s_cone {
    t_vec3  position;
    t_vec3  axis;
    double  angle;
    t_color color;
}   t_cone;

// Add to object union
typedef struct s_object {
    t_object_type   type;
    union {
        t_sphere    sphere;
        t_plane     plane;
        t_cylinder  cylinder;
        t_cone      cone;  // NEW
    };
}   t_object;

// 2. Implement intersection in src/ray/ or appropriate location
int intersect_cone(t_ray *ray, t_cone *cone, t_hit_info *hit)
{
    // Implementation
}

// 3. Update main intersection dispatcher
int intersect_object(t_ray *ray, t_object *obj, t_hit_info *hit)
{
    if (obj->type == OBJ_CONE)
        return (intersect_cone(ray, &obj->cone, hit));
    // ... other types
}
```

---

### Pattern 3: Adding UI Feature

**Scenario**: Add FPS counter to HUD

```c
// 1. Add state to includes/hud.h
typedef struct s_hud {
    // ... existing fields
    double  fps;  // NEW
}   t_hud;

// 2. Add calculation in src/hud/hud_performance.c
void    update_hud_fps(t_hud *hud, double delta_time)
{
    if (delta_time > 0)
        hud->fps = 1.0 / delta_time;
}

// 3. Add rendering in src/hud/hud_render.c
void    render_hud_fps(t_window *win, t_hud *hud)
{
    char    fps_str[32];

    snprintf(fps_str, sizeof(fps_str), "FPS: %.1f", hud->fps);
    mlx_string_put(win->mlx, win->mlx_win, 10, 30, 0xFFFFFF, fps_str);
}

// 4. Domain check: HUD (Tier 4) using Window (Tier 4) ✓ OK (peer)
```

---

## Testing Workflow

### After Code Changes

```bash
# 1. Run norminette on changed files
norminette includes/vec3.h src/math/vector.c

# 2. Compile project
make

# 3. Test with scenes
./miniRT scenes/simple_sphere.rt
./miniRT scenes/complex_scene.rt

# 4. Verify no memory leaks (if available)
valgrind ./miniRT scenes/simple_sphere.rt

# 5. If all pass → commit
git add includes/vec3.h src/math/vector.c
git commit -m "feat(math): add vec3_lerp for vector interpolation"
```

---

## Common Mistakes to Avoid

### ❌ Mistake 1: Circular Dependencies

```c
// ❌ Bad: In shadow.h
#include "render_state.h"

// ❌ Bad: In render_state.h  
#include "shadow.h"

// Result: Circular dependency!
```

**Fix**: Use forward declarations
```c
// ✅ Good: In shadow.h
typedef struct s_render_state t_render_state;  // forward declaration
// ... use pointer to t_render_state
```

---

### ❌ Mistake 2: Upward Dependencies

```c
// ❌ Bad: In src/math/vector.c
#include "window.h"  // Math (Tier 0) depending on Window (Tier 4)!

void vec3_debug(t_vec3 v) {
    mlx_string_put(...);  // VIOLATION
}
```

**Fix**: Keep lower tiers independent
```c
// ✅ Good: In src/math/vector.c
void vec3_debug(t_vec3 v) {
    printf("Vec3: (%f, %f, %f)\n", v.x, v.y, v.z);  // Use standard output
}
```

---

### ❌ Mistake 3: Forgetting Norminette

```c
// ❌ Bad: 35-line function, 90-character line
int very_long_function_that_does_too_many_things_and_violates_norminette(int a, int b, int c) {
    // 35 lines of code...
}
```

**Fix**: Run norminette early and often
```bash
# Before committing
norminette path/to/file.c
# Fix violations immediately
```

---

## Where to Get Help

### Documentation

- **This guide**: `specs/012-code-cleanup-refactor/quickstart.md`
- **Full spec**: `specs/012-code-cleanup-refactor/spec.md`
- **Domain boundaries**: `specs/012-code-cleanup-refactor/contracts/domain-boundaries.md`
- **Norminette rules**: `specs/012-code-cleanup-refactor/contracts/norminette-compliance.md`
- **Function usage**: `specs/012-code-cleanup-refactor/contracts/function-usage.md`

### Quick Checks

**"Where should I put this function?"**
→ Check domain reference table above

**"Can domain A use domain B?"**
→ Check tier table and dependency rules

**"Why is norminette failing?"**
→ Check norminette compliance contract

**"Is this function allowed?"**
→ Check 42 School constraints section (allowed/prohibited lists)

---

## Summary: Golden Rules

1. ✅ **Know your domain** - Every function belongs to a domain
2. ✅ **Respect tiers** - Lower tiers never depend on higher tiers
3. ✅ **Run norminette** - Before every commit
4. ✅ **Keep functions small** - ≤25 lines, ≤80 chars per line
5. ✅ **No defines in `.c`** - Always in headers
6. ✅ **Only allowed functions** - No pthread, fork, or external libs
7. ✅ **Test after changes** - Compile and run scenes
8. ✅ **One domain per header** - No mixing concerns

---

## Quick Command Reference

```bash
# Norminette check
norminette includes/*.h src/**/*.c

# Check for defines in source files
grep -r "#define" src/ --include="*.c"

# Search for function usage
grep -r "function_name(" src/ includes/

# Check for prohibited functions
grep -r "pthread_\|fork\|pipe" src/ includes/

# Compile
make

# Test with scene
./miniRT scenes/test.rt

# Memory check
valgrind --leak-check=full ./miniRT scenes/test.rt
```

---

## Version

**Document Version**: 1.0  
**Last Updated**: 2026-01-03  
**Maintained By**: miniRT development team

For questions or clarifications, refer to the full specification and contracts in `specs/012-code-cleanup-refactor/`.
