# Implementation Plan: BVH Tree Visualization

**Branch**: `014-bvh-tree-visualization` | **Date**: 2026-01-12 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/014-bvh-tree-visualization/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Implement terminal-based visualization of the BVH tree structure using ASCII tree characters (├──, └──, │) to display hierarchical relationships, bounding box coordinates, depth levels, and object references. The feature is disabled by default and activated via --bvh-vis flag to ensure zero performance overhead during normal operation. Visualization displays after BVH construction completes but before rendering begins, with performance overhead target of <1% of construction time.

## Technical Context

**Language/Version**: C (C99 standard), compiled with gcc/clang with -Wall -Wextra -Werror  
**Primary Dependencies**: Standard C library (stdio.h, stdlib.h), math.h (-lm), MiniLibX graphics library, libft (custom C library), get_next_line  
**Storage**: N/A (terminal output only, no persistence required)  
**Testing**: Manual testing with test scenes in scenes/ directory, validation via test_miniRT.sh script  
**Target Platform**: Linux (primary), macOS (secondary) - 42 School evaluation environment
**Project Type**: Single binary executable (ray tracer with spatial acceleration)  
**Performance Goals**: Visualization overhead <1% of BVH construction time, display output within 5 seconds for scenes with up to 100 objects  
**Constraints**: Must use only 42 School allowed functions (no pthread, no fork, no external parallelization), terminal width 80-120 characters, 2 decimal precision for coordinates  
**Scale/Scope**: Support scenes with 1-100 objects, BVH trees up to 20+ levels deep, leaf nodes with 1-10 objects each

**Existing BVH Implementation**:
- BVH node structure defined in includes/spatial.h with fields: bounds (t_aabb), left/right pointers, objects array, object_count
- Current BVH node does NOT store depth - needs to be added as integer field
- BVH tree structure stores: root node, enabled flag, total_nodes, max_depth
- BVH construction in src/spatial/bvh_build_*.c files
- BVH traversal in src/spatial/bvh_traverse.c
- Command-line parsing exists in src/parser/ directory

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Phase 0 Check (Before Research)**: ✅ PASSED  
**Phase 1 Check (After Design)**: ✅ PASSED

**Project Structure Standards** (Principle I):
- ✅ User-facing scripts remain at project root
- ✅ Documentation organized in docs/ (Korean) and specs/ (English) hierarchy
- ✅ Active files have clear purpose and location
- ✅ Build artifacts in .gitignore
- ✅ Design creates new src/bvh_vis/ module following existing structure patterns

**Code Quality Automation** (Principle II):
- ✅ All code changes must pass norminette check
- ✅ Build must succeed without errors
- ✅ Memory leak check with valgrind required
- ✅ Quality gates enforced in CI/CD
- ✅ Prefix buffer management designed with explicit malloc/free tracking

**Documentation and Wiki Synchronization** (Principle III):
- ✅ Source documentation in specs/ is source of truth
- ✅ Wiki synchronization on release tags
- ✅ Korean documentation in docs/specs/ mirrors English specs/
- ✅ Complete documentation generated: research.md, data-model.md, contracts/, quickstart.md

**Workflow System** (Principle IV):
- ✅ Feature development follows structured workflow
- ✅ Atomic commits with clear descriptions
- ✅ All checks automated
- ✅ Design follows plan → research → design → contracts workflow

**Tools and Environment Standards** (Principle V):
- ✅ Linux and macOS support required
- ✅ Test scenes organized in scenes/ directory
- ✅ Build artifacts in build/ and logs/ directories
- ✅ Design uses POSIX-compliant ioctl (works on Linux and macOS)

**Bilingual Specification Management** (Principle VI):
- ✅ English specification in specs/014-bvh-tree-visualization/
- ⚠️  Korean translation in docs/specs/014-bvh-tree-visualization/ (to be created in parallel during implementation)
- ✅ Both versions will be updated in same commit/PR
- ✅ Design documentation created in English, Korean translation follows same structure

**42 School Constraints Check** (Principle VII):
- ✅ All functions from allowed list only (libc, math, mlx_*, libft, get_next_line)
  - Confirmed in design: printf/fprintf, malloc/free, ioctl, strcmp, fabs
