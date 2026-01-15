# Implementation Plan: Fix BVH Integration for Scene Rendering

**Branch**: `017-fix-bvh-rendering` | **Date**: 2026-01-15 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/017-fix-bvh-rendering/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Fix BVH (Bounding Volume Hierarchy) integration in the ray tracer's `trace_ray()` function to enable accelerated rendering while maintaining visual output parity between BVH and non-BVH rendering paths. Primary issue identified: `bvh_enabled` flag defaults to 0, preventing BVH from being used even when successfully initialized. Secondary concerns include data structure compatibility between `t_hit` and `t_hit_record`, and proper BVH initialization sequence during scene loading.

## Technical Context

**Language/Version**: C (C99 standard, 42 School norminette compliant)  
**Primary Dependencies**: MiniLibX (X11-based graphics), libft (custom), math.h  
**Storage**: Scene files (.rt format), no persistent storage required  
**Testing**: Manual testing with scene files in `scenes/` directory, test_miniRT.sh script  
**Target Platform**: Linux and macOS with X11 support  
**Project Type**: Single binary (miniRT executable)  
**Performance Goals**: 2-10x speedup with BVH on complex scenes (50+ objects), maintain 60 FPS interaction  
**Constraints**: Must comply with 42 School allowed functions, no threading/forking, norminette compliance  
**Scale/Scope**: Render scenes with 1-1000+ objects (spheres, planes, cylinders)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**42 School Constraints Check** (Principle VII):
- ✅ All functions used are from allowed list (libc: malloc, free; math.h; mlx_*)
- ✅ pthread functions are NOT used (confirmed in codebase)
- ✅ fork/pipe/multiprocessing are NOT used (confirmed)  
- ✅ External parallelization libraries (SIMD, OpenMP, etc.) are NOT used (confirmed)
- ✅ Optimizations use only allowed techniques: BVH is an algorithmic optimization (spatial partitioning)

**Code Quality Automation** (Principle II):
- ✅ All changes MUST pass norminette check with zero errors
- ✅ Build MUST succeed without warnings
- ✅ Memory leak check (valgrind) MUST pass with zero leaks
- ✅ Changes do not introduce new quality gate failures

**Project Structure Standards** (Principle I):
- ✅ BVH implementation properly organized in `src/spatial/` directory
- ✅ Headers properly located in `includes/` directory
- ✅ Test scenes available in `scenes/` directory
- ✅ No changes to CI scripts or documentation hierarchy needed

**Bilingual Specification** (Principle VI):
- ✅ Specification exists in both English (`specs/`) and Korean (`docs/specs/`)
- ✅ Will update both versions in same commit if spec changes required

**GATE STATUS**: ✅ PASS - All constitution requirements met. No violations to justify.

## Project Structure

### Documentation (this feature)

```text
specs/[###-feature]/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
src/
├── spatial/              # BVH acceleration structures
│   ├── bvh_init.c       # BVH initialization & scene_build_bvh()
│   ├── bvh_traverse.c   # bvh_intersect(), bvh_node_intersect()
│   ├── bvh_lifecycle.c  # bvh_create(), bvh_destroy()
│   ├── bvh_build_*.c    # BVH tree construction algorithms
│   └── aabb.c           # Axis-aligned bounding box operations
├── render/
│   ├── trace.c          # trace_ray() - main ray tracing function (FIX HERE)
│   ├── render_state.c   # render_state_init() - initializes bvh_enabled flag (FIX HERE)
│   └── intersect_*.c    # Object-specific intersection functions
├── parser/
│   └── parser.c         # Scene loading, calls scene_build_bvh() (CHECK HERE)
└── hud/
    └── hud_performance.c # BVH status display in HUD

includes/
├── spatial.h            # BVH structures: t_bvh, t_bvh_node, t_aabb
├── ray.h                # t_hit, t_hit_record (typedef alias), t_ray
├── render_state.h       # t_render_state with bvh and bvh_enabled fields
└── minirt.h             # Main header, includes render_state.h

tests/
└── (manual testing with scenes/*.rt files)
```

