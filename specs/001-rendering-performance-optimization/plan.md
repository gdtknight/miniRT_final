# Implementation Plan: Advanced Rendering Performance Optimization

**Branch**: `001-rendering-performance-optimization` | **Date**: 2025-12-19 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/001-rendering-performance-optimization/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

This implementation plan addresses the critical performance bottleneck in miniRT ray tracer rendering. The primary requirement is to achieve interactive frame rates (5+ FPS) for complex scenes with 50+ objects while maintaining camera movement response time under 100ms. The technical approach combines spatial acceleration structures (BVH), bounding volume culling, adaptive sampling, and cache-friendly memory layouts—all implemented within strict 42 School constraints (no pthread, no external libraries, single-threaded execution).

## Technical Context

**Language/Version**: C (C99/C11 standard, gcc/clang compatible)
**Primary Dependencies**: 
- libc (malloc, free, memcpy, standard I/O)
- libm (sqrt, sin, cos, fabs, pow - linked with -lm)
- MiniLibX (mlx_* functions for graphics output)
- libft (42 custom C library)
- get_next_line (line reading utility)

**Storage**: Scene files (.rt format) loaded at startup, in-memory representation only
**Testing**: Manual testing with provided test scenes, visual validation, performance profiling with scene complexity
**Target Platform**: Linux (Ubuntu/Debian on 42 School evaluation systems), x86-64 architecture
**Project Type**: Single project (graphics application)
**Performance Goals**: 
- Camera movement response: <100ms (P1)
- Object manipulation feedback: <200ms (P2)
- Complex scene (50+ objects) preview: 5+ FPS minimum (P3)
- Ray-object intersection reduction: 80% for complex scenes (P4)

**Constraints**: 
- **42 School Function Restrictions**: pthread prohibited, fork/pipe prohibited, no external parallelization libraries
- **Single-threaded execution**: All optimizations algorithmic, no multithreading/multiprocessing
- **Visual quality preservation**: Final renders must be pixel-perfect identical to naive implementation
- **Memory budget**: <50MB additional overhead for acceleration structures (100 objects)
- **Stack limit awareness**: Avoid deep recursion in BVH traversal

**Scale/Scope**: 
- Target scene complexity: 10-50 objects (typical educational use)
- Extreme test cases: up to 100 objects
- Resolution: 800x600 pixels (current hardcoded standard)
- Optimization structures: BVH tree, bounding boxes, intersection cache

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Principle I - Project Structure Standards**:
- ✅ Implementation in `src/optimization/` maintains clear organization
- ✅ No CI scripts scattered in source tree
- ✅ Documentation organized in `specs/001-rendering-performance-optimization/`
- ✅ Test scenes organized in `scenes/optimization/`

**Principle II - Code Quality Automation**:
- ✅ All code must pass norminette (42 coding standard)
- ✅ Valgrind memory leak checks must pass with zero leaks
- ✅ Build must succeed without warnings
- ✅ CI pipeline enforces quality gates automatically

**Principle VII - 42 School Function Constraints** (CRITICAL):
- ✅ **Allowed functions only**: libc, math.h, mlx_*, libft, get_next_line
- ✅ **pthread PROHIBITED**: No multithreading - single-threaded BVH traversal only
- ✅ **fork/pipe PROHIBITED**: No multiprocessing - all optimization in single process
- ✅ **External libraries PROHIBITED**: No SIMD (SSE/AVX), no OpenMP, no Intel TBB
- ✅ **Allowed optimizations**: 
  - Algorithmic: BVH, spatial partitioning, octree acceptable
  - Early termination: Bounding box tests, frustum culling acceptable
  - Caching: Precomputed bounds, lookup tables acceptable
  - Memory layout: Structure packing, cache-friendly arrays acceptable
  - Math: Reduced precision, algebraic simplification acceptable

**Quality Assurance**:
- ✅ Visual regression: Final renders must be pixel-perfect identical
- ✅ Edge case handling: Graceful degradation on allocation failure
- ✅ Performance validation: Measurable metrics for optimization effectiveness

**GATE STATUS**: ✅ PASSED - All optimizations use allowed techniques within 42 constraints

## Project Structure

### Documentation (this feature)

```text
specs/001-rendering-performance-optimization/
├── plan.md              # This file (implementation plan)
├── research.md          # Phase 0: BVH algorithms, optimization techniques
├── data-model.md        # Phase 1: Acceleration structure data models
├── quickstart.md        # Phase 1: Integration guide for developers
├── contracts/           # Phase 1: API contracts for optimization modules
│   ├── bvh-api.md      # BVH construction and traversal interface
│   ├── bbox-api.md     # Bounding box calculation interface
│   └── cache-api.md    # Intersection cache interface
└── tasks.md             # Phase 2: Detailed implementation tasks (not created by plan)
```

