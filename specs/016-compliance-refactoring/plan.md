# Implementation Plan: miniRT Complete Compliance Refactoring

**Branch**: `016-compliance-refactoring` | **Date**: 2026-01-14 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/016-compliance-refactoring/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

This implementation plan addresses comprehensive compliance refactoring of the miniRT project to meet 42 School evaluation requirements. The primary goals are: (1) achieve 100% norminette 3.3.50 compliance across 75 source files, (2) eliminate all 3 forbidden function usages (memcpy, memset), and (3) remove duplicate source files. The technical approach employs automated incremental refactoring with pixel-diff validation (<0.1% threshold) and memory leak detection to ensure functional preservation throughout the process.

## Technical Context

**Language/Version**: C99 (ISO/IEC 9899:1999 standard)  
**Primary Dependencies**: MinilibX (42 School graphics library), libm (math library), X11/Xext (Linux), AppKit (macOS)  
**Storage**: Scene files (.rt format), PPM image files (baseline validation)  
**Testing**: Valgrind (Linux) / leaks (macOS) for memory validation, norminette 3.3.50 for style checks, custom pixel-diff tool for regression testing  
**Target Platform**: Linux (Ubuntu 20.04+) and macOS (10.14+) with 42 School toolchain  
**Project Type**: Single desktop application (ray tracer)  
**Performance Goals**: Render times <5s for simple scenes, <30s for complex scenes (not primary focus - compliance is priority)  
**Constraints**: 
- Zero norminette violations (automatic evaluation failure otherwise)
- Zero forbidden function usages (automatic evaluation failure)
- Zero memory leaks (evaluation penalty)
- <0.1% pixel difference after refactoring (preserve visual quality)
- Compilation time increase <20% from baseline
**Scale/Scope**: 75 source files, ~300 functions, 38 test scenes, estimated 8-11 days effort

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Constitution Compliance Status**: ✅ PASS

**42 School Constraints Check** (Principle VII):
- ✅ All functions used MUST be from allowed list (libc, math, mlx_*, libft, get_next_line)
  - Status: Currently 3 violations (memcpy x2, memset x1) - will be fixed with custom ft_memcpy/ft_memset in Phase 4
  - Custom implementations use only allowed functions (basic loops, pointer arithmetic)
- ✅ pthread functions MUST NOT be used
  - Status: COMPLIANT - No pthread usage detected in codebase
- ✅ fork/pipe/multiprocessing MUST NOT be used  
  - Status: COMPLIANT - No process management functions used
- ✅ External parallelization libraries (SIMD, OpenMP, etc.) MUST NOT be used
  - Status: COMPLIANT - Only BVH algorithmic optimization used (allowed)
- ✅ Optimizations MUST use only allowed techniques (algorithmic, caching, math, memory layout)
  - Status: COMPLIANT - Current optimizations: BVH spatial partitioning (algorithmic), precomputed normals (caching), cache-friendly data structures (memory layout)

**Other Constitution Principles**:
- ✅ Project Structure Standards (Principle I): Specs in `specs/016-compliance-refactoring/`, tracking in `tracking/`, baselines in `tests/baselines/`
- ✅ Code Quality Automation (Principle II): norminette + build + memory checks automated
- ✅ Documentation Standards: Korean docs in `docs/`, English specs in `specs/`, bilingual synchronization maintained
- ✅ Workflow System (Principle IV): Feature branch `016-compliance-refactoring`, incremental commits, validation gates

**Pre-Phase 0 Gate**: ✅ PASS - All constitution checks satisfied or have clear remediation plan

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
# miniRT project structure (refactoring maintains existing structure)
src/
├── bvh/                    # BVH acceleration structure
├── bvh_vis/                # BVH visualization (contains 2 memcpy violations)
├── camera/                 # Camera and viewport
├── color/                  # Color operations
├── error/                  # Error handling
├── hit/                    # Ray-object intersection
├── light/                  # Lighting calculations
├── main.c                  # Entry point (contains 1 memset violation)
├── material/               # Material properties
├── object/                 # Object definitions (sphere, plane, cylinder)
├── parser/                 # Scene file parsing
├── ray/                    # Ray tracing core
├── render/                 # Rendering pipeline
├── utils/                  # Utilities (will add memory.c for ft_memcpy/ft_memset)
├── vector/                 # Vector mathematics
└── window/                 # MinilibX window management

includes/
├── miniRT.h                # Main header
├── structures.h            # Data structures
├── utils.h                 # Utility function prototypes (will be created/updated)
└── [other module headers]

tests/
├── baselines/              # Baseline renders for pixel-diff validation
│   ├── *.ppm              # Rendered scene images
│   └── metadata.json      # Baseline generation metadata
└── current/               # Current renders for comparison

scenes/
├── mandatory/             # Required feature test scenes
├── edge_cases/            # Boundary condition tests
├── error_cases/           # Invalid input tests
└── bonus/                 # Additional feature tests (if applicable)