- ✅ NO pthread functions (design uses single-threaded traversal)
- ✅ NO fork/pipe/multiprocessing (design is pure single-process output)
- ✅ NO external parallelization libraries (pure C standard library implementation)
- ✅ Optimizations use allowed techniques only:
  - Algorithmic: O(n) tree traversal, single-pass statistics
  - Caching: Depth stored in node (eliminates recalculation)
  - Memory layout: Dynamic prefix buffer with minimal allocations

**GATE STATUS**: ✅ PASS - All constitution principles satisfied after design phase. Design artifacts complete: research.md, data-model.md, contracts/, quickstart.md. Agent context updated. Ready for Phase 2 task breakdown.

## Project Structure

### Documentation (this feature)

```text
specs/014-bvh-tree-visualization/
├── spec.md              # Feature specification (already exists)
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output - research decisions
├── data-model.md        # Phase 1 output - data structures
├── quickstart.md        # Phase 1 output - usage guide
└── contracts/           # Phase 1 output - API/interface contracts
    └── visualization_api.md
```

### Source Code (repository root)

```text
src/
├── bvh_vis/                    # NEW: BVH visualization module
│   ├── bvh_vis_init.c         # Initialization and configuration
│   ├── bvh_vis_tree.c         # Tree traversal and rendering
│   ├── bvh_vis_node.c         # Individual node formatting
│   ├── bvh_vis_stats.c        # Statistics calculation and display
│   ├── bvh_vis_prefix.c       # Prefix management for tree drawing
│   └── bvh_vis_print.c        # Output primitives and formatting
├── spatial/                    # EXISTING: BVH data structures
│   ├── bvh_build_core.c       # MODIFIED: Add depth tracking
│   ├── bvh_build_partition.c
│   ├── bvh_build_split.c      # MODIFIED: Pass depth to children
│   ├── bvh_init.c
│   ├── bvh_lifecycle.c
│   └── bvh_traverse.c
├── parser/                     # EXISTING: Command-line parsing
│   └── (existing files)       # MODIFIED: Add --bvh-vis flag parsing
└── main.c                      # MODIFIED: Call visualization after BVH build

includes/
├── bvh_vis.h                   # NEW: Visualization API header
└── spatial.h                   # MODIFIED: Add depth field to t_bvh_node

tests/
└── bvh_vis/                    # NEW: Visualization tests
    ├── test_small_scene.c      # Test with 3-5 objects
    ├── test_medium_scene.c     # Test with 10-20 objects
    └── test_edge_cases.c       # Deep trees, empty scenes, etc.
```

**Structure Decision**: Single project structure with new src/bvh_vis/ module for maximum granularity (5+ files as specified in spec clarifications). Visualization module is cleanly separated from existing BVH spatial acceleration code, with minimal modifications to existing files (only adding depth field and calling visualization function). All visualization logic is self-contained in the new module.

## Complexity Tracking

> No constitution violations detected. This section is empty as all principles are satisfied.

---

## Planning Workflow Completion Report

**Status**: ✅ COMPLETE  
**Date Completed**: 2026-01-12  
**Workflow Version**: speckit.plan (Phase 0-1)

### Phase 0: Research & Outline ✅

**Objective**: Resolve all technical unknowns and document research decisions

**Deliverables**:
- ✅ `research.md` (302 lines) - Complete with 8 research questions resolved
  - ASCII tree drawing implementation strategy
  - Depth field storage approach
  - Coordinate formatting decisions
  - Terminal width detection method
  - Object ID display format
  - Statistics calculation algorithm
  - Warning message strategy
  - Command-line flag integration

**Key Decisions**:
- Store depth in node structure during construction (eliminates recalculation overhead)
- Use dynamic prefix buffer with stack-based management (O(d) space, minimal allocations)
- POSIX ioctl for terminal width detection (Linux + macOS compatible)
- Format coordinates to 2 decimal places with scientific notation for extremes

### Phase 1: Design & Contracts ✅

**Objective**: Define data models, API contracts, and usage documentation

