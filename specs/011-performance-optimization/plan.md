# Implementation Plan: Complex Scene Performance Optimization

**Branch**: `001-performance-optimization` | **Date**: 2026-01-01 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/001-performance-optimization/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Implement BVH (Bounding Volume Hierarchy) acceleration structure to optimize ray-object intersection tests, improving FPS for complex scenes from <1 FPS to 3-5 FPS. Includes real-time performance metrics display and optimized cylinder intersection tests while maintaining visual accuracy and complying with 42 School constraints (no multithreading/external libraries).

## Technical Context

**Language/Version**: C (C99 standard)
**Primary Dependencies**: MiniLibX (42 School graphics library), math.h, libft (custom C library)
**Storage**: In-memory scene data structures (no persistent storage)
**Testing**: Manual testing with .rt scene files, valgrind for memory leak detection
**Target Platform**: Linux and macOS (42 School evaluation platforms)
**Project Type**: Single project (real-time graphics renderer)
**Performance Goals**: 3-5 FPS for complex scenes (15+ objects), <2s BVH construction time, 60% reduction in intersection tests
**Constraints**: 
- No multithreading (pthread prohibited by 42 School)
- No external optimization libraries (SIMD, OpenMP prohibited)
- 512 MB memory limit for BVH structure
- Must maintain pixel-perfect visual accuracy
- BVH tree depth limited to 64 levels
**Scale/Scope**: Scenes with up to 50 objects, real-time camera movement, multiple ray types (primary/shadow/reflection)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Project Structure Standards** (Principle I):
- ✅ Implementation files will be in src/optimization/ and src/bvh/
- ✅ Test scenes in scenes/ directory
- ✅ Performance metrics code clearly separated
- ✅ No scattered automation scripts

**Code Quality Automation** (Principle II):
- ✅ All code will pass norminette
- ✅ Valgrind memory leak checks mandatory
- ✅ Build must succeed without warnings
- ✅ CI quality gates enforced

**Documentation Standards** (Principle III):
- ✅ Implementation will update docs/ with Korean documentation
- ✅ Wiki will be auto-synchronized on release
- ✅ Code comments in English (Doxygen style)
- ✅ Feature specification already in specs/001-performance-optimization/

**42 School Constraints Check** (Principle VII):
- ✅ Only allowed functions used: malloc, free, math.h functions, mlx_* functions, libft
- ✅ pthread functions NOT used (no multithreading)
- ✅ fork/pipe/multiprocessing NOT used
- ✅ External parallelization libraries (SIMD, OpenMP, etc.) NOT used
- ✅ Optimizations use ONLY allowed techniques:
  - **Algorithmic**: BVH spatial hierarchy (binary tree structure)
  - **Caching**: Precomputed bounding boxes, stored traversal results
  - **Math optimization**: Simplified ray-box intersection tests, algebraic optimizations
  - **Early termination**: Ray culling via bounding box tests
  - **Memory layout**: Cache-friendly BVH node structure

**Pre-Phase-0 Status**: ✅ PASS - All constraints satisfied, no violations requiring justification

---

**Post-Phase-1 Re-evaluation**: ✅ PASS

Design artifacts reviewed:
- ✅ **research.md**: All techniques comply with 42 constraints (no pthread, SIMD, or external libs)
- ✅ **data-model.md**: Data structures use only standard C types and allowed functions
- ✅ **contracts/bvh_api.md**: API uses only malloc, free, math.h (fminf/fmaxf), libft functions
- ✅ **contracts/metrics_api.md**: Uses clock_gettime (allowed), mlx_string_put (MiniLibX)
- ✅ **quickstart.md**: Integration guide follows 42 School patterns

**No constitution violations detected**. All implementations remain compliant.

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
# Single project structure (C raytracer)
src/
├── bvh/                     # BVH acceleration structure
│   ├── bvh_build.c         # BVH construction with SAH
│   ├── bvh_traverse.c      # Ray traversal through BVH
│   ├── bvh_rebuild.c       # Selective rebuild for dynamic scenes
│   └── aabb.c              # Axis-aligned bounding box operations
├── optimization/            # Performance optimization code
│   ├── cylinder_intersect.c # Optimized cylinder intersection
│   ├── ray_cull.c          # Ray culling and early termination
│   └── metrics.c           # Performance metrics collection
├── render/                  # Existing rendering code (modified)
│   ├── ray_trace.c         # Modified to use BVH
│   ├── intersect.c         # Modified intersection tests
│   └── scene.c             # Scene loading (builds BVH)
└── ui/                      # User interface (modified)
    ├── input.c             # Add 'I' key handler for metrics
    └── display.c           # Add metrics HUD rendering

includes/
├── bvh.h                   # BVH data structures and API
├── optimization.h          # Optimization utilities
└── miniRT.h               # Main header (extended)

tests/
└── scenes/
    ├── test_complex_scene.rt  # Primary test scene (15 objects)
    └── benchmark_bvh.rt       # BVH stress test (50+ objects)