**Structure Decision**: Single project structure is appropriate. BVH implementation is already well-organized in `src/spatial/`. The fix requires minimal changes to `src/render/trace.c` and `src/render/render_state.c` to properly enable and use BVH.

## Complexity Tracking

**No violations requiring justification**. All constitution checks pass. The BVH implementation uses only allowed functions and algorithmic optimizations permitted by 42 School constraints.

---

## Phase 0: Research & Problem Diagnosis

### Current State Analysis (from codebase inspection)

**BVH Implementation Status**:
- ✅ BVH data structures properly defined (`t_bvh`, `t_bvh_node`, `t_aabb` in `spatial.h`)
- ✅ BVH construction algorithms implemented (`bvh_build_*.c`, `bvh_lifecycle.c`)
- ✅ BVH traversal implemented (`bvh_intersect()`, `bvh_node_intersect()` in `bvh_traverse.c`)
- ✅ Scene initialization calls `scene_build_bvh()` in `parser.c` after successful validation
- ✅ Data structure compatibility: `typedef t_hit t_hit_record;` confirms structures are identical (ray.h:38)

**Root Cause Identified**:
1. **Primary Issue**: `render_state.c:37` initializes `bvh_enabled = 0`, causing BVH to be disabled by default
2. **Secondary Issue**: `trace_ray()` early returns black screen when BVH path is taken but finds no intersection (line 163)
3. **Logic Issue**: `scene_build_bvh()` checks `bvh_enabled` flag before building, creating chicken-egg problem

**Current Flow (Broken)**:
```
Scene Load → render_state_init() sets bvh_enabled=0
          → validate_scene() passes
          → scene_build_bvh() checks bvh_enabled (0) → RETURNS EARLY, no BVH built
          → trace_ray() sees bvh=NULL or disabled → falls back to brute force
```

**Why Black Screen Occurs After "Fix"**:
When user manually enables BVH without proper initialization:
```
trace_ray() → checks bvh->enabled (manually set to 1)
          → calls bvh_intersect() with NULL or empty root
          → returns early at line 163 with black color {0,0,0}
          → Never tries fallback non-BVH path
```

### Research Questions Resolved

**Q1: Is BVH initialization sequence correct?**
- **A**: No. `bvh_enabled` flag is checked before building BVH, but flag defaults to 0. Need to either:
  - Set `bvh_enabled = 1` before calling `scene_build_bvh()`, OR
  - Remove `bvh_enabled` check from `scene_build_bvh()` and build BVH unconditionally

**Q2: Are t_hit and t_hit_record compatible?**
- **A**: Yes, perfectly. `ray.h:38` defines `typedef t_hit t_hit_record;` making them the same type.

**Q3: How should BVH be enabled/disabled?**
- **A**: Current design uses `render_state.bvh_enabled` flag AND `bvh->enabled` field. Need to:
  - Set `bvh_enabled = 1` in `render_state_init()` to enable by default
  - OR provide runtime toggle mechanism via key handler
  - Keep BVH pointer check for safety (NULL means not built)

**Q4: What's the correct fallback behavior?**
- **A**: When BVH path fails, `trace_ray()` should continue to non-BVH checks instead of early return.

### Decisions

**Decision 1: Enable BVH by Default**
- **Rationale**: Users expect BVH to work. Project is a ray tracer optimization showcase.
- **Implementation**: Change `bvh_enabled = 0` to `bvh_enabled = 1` in `render_state_init()`

**Decision 2: Fix trace_ray() Fallback Logic**
- **Rationale**: Early return on BVH failure prevents fallback to working non-BVH path
- **Implementation**: Remove early black screen return, let code flow to brute force checks

**Decision 3: Keep BVH Toggle Capability**
- **Rationale**: Testing requirement - need to compare BVH vs non-BVH rendering
- **Implementation**: Add key handler to toggle `bvh->enabled` flag at runtime (future task)

**Decision 4: Add Diagnostic Logging**
- **Rationale**: Debug future BVH issues, meet requirement FR-009
- **Implementation**: Log BVH status during initialization and when used in trace_ray()

**Alternatives Considered**:
- Build BVH unconditionally regardless of flag → Rejected: wastes memory for scenes where BVH not wanted
- Remove BVH entirely → Rejected: defeats purpose of optimization
- Always use BVH, no fallback → Rejected: fragile, violates robustness requirement