### Source Code (repository root)

```text
src/
├── optimization/             # NEW: Performance optimization modules
│   ├── bvh.c                # BVH construction (build_bvh, free_bvh)
│   ├── bvh_traverse.c       # BVH ray traversal (traverse_bvh)
│   ├── bbox.c               # Bounding box calculations (compute_bbox, bbox_intersect)
│   ├── spatial_sort.c       # Object sorting for BVH (sort_by_axis)
│   └── intersection_cache.c # Ray-object intersection cache (cache_get, cache_set)
│
├── render/
│   ├── render.c             # MODIFY: Integrate BVH traversal
│   ├── trace.c              # MODIFY: Use cached intersections
│   └── camera.c             # Existing camera ray generation
│
├── ray/
│   └── intersections.c      # MODIFY: Add bounding box early-out tests
│
├── math/
│   ├── vector.c             # Existing vector operations
│   └── bbox_math.c          # NEW: Bounding box math helpers
│
└── [other existing modules unchanged]

includes/
├── optimization.h           # NEW: BVH, bbox, cache public interfaces
├── bvh_internal.h          # NEW: BVH implementation details
└── [other existing headers]

tests/
├── unit/
│   ├── test_bvh.c          # BVH construction and traversal tests
│   ├── test_bbox.c         # Bounding box calculation tests
│   └── test_cache.c        # Intersection cache tests
│
└── performance/
    ├── benchmark_scenes/    # Scenes with varying complexity (5, 10, 20, 50, 100 objects)
    └── profile_render.c     # Performance measurement utility

scenes/
└── optimization/            # NEW: Test scenes for optimization validation
    ├── simple_5.rt         # 5 objects baseline
    ├── medium_20.rt        # 20 objects typical case
    ├── complex_50.rt       # 50 objects target
    └── extreme_100.rt      # 100 objects stress test
```

**Structure Decision**: Single project structure with new `src/optimization/` module for acceleration structures. Modifications to existing render pipeline integrate BVH traversal without disrupting current architecture. Clear separation between optimization logic (new modules) and rendering logic (modified modules) enables incremental testing and rollback capability.

## Complexity Tracking

> **No violations requiring justification** - All implementation choices comply with 42 School constraints and constitution principles.

This section is empty because the implementation uses only allowed optimization techniques within single-threaded execution. All complexity is inherent to the algorithmic optimization domain (BVH construction, spatial partitioning) rather than architectural violations.

---

## Phase 0: Research & Technical Decisions

### Research Tasks

The following unknowns from Technical Context require research before design:

1. **BVH vs Octree Selection**
   - **Question**: Which spatial acceleration structure provides best single-threaded performance within 42 constraints?
   - **Research needed**: Algorithm complexity analysis, memory overhead comparison, implementation complexity
   - **Decision criteria**: Construction time, traversal efficiency, memory footprint, code maintainability

2. **BVH Construction Strategy**
   - **Question**: Top-down vs bottom-up, axis selection heuristic, splitting strategy
   - **Research needed**: SAH (Surface Area Heuristic) vs median split, recursive depth limits
   - **Decision criteria**: Build time vs query performance tradeoff

3. **Bounding Volume Type**
   - **Question**: Axis-Aligned Bounding Box (AABB) vs Bounding Sphere
   - **Research needed**: Intersection test performance, memory size, tightness of fit
   - **Decision criteria**: Ray-box vs ray-sphere intersection cost, false positive rate

4. **Cache Strategy**
   - **Question**: When and what to cache during frame rendering
   - **Research needed**: Cache invalidation strategy, hit rate prediction, memory vs speed tradeoff
   - **Decision criteria**: Measurable performance gain, memory overhead

5. **Integration Points**
   - **Question**: Where in existing render pipeline to insert BVH traversal
   - **Research needed**: Current ray tracing code flow analysis
   - **Decision criteria**: Minimal code disruption, rollback capability, testability

### Research Methodology

For each task:
1. Study existing miniRT codebase (render.c, intersections.c, trace.c)
2. Review BVH/octree literature and reference implementations
3. Prototype minimal test cases with 5-10 objects
4. Measure actual performance on target hardware
5. Document decision with rationale and alternatives considered

**Output**: `research.md` containing all decisions with evidence-based rationale

---

## Phase 1: Design & API Contracts

### Data Model Design

**Entity Extraction from Requirements**:

1. **BVH Node** (FR-005, FR-007)
   - Represents single node in binary hierarchy
   - Contains: bounding box, object references OR child node pointers
   - Leaf nodes: 1-4 objects (tunable threshold)
   - Interior nodes: 2 children (left/right)