```

**Structure Decision**: Single project structure is appropriate for this C-based raytracer. New BVH functionality is isolated in src/bvh/ while optimization code goes in src/optimization/. Existing render/ code is modified minimally to integrate BVH traversal. This maintains 42 School project structure conventions while clearly separating new acceleration features.

## Complexity Tracking

No constitution violations - this section intentionally left empty as all constraints are satisfied.

---

## Workflow Execution Summary

**Completed**: 2026-01-01  
**Command**: `/speckit.plan` for feature 001-performance-optimization  
**Branch**: `001-performance-optimization`

### Phase 0: Research ✅ COMPLETE

**Objective**: Resolve all technical unknowns and establish implementation patterns

**Deliverable**: `research.md` (13.4 KB, 7 research tasks completed)

**Key Decisions Made**:
1. **BVH Construction**: Binned SAH with 16 bins per axis, max depth 64
2. **Ray-AABB Intersection**: Slab method with IEEE floating-point arithmetic
3. **BVH Traversal**: Iterative stack-based (64-entry stack)
4. **Selective Rebuild**: Hybrid refit/rebuild based on dirty percentage (<10% refit, >=10% rebuild)
5. **Performance Metrics**: Ring buffer (60 frames), update display every 10 frames
6. **Cylinder Optimization**: Two-phase AABB rejection + infinite cylinder test
7. **Memory Management**: Monitor allocation, 512 MB limit with fallback

**All NEEDS CLARIFICATION items resolved**: ✅

---

### Phase 1: Design & Contracts ✅ COMPLETE

**Objective**: Generate data models, API contracts, and integration guide

**Deliverables**:
- ✅ `data-model.md` (12.1 KB) - 7 core entities defined
  - BVH Node, AABB, Performance Metrics, Ray (extended), Hit Record (extended), BVH Allocator, BVH Build Info
  - All relationships, validation rules, and state transitions documented
  - Memory layout optimization for cache efficiency
  
- ✅ `contracts/bvh_api.md` (14.2 KB) - Complete BVH API specification
  - 9 core functions: build, destroy, intersect, intersect_any, mark_dirty, update, AABB operations
  - Pre/post conditions, complexity analysis, error handling
  - 42 School compliance verified (only malloc, free, math.h, mlx_*)
  - Thread safety documentation (not thread-safe, intentional)
  
- ✅ `contracts/metrics_api.md` (13.6 KB) - Performance metrics API specification
  - 14 functions: init, destroy, frame timing, BVH timing, counters, display control, computed metrics, HUD rendering
  - Ring buffer design, high-resolution timing (clock_gettime)
  - Overhead analysis: <0.01% performance impact
  
- ✅ `quickstart.md` (15.9 KB) - Developer integration guide
  - 5-minute demo, 7-step integration (30-40 minutes)
  - Testing & validation procedures
  - Common issues & troubleshooting (5 issues documented)
  - Performance tuning guidance

**Agent Context Updated**: ✅
- Added C (C99 standard) to language stack
- Added MiniLibX, math.h, libft to framework stack
- Updated `.github/agents/copilot-instructions.md`

---

### Constitution Re-evaluation ✅ PASS

**Post-Phase-1 Status**: All design artifacts reviewed and verified compliant

- ✅ No pthread usage
- ✅ No SIMD/external parallelization libraries
- ✅ Only allowed functions (malloc, free, math.h, mlx_*, libft)
- ✅ All optimizations use allowed techniques (algorithmic, caching, math, memory layout)

---

### Phase 2: Task Breakdown ⏸️ PENDING

**Status**: Not started (requires separate `/speckit.tasks` command)

**Expected Output**: `tasks.md` with atomic implementation tasks

**Note**: Per workflow specification, Phase 2 task generation is handled by a separate command and is NOT part of `/speckit.plan` output.

---

### Generated Artifacts Summary

| File | Size | Purpose |
|------|------|---------|
| `plan.md` | 12.7 KB | This file - implementation plan with technical context |
| `research.md` | 13.4 KB | Phase 0 - resolved technical unknowns |
| `data-model.md` | 12.1 KB | Phase 1 - entity definitions and relationships |
| `contracts/bvh_api.md` | 14.2 KB | Phase 1 - BVH API specification |
| `contracts/metrics_api.md` | 13.6 KB | Phase 1 - metrics API specification |
| `quickstart.md` | 15.9 KB | Phase 1 - developer integration guide |

**Total**: 6 files, 81.9 KB of design documentation

---

### Next Steps

1. **Run `/speckit.tasks`** to generate Phase 2 task breakdown
2. **Begin implementation** following task sequence in `tasks.md`
3. **Test iteratively** using procedures in `quickstart.md`
4. **Update Korean documentation** in `docs/` during implementation
5. **Run CI quality gates** (norminette, build, valgrind) before merge

---

### Success Criteria Met

- ✅ All technical unknowns resolved (no NEEDS CLARIFICATION remaining)
- ✅ Constitution compliance verified (no violations)
- ✅ Data models complete with validation rules
- ✅ API contracts complete with pre/post conditions
- ✅ Integration guide provides clear 30-minute setup path
- ✅ 42 School constraints respected throughout design

**Workflow Status**: Phase 0-1 complete, ready for Phase 2 task generation