---

## Phase 1: Design & Implementation Strategy

### Data Model Changes

**No data model changes required**. Existing structures are correct:
- `t_hit` and `t_hit_record` are already compatible (typedef alias)
- `t_render_state` already contains `bvh` pointer and `bvh_enabled` flag
- `t_bvh` structure properly designed with `enabled` field

### Code Changes Required

**File 1: `src/render/render_state.c`**
```c
// Line 37: Change default BVH state
- state->bvh_enabled = 0;
+ state->bvh_enabled = 1;  // Enable BVH by default
```

**File 2: `src/render/trace.c`**
```c
// Lines 159-164: Fix early return logic
- if (scene->render_state.bvh && scene->render_state.bvh->enabled)
- {
-     if (bvh_intersect(scene->render_state.bvh, *ray, &hit, scene))
-         return (apply_lighting(scene, &hit));
-     return ((t_color){0, 0, 0});  // WRONG: early return prevents fallback
- }
+ if (scene->render_state.bvh && scene->render_state.bvh->enabled)
+ {
+     if (bvh_intersect(scene->render_state.bvh, *ray, &hit, scene))
+         hit_found = 1;
+ }
+ // Continue to non-BVH checks if BVH disabled or didn't find intersection
+ if (!hit_found)
+ {
```

**File 3: `src/render/trace.c` (continued)**
```c
// Lines 165-173: Wrap non-BVH checks in condition
+ if (!hit_found)
+ {
      if (check_sphere_intersections(scene, ray, &hit))
          hit_found = 1;
      if (check_plane_intersections(scene, ray, &hit))
          hit_found = 1;
      if (check_cylinder_intersections(scene, ray, &hit))
          hit_found = 1;
+ }
  if (hit_found)
      return (apply_lighting(scene, &hit));
  return ((t_color){0, 0, 0});
```

### API Contracts

**No public API changes**. All functions maintain existing signatures:
- `trace_ray(t_scene *scene, t_ray *ray)` → `t_color` (unchanged)
- `bvh_intersect(t_bvh *bvh, t_ray ray, t_hit_record *hit, void *scene)` → `int` (unchanged)
- `scene_build_bvh(t_scene *scene)` → `void` (unchanged)

### Testing Strategy

**Test Scenarios** (maps to user stories in spec):
1. **Basic Scene Rendering** (US-001):
   - Load `scenes/simple_sphere.rt` with 1 sphere
   - Verify sphere renders with correct color and lighting
   - Toggle BVH off, verify identical output

2. **Complex Scene Performance** (US-002):
   - Load `scenes/many_objects.rt` with 50+ objects
   - Measure render time with BVH enabled
   - Toggle BVH off, measure render time
   - Verify BVH is 2x+ faster

3. **Fallback Behavior** (US-003):
   - Manually corrupt BVH pointer (test mode)
   - Verify scene still renders using fallback
   - Check logs for fallback indication

4. **Edge Cases**:
   - Empty scene (0 objects) → BVH shouldn't be built, no crash
   - Single object scene → BVH builds but minimal benefit
   - All objects outside view → Both paths return black (correct)

**Validation Commands**:
```bash
# Build and test basic functionality
make re
./miniRT scenes/simple_sphere.rt

# Test with complex scene
./miniRT scenes/many_objects.rt

# Run full test suite
./test_miniRT.sh

# Memory leak check
valgrind --leak-check=full ./miniRT scenes/simple_sphere.rt
```

### Quickstart for Developers

**To verify BVH is working**:
1. Build: `make re`
2. Run: `./miniRT scenes/simple_sphere.rt`
3. Check HUD: Should show "BVH: Enabled" at top
4. Verify scene renders (not black screen)
5. Press 'B' key to toggle BVH on/off (if implemented)
6. Verify visual output identical in both modes

**To debug BVH issues**:
1. Add debug prints in `trace_ray()` to log BVH path taken
2. Check `scene->render_state.bvh` is non-NULL
3. Check `scene->render_state.bvh->enabled` is 1
4. Check `scene->render_state.bvh->root` is non-NULL
5. Use `bvh_vis` tools to visualize BVH tree structure

