# Implementation Plan: BVH Tree Visualization Improvements

**Branch**: `015-bvh-viz-improvements` | **Date**: 2026-01-12 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/015-bvh-viz-improvements/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Enhance BVH tree visualization debugging experience through unified object identification and debounced keyboard rendering. Implement lowercase object identifiers (sp-1, pl-2, cy-3) stored in each object struct, 1-second keyboard debounce with state machine (IDLE/PENDING/RENDERING), 4-slot timer pool with error handling, 8-capacity input queue, progress bar showing debounce countdown, gettimeofday() timing, and smart input cancellation on mouse/view actions. All implementations follow 42 School constraints (no pthread, no external parallelization).

## Technical Context

**Language/Version**: C99 (42 School project - strict standard compliance)  
**Primary Dependencies**: MiniLibX (42 graphics library), libft (custom C library), math.h  
**Storage**: In-memory scene data structures (no persistent storage)  
**Testing**: Manual testing with test scenes, valgrind memory checks  
**Target Platform**: Linux (minilibx-linux) and macOS (minilibx-macos)
**Project Type**: Single project (graphics application with real-time rendering)  
**Performance Goals**: <5% CPU overhead for debounce operations, ≤2ms HUD render time, smooth 60fps interaction  
**Constraints**: 42 School function constraints (no pthread, no fork, no SIMD), norminette compliance, 25-line function limit  
**Scale/Scope**: Up to 100 objects per scene, BVH tree depth ~10 levels, single-threaded execution

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Project Structure Standards** (Principle I):
- ✅ Feature artifacts stored in `specs/015-bvh-viz-improvements/` per standard structure
- ✅ Source changes will be in `src/` and `includes/` directories (not scattered)
- ✅ No deprecated files introduced; clean implementation

**Code Quality Automation** (Principle II):
- ✅ All changes MUST pass norminette (zero warnings/errors)
- ✅ Build MUST succeed without errors
- ✅ Valgrind MUST report zero memory leaks
- ✅ CI pipeline will enforce all quality gates

**Documentation Synchronization** (Principle III):
- ✅ Implementation plan tracked in `specs/015-bvh-viz-improvements/plan.md`
- ✅ Technical artifacts (research, data-model, contracts, quickstart) will be generated
- ✅ Korean documentation in `docs/specs/015-bvh-viz-improvements/` will be synchronized

**42 School Constraints Check** (Principle VII):
- ✅ All functions MUST be from allowed list (libc, math, mlx_*, libft, get_next_line)
- ✅ Timer implementation uses gettimeofday() (allowed libc function)
- ✅ pthread functions WILL NOT be used
- ✅ fork/pipe/multiprocessing WILL NOT be used  
- ✅ No external parallelization libraries (SIMD, OpenMP, etc.)
- ✅ Optimizations use only allowed techniques:
  - Algorithmic: State machine for debounce, input queue for buffering
  - Caching: Precomputed object identifiers in structs
  - Memory layout: Fixed-size arrays (timer pool, input queue, char id[8])
  - Mathematical: Microsecond timestamp comparisons for timing

**GATE STATUS**: ✅ PASS - No violations, all constraints satisfied

**Post-Phase 1 Re-check** (2026-01-12):
- ✅ Data model uses only fixed arrays (no dynamic allocation)
- ✅ State machine design is algorithmic (no threading/parallelism)
- ✅ All timer operations use gettimeofday() (allowed function)
- ✅ Input queue uses circular buffer (standard algorithm)
- ✅ Progress bar uses MLX primitives (allowed library)
- ✅ Object IDs use snprintf() (allowed libc function)

**Final Constitution Check**: ✅ APPROVED - Ready for Phase 2 implementation

## Project Structure

### Documentation (this feature)

```text
specs/015-bvh-viz-improvements/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
│   ├── object-id-api.md      # Object identifier formatting contracts
│   ├── debounce-timer-api.md # Timer state machine and management contracts
│   └── progress-bar-api.md   # Progress bar rendering contracts
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
includes/
├── objects.h            # ADD: char id[8] field to t_sphere, t_plane, t_cylinder
├── window.h             # ADD: debounce timer state to t_render
├── debounce.h           # NEW: debounce timer API and state machine definitions
└── hud.h                # MODIFY: progress bar rendering function

src/
├── parser/
│   ├── parse_objects.c  # MODIFY: Generate object IDs during parsing
│   └── parse_validation.c # MODIFY: Validate object ID format in scene file
├── debounce/            # NEW: debounce timer implementation module
│   ├── debounce_init.c      # Timer pool initialization
│   ├── debounce_state.c     # State machine transitions
│   ├── debounce_timer.c     # Timer management with gettimeofday()
│   └── debounce_queue.c     # Input queue operations
├── hud/
│   ├── hud_objects.c    # MODIFY: Display object IDs in unified format
│   └── hud_progress.c   # NEW: Progress bar rendering for debounce countdown
├── bvh_vis/
│   └── bvh_vis_node.c   # MODIFY: Display object IDs in leaf nodes
└── window/
    ├── handle_key.c     # MODIFY: Route keyboard inputs through debounce
    └── handle_mouse.c   # MODIFY: Cancel debounce on mouse actions

tests/
└── scenes/
    ├── test_object_ids.rt    # NEW: Scene for testing object ID format
    └── test_debounce.rt      # NEW: Scene for testing debounce behavior
```

**Structure Decision**: Single project structure with new debounce module. Changes are surgical and localized:
- Object ID storage: 3 struct modifications in objects.h
- Debounce system: New module with 4 files (init, state, timer, queue)
- Integration points: Parser (ID generation), HUD (ID display), BVH vis (ID display), window handlers (debounce routing)
- All changes follow existing miniRT patterns (header-based modules, norminette compliance)

## Complexity Tracking

> **No violations - this section is not applicable**

All implementations comply with 42 School constraints and project constitution. No complexity justifications required.