**Deliverables**:
- ✅ `data-model.md` (497 lines) - Complete entity definitions and relationships
  - Modified t_bvh_node with depth field
  - Modified t_bvh with visualize flag
  - New t_vis_config structure
  - New t_prefix_state structure
  - New t_bvh_stats structure
  - New t_node_info structure
  - All invariants and validation rules defined

- ✅ `contracts/visualization_api.md` (542 lines) - Complete API specification
  - 4 public API functions documented
  - 11 internal module functions specified
  - Data type contracts with invariants
  - Output format specification
  - Error codes and messages
  - Performance guarantees
  - Integration points defined

- ✅ `quickstart.md` (395 lines) - Complete user guide
  - Quick start in 30 seconds
  - Usage examples and scenarios
  - Output interpretation guide
  - Troubleshooting section
  - Performance impact analysis
  - FAQ and support information

- ✅ Agent context updated (GitHub Copilot instructions)
  - Added C99 standard with gcc/clang flags
  - Added standard C library and dependencies
  - Added project type information

**Architecture Decisions**:
- Single project structure with new src/bvh_vis/ module (6 files)
- Maximum granularity: bvh_vis_init.c, bvh_vis_tree.c, bvh_vis_node.c, bvh_vis_stats.c, bvh_vis_prefix.c, bvh_vis_print.c
- Minimal modifications to existing BVH code (only depth field addition)
- Clean separation between visualization and core BVH functionality

### Phase 2: Implementation Planning ✅

**Objective**: Command ends after Phase 1 design artifacts generation

**Status**: ✅ COMPLETE - Per workflow specification, `/speckit.plan` command stops after Phase 1
- Phase 2 task breakdown will be handled by separate `/speckit.tasks` command
- Implementation plan provides complete foundation for task generation

### Constitution Compliance

**Final Verification**: ✅ ALL PRINCIPLES SATISFIED

- ✅ Project Structure Standards: New module follows existing patterns
- ✅ Code Quality Automation: Design supports norminette, valgrind, build checks
- ✅ Documentation Synchronization: All docs generated in specs/, Korean translation pending
- ✅ Workflow System: Followed structured plan → research → design workflow
- ✅ Tools and Environment: POSIX-compliant, Linux + macOS support
- ✅ Bilingual Management: English docs complete, Korean translation structure defined
- ✅ 42 School Constraints: Only allowed functions used, no prohibited techniques

### Artifacts Generated

```
specs/014-bvh-tree-visualization/
├── spec.md                          # (Pre-existing) Feature specification
├── plan.md                          # (Generated) This implementation plan
├── research.md                      # (Generated) Research decisions - 302 lines
├── data-model.md                    # (Generated) Entity definitions - 497 lines
├── quickstart.md                    # (Generated) User guide - 395 lines
└── contracts/
    └── visualization_api.md         # (Generated) API specification - 542 lines

Total documentation: 1,736 lines of design artifacts
```

### Next Steps

1. **Korean Documentation** (Parallel with implementation):
   - Create docs/specs/014-bvh-tree-visualization/ directory
   - Translate all generated documents to Korean
   - Maintain semantic equivalence with English versions

2. **Task Breakdown** (Use `/speckit.tasks` command):
   - Generate tasks.md from this plan and design artifacts
   - Break implementation into atomic, testable tasks
   - Define task dependencies and acceptance criteria

3. **Implementation** (After task breakdown):
   - Follow tasks.md implementation order
   - Test each task independently
   - Commit atomically with clear messages

4. **Validation** (Throughout implementation):
   - Run norminette on all new C files
   - Verify no memory leaks with valgrind
   - Test with various scene configurations
   - Validate against acceptance criteria in spec.md

### Success Metrics

**Planning Completeness**: ✅ 100%
- All technical unknowns resolved
- Complete data model defined
- Full API contract specified
- User documentation provided
- Constitution compliance verified

**Documentation Quality**: ✅ High
- 1,736 lines of detailed design documentation
- Clear examples and usage scenarios
- Comprehensive troubleshooting guidance
- Full API reference with contracts

**Readiness for Implementation**: ✅ Ready
- No blocking unknowns or clarifications needed
- Clear implementation path defined
- All dependencies identified
- Performance targets specified

---

**Planning Phase Complete** - Ready for `/speckit.tasks` command to generate task breakdown.