2. **Bounding Box** (FR-006, FR-010)
   - Axis-aligned box defined by min/max corners
   - Used for: fast ray intersection test, spatial containment
   - Properties: volume, surface area (for SAH)

3. **Spatial Metadata** (FR-018, FR-020)
   - Precomputed object bounds
   - Static object data: centers, radii, axis-aligned extents
   - Memory layout: contiguous array for cache efficiency

4. **Intersection Cache Entry** (FR-011)
   - Key: ray direction hash + object ID
   - Value: hit distance, normal, color
   - Scope: single frame (invalidated on camera movement)

5. **Render Quality Level** (FR-014, FR-016)
   - Enum: PREVIEW (low quality, fast) vs FINAL (high quality)
   - Controls: sample density, BVH traversal depth limit, tile priority

### API Contract Generation

Contracts defined in `contracts/` directory:

**bvh-api.md**: BVH lifecycle and traversal
- `build_bvh(scene, settings) -> bvh_root`
- `traverse_bvh(bvh_root, ray, hit) -> bool`
- `free_bvh(bvh_root) -> void`

**bbox-api.md**: Bounding volume operations
- `compute_bbox_sphere(sphere) -> bbox`
- `compute_bbox_plane(plane, scene_bounds) -> bbox`
- `compute_bbox_cylinder(cylinder) -> bbox`
- `bbox_intersect(bbox, ray) -> (bool, t_near, t_far)`

**cache-api.md**: Intersection caching
- `cache_init(capacity) -> cache`
- `cache_get(cache, ray_hash, object_id) -> hit_result?`
- `cache_set(cache, ray_hash, object_id, hit_result) -> void`
- `cache_clear(cache) -> void`

### Integration Design

**Quickstart Guide** (`quickstart.md`):
- How to enable/disable optimizations via compile flags
- Performance measurement methodology
- Troubleshooting common issues
- Scene file recommendations for testing

### Agent Context Update

Run agent context update script after generating contracts:
```bash
.specify/scripts/bash/update-agent-context.sh copilot
```

This updates `.github/copilot-instructions.md` with:
- New BVH/bbox/cache API patterns
- Optimization module coding conventions
- Performance testing expectations

**Output**: `data-model.md`, `contracts/*.md`, `quickstart.md`, updated agent context

---

## Phase 2: Task Breakdown (NOT DONE BY THIS COMMAND)

Phase 2 task breakdown is created by the separate `/speckit.tasks` command. This command ends after Phase 1 completion.

The task breakdown will include:
- BVH construction implementation tasks
- Ray traversal integration tasks
- Bounding box calculation tasks
- Performance testing and validation tasks
- Documentation and scene creation tasks

---

## Post-Design Constitution Re-check

After completing Phase 1 design, re-verify constitution compliance:

**Principle VII - 42 School Constraints**:
- ✅ BVH construction uses only malloc/free (no external allocators)
- ✅ Ray traversal is iterative with bounded stack (no pthread)
- ✅ Math operations use libm only (sqrt, fabs, fmin, fmax)
- ✅ No SIMD intrinsics in bounding box intersection tests

**Quality Assurance**:
- ✅ BVH construction is deterministic (same scene → same tree)
- ✅ Traversal produces identical visual results (pixel-perfect)
- ✅ Memory usage tracked and bounded (<50MB for 100 objects)
- ✅ Graceful degradation on malloc failure (fallback to linear scan)

**FINAL GATE STATUS**: ✅ PASSED - Design complies with all constraints

---

## Success Criteria Validation Plan

Post-implementation, validate against spec success criteria:

**Performance Metrics** (SC-001 to SC-004):
- Camera movement latency measurement: timestamp key press → pixel change
- FPS measurement during continuous camera rotation
- Complex scene (50 objects) frame time profiling
- Final render completion timer after interaction stop

**Optimization Effectiveness** (SC-005 to SC-008):
- Intersection test counter: before/after BVH comparison
- Render time scaling: 10 objects vs 20 vs 50 vs 100
- Cache miss rate profiling via instrumentation
- Preview vs final render time ratio measurement

**User Experience** (SC-009 to SC-012):
- Subjective smoothness evaluation by multiple users
- Freeze detection: max frame time never exceeds 500ms
- Composition time measurement: position object → satisfied

**Quality Assurance** (SC-013 to SC-016):
- Visual regression: pixel diff between optimized and naive render
- Edge case scene rendering: 1 object, 100 objects, overlapping geometry
- Memory profiler: valgrind massif heap analysis
- norminette compliance verification in CI