specs/016-compliance-refactoring/
├── spec.md                # Feature specification
├── plan.md                # This file
├── research.md            # Phase 0 research output
├── data-model.md          # Phase 1 data model
├── quickstart.md          # Phase 1 developer guide
├── contracts/             # Phase 1 API contracts
└── tracking/              # Refactoring metadata (JSON files)
```

**Structure Decision**: This is a single desktop application project. The existing modular structure (organized by component: bvh, camera, parser, etc.) is well-suited for incremental refactoring. Each module can be validated independently. New additions: `src/utils/memory.c` for custom memory utilities, `tests/baselines/` for regression testing, `specs/016-compliance-refactoring/tracking/` for refactoring metadata.

## Complexity Tracking

> **Not Applicable** - No constitution violations exist that require justification.

This refactoring project complies with all constitution principles:
- Uses only allowed functions (custom implementations for forbidden ones)
- No additional projects/repositories needed
- No architecture patterns that violate simplicity principles
- Follows established 42 School constraints and project structure standards

All identified violations (3 forbidden function usages) will be resolved through compliant custom implementations (ft_memcpy, ft_memset) as documented in research.md and contracts/refactoring-contracts.md.

---

## Phase Completion Summary

### ✅ Phase 0: Research & Analysis (COMPLETED)

**Status**: Complete  
**Output**: `research.md`  
**Date**: 2026-01-14

**Key Achievements**:
- Resolved all technical unknowns from specification
- Established norminette compliance strategy (incremental refactoring)
- Designed custom memory utilities (ft_memcpy, ft_memset) for forbidden function replacement
- Defined hash-based duplicate detection approach (100% match only)
- Created automated validation framework (pixel-diff <0.1% + memory checks)
- Identified risk mitigation strategies and rollback procedures
- Mapped 10-phase implementation workflow with dependency graph

**Artifacts Generated**:
- `specs/016-compliance-refactoring/research.md` (630 lines)
- Implementation patterns for norminette violations
- Custom memory utility specifications
- Validation tool designs

---

### ✅ Phase 1: Design & Contracts (COMPLETED)

**Status**: Complete  
**Output**: `data-model.md`, `quickstart.md`, `contracts/refactoring-contracts.md`  
**Date**: 2026-01-14

**Key Achievements**:
- Defined 6 core entities for tracking refactoring state
- Created comprehensive data model for compliance metadata
- Generated developer quickstart guide with 10-phase workflow
- Documented API contracts for custom memory utilities
- Updated GitHub Copilot agent context with C99 and MinilibX information
- Validated constitution compliance (all gates passed)

**Artifacts Generated**:
- `specs/016-compliance-refactoring/data-model.md` (334 lines)
- `specs/016-compliance-refactoring/quickstart.md` (745 lines)
- `specs/016-compliance-refactoring/contracts/refactoring-contracts.md` (exists)
- `.github/agents/copilot-instructions.md` (updated)

**Data Model Entities**:
1. Source File Metadata (t_file_compliance)
2. Function Metadata (t_function_compliance)
3. Duplicate File Group (t_duplicate_group)
4. Forbidden Function Usage (t_forbidden_usage)
5. Validation Result (t_validation_result)
6. Refactoring Phase State (t_phase_state)

**Constitution Re-check**: ✅ PASS
- No new violations introduced during design phase
- Custom memory utilities comply with allowed functions (basic loops only)
- All optimizations remain within allowed techniques
- Project structure follows established standards

---

## Next Steps (Phase 2)

**Command**: `/speckit.tasks` or equivalent task generation workflow

This planning phase (Phases 0-1) is now complete. The next phase involves:

1. **Task Breakdown**: Generate `tasks.md` from implementation plan
2. **Implementation**: Execute Phases 0-10 as documented in quickstart.md
3. **Validation**: Run quality gates after each phase
4. **Documentation**: Update docs/ with final results

**Ready for Implementation**: ✅ YES

All prerequisites satisfied:
- ✅ Technical context defined
- ✅ Constitution compliance verified
- ✅ Research completed (all unknowns resolved)
- ✅ Data model designed
- ✅ API contracts specified
- ✅ Developer guide created
- ✅ Agent context updated

**Branch**: `016-compliance-refactoring` (ready to begin implementation work)

---

## Quick Reference

| Document | Purpose | Status |
|----------|---------|--------|
| `spec.md` | Feature specification (user stories, requirements) | ✅ Complete |
| `plan.md` | Implementation plan (this file) | ✅ Complete |
| `research.md` | Phase 0: Technical research and decisions | ✅ Complete |
| `data-model.md` | Phase 1: Entity definitions and relationships | ✅ Complete |
| `quickstart.md` | Phase 1: Developer onboarding guide | ✅ Complete |
| `contracts/refactoring-contracts.md` | Phase 1: API contracts and specifications | ✅ Complete |
| `tasks.md` | Phase 2: Executable task breakdown | ⏳ Not Started |

**Estimated Timeline**: 8-11 days for full implementation (Phases 0-10)  
**Start Date**: TBD (after task generation)  
**Team**: Individual developer (42 School project)

---

*End of Implementation Plan*