---

## Phase 2: Task Breakdown (Execution Plan)

**NOTE**: Phase 2 detailed task breakdown will be generated by `/speckit.tasks` command.
This is a preview of the task structure:

### Task Categories

**Category 1: Core Fixes** (Priority: P0 - Critical)
- Task 1.1: Enable BVH by default in render_state_init()
- Task 1.2: Fix trace_ray() early return logic
- Task 1.3: Implement proper fallback to non-BVH checks

**Category 2: Testing & Validation** (Priority: P1 - High)
- Task 2.1: Test basic scene rendering (US-001)
- Task 2.2: Test complex scene performance (US-002)
- Task 2.3: Validate memory usage with valgrind
- Task 2.4: Verify norminette compliance

**Category 3: Documentation** (Priority: P2 - Medium)
- Task 3.1: Update Korean documentation (docs/specs/)
- Task 3.2: Add BVH troubleshooting guide
- Task 3.3: Document BVH toggle key handler

**Category 4: Future Enhancements** (Priority: P3 - Low)
- Task 4.1: Add runtime BVH toggle key (Press 'B')
- Task 4.2: Add BVH statistics to HUD (node count, depth)
- Task 4.3: Optimize BVH build for very large scenes (1000+ objects)

### Estimated Effort

- Core fixes: 2-4 hours (minimal code changes)
- Testing: 2-3 hours (manual testing with various scenes)
- Documentation: 1-2 hours (update specs and troubleshooting)
- **Total**: 5-9 hours for complete implementation and validation

### Success Metrics (from spec SC-001 to SC-006)

- ✅ SC-001: 100% scene rendering success rate (no black screens)
- ✅ SC-002: Pixel-identical output between BVH/non-BVH modes
- ✅ SC-003: 2x+ speedup on complex scenes (50+ objects)
- ✅ SC-004: 100% uptime regardless of BVH state
- ✅ SC-005: Clear BVH status in logs/HUD
- ✅ SC-006: BVH toggle works within 1 second

---

## Re-evaluation: Constitution Check (Post-Design)

Reviewing all design decisions against constitution principles:

**42 School Constraints** (Principle VII): ✅ PASS
- No new functions added beyond allowed list
- All changes are algorithmic/logic fixes, no external libraries
- BVH remains pure C implementation with allowed functions

**Code Quality Automation** (Principle II): ✅ PASS  
- Changes affect only 2 files (render_state.c, trace.c)
- Both files already norminette compliant
- No new memory allocations → no new leak risks
- Will verify with norminette and valgrind before merge

**Project Structure Standards** (Principle I): ✅ PASS
- No new files created
- No changes to directory structure
- All changes in existing src/render/ directory

**Bilingual Specification** (Principle VI): ✅ PASS
- Plan created in English (specs/)
- Will create matching Korean version (docs/specs/)
- Both versions will be committed together

**FINAL GATE STATUS**: ✅✅ PASS - All principles maintained. Ready to proceed to implementation.

---

## Appendix: Diagnostic Information

### Key Files and Line Numbers
- `src/render/render_state.c:37` - bvh_enabled initialization
- `src/render/trace.c:159-174` - trace_ray() BVH integration
- `src/spatial/bvh_init.c:105-106` - bvh_enabled check
- `src/parser/parser.c` - scene_build_bvh() call location
- `includes/ray.h:38` - t_hit_record typedef

### BVH Status Indicators
- HUD displays "BVH: Enabled/Disabled" status
- `scene->render_state.bvh != NULL` → BVH built
- `scene->render_state.bvh->enabled == 1` → BVH active
- `scene->render_state.bvh->root != NULL` → BVH has data

### Common Issues & Solutions
| Issue | Symptom | Solution |
|-------|---------|----------|
| BVH not initialized | bvh == NULL | Set bvh_enabled=1 before scene_build_bvh() |
| Black screen | No objects rendered | Fix early return in trace_ray() |
| BVH ignored | Falls back to brute force | Check bvh->enabled flag is 1 |
| Performance same | BVH not helping | Verify BVH path is taken with debug logs |
