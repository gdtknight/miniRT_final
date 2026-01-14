# Implementation Plan: Complete Compliance Refactoring

**Branch**: `016-compliance-refactoring` | **Date**: 2026-01-14 | **Spec**: [spec.md](./spec.md)  
**Input**: Feature specification from `/specs/016-compliance-refactoring/spec.md`

## Summary

This implementation plan provides a comprehensive, risk-mitigated approach to achieving full 42 School compliance for the miniRT project. The refactoring addresses three critical compliance areas:

1. **Norminette Compliance**: Achieving 100% style compliance across 75 source files through incremental refactoring with automated validation
2. **Forbidden Function Elimination**: Replacing 3 instances of non-allowed functions (memcpy, memset) with custom implementations that maintain identical behavior
3. **Code Deduplication**: Identifying and consolidating files with 100% identical content using hash-based detection

**Technical Approach**: The plan uses an incremental, phase-based approach with continuous validation after each module. A pixel-diff validation tool with <0.1% threshold ensures no visual regressions, while automated memory checks prevent leak introduction. The refactoring is structured in 11 phases (0-10) with clear dependencies, allowing for parallel work where possible and safe rollback procedures at each validation point.

**Key Risk Mitigation**: Baseline renders captured before any changes, frequent automated validation, git branching for instant rollback, and focus on preserving existing functionality while achieving compliance.

## Technical Context

**Language/Version**: C (C99 standard), GCC 7.5+/Clang 10+  
**Primary Dependencies**: MinilibX (graphics), libm (math library), norminette 3.3.50 (style checker), valgrind/leaks (memory validation)  
**Storage**: N/A (refactoring existing code, no new data storage)  
**Testing**: Custom test suite (38 scene files), pixel-diff validation tool (<0.1% threshold), valgrind memory leak detection  
**Target Platform**: Linux (Ubuntu 20.04+) and macOS (10.14+) with X11/AppKit support  
**Project Type**: Single graphics application (miniRT ray tracer)  
**Performance Goals**: Maintain current render times (refactoring should not degrade performance), compilation time increase <20% from baseline  
**Constraints**: 
- Zero tolerance for norminette violations (automatic evaluation failure)
- Zero tolerance for forbidden function usage (evaluation penalty)
- Visual output must remain identical (<0.1% pixel difference)
- Zero memory leaks tolerated (verified by valgrind)
- Must maintain all existing functionality (no breaking changes)

**Scale/Scope**: 
- 75 source files (.c) requiring compliance checks
- ~300+ functions requiring validation
- 3 known forbidden function violations to fix
- 38 test scenes for regression validation
- 11 implementation phases (Phase 0-10)
- Estimated 8-11 days completion time

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Status**: ✅ PASSED - No violations detected

### Project Structure Standards (Principle I)
- ✅ Scripts properly organized: CI scripts in `.github/scripts/`, user scripts at root
- ✅ Documentation hierarchy clear: Active docs in `docs/`, spec in `specs/`
- ✅ No scattered backup files or deprecated content at root level
- ✅ Build artifacts managed via `.gitignore`

### Code Quality Automation (Principle II)
- ✅ Quality gates defined: norminette + build + memory leak check
- ✅ CI enforcement planned: Failed checks will block merges
- ✅ Automated reporting: norminette and valgrind provide clear error messages
- ✅ No manual exceptions: All checks automated

### Documentation and Wiki Synchronization (Principle III)
- ✅ Wiki synchronization not applicable for refactoring (no user-facing changes)
- ✅ Documentation updates planned in Phase 10
- ✅ Source docs in `specs/` directory maintained as source of truth

### Workflow System (Principle IV)
- ✅ Development workflow defined: norminette + build + test per commit
- ✅ PR workflow clear: All checks must pass before merge
- ✅ Release workflow: Not applicable (internal refactoring)
- ✅ Automation principles followed: No manual gates, clear failure messages

### Tools and Environment Standards (Principle V)
- ✅ Platform support maintained: Linux and macOS compatibility preserved
- ✅ Dependency management: MinilibX, norminette, valgrind clearly documented
- ✅ Test infrastructure: 38 scene files systematically organized in `scenes/`
- ✅ Build artifacts: Logs in `logs/`, builds in `build/`, all gitignored

### Bilingual Specification Management (Principle VI)
- ✅ English spec located at `specs/016-compliance-refactoring/spec.md`
- ✅ Korean translation location: `docs/specs/016-compliance-refactoring/` (if applicable)
- ✅ Synchronization planned: Both versions will be updated in Phase 10
- ✅ Release consistency: Both languages will be included in final documentation

### 42 School Function Constraints (Principle VII) - CRITICAL
- ✅ Allowed functions verified: Using only open, close, read, write, printf, malloc, free, math.h, mlx_*
- ✅ pthread MUST NOT be used: Project does not use pthread (verified)
- ✅ fork/pipe MUST NOT be used: Project does not use multiprocessing (verified)
- ✅ External parallelization libraries MUST NOT be used: No SIMD, OpenMP, or TBB (verified)
- ✅ Optimizations within constraints: BVH spatial partitioning, caching, math optimizations only
- ⚠️ **VIOLATIONS TO BE FIXED**: 3 instances of forbidden functions (2x memcpy, 1x memset) - addressed in Phases 3-4

**Re-evaluation After Phase 1 Design**: All principles remain compliant. No additional violations introduced by design decisions.

**Justification for Proceeding**: The only violations (forbidden functions) are explicitly targeted for remediation in this refactoring. All other constitution principles are satisfied.

## Project Structure

### Documentation (this feature)

```text
specs/016-compliance-refactoring/
├── spec.md                          # Feature specification (input)
├── plan.md                          # This file (implementation plan)
├── research.md                      # Phase 0 output (technical research)
├── data-model.md                    # Phase 1 output (compliance metadata model)
├── quickstart.md                    # Phase 1 output (developer onboarding)
├── contracts/                       # Phase 1 output (refactoring contracts)
│   └── refactoring-contracts.md     # Behavior specifications and interfaces
└── tracking/                        # Runtime tracking (generated during execution)
    ├── file_compliance.json         # Per-file compliance state
    ├── function_compliance.json     # Per-function metadata
    ├── duplicates.json              # Duplicate file groups
    ├── forbidden_usage.json         # Forbidden function instances
    ├── validation_results.json      # Pixel-diff and memory validation
    └── phase_state.json             # Phase progress tracking
```

### Source Code (repository root)

```text
miniRT/
├── src/                             # Source files requiring refactoring
│   ├── bvh_vis/                     # BVH visualization (contains memcpy violations)
│   ├── hud/                         # Heads-up display
│   ├── keyguide/                    # Keyboard guide
│   ├── lighting/                    # Lighting calculations
│   ├── main.c                       # Main entry point (contains memset violation)
│   ├── math/                        # Math utilities
│   ├── parser/                      # Scene file parser
│   ├── ray/                         # Ray tracing logic
│   ├── render/                      # Rendering engine
│   ├── spatial/                     # Spatial acceleration (BVH)
│   ├── utils/                       # Utility functions
│   │   └── memory.c                 # NEW: Custom memory utilities (ft_memcpy, ft_memset)
│   └── window/                      # Window management
│
├── includes/                        # Header files
│   ├── minirt.h                     # Main header
│   ├── utils.h                      # NEW: Utility function declarations
│   └── [other headers]              # Module-specific headers
│
├── tests/                           # Testing infrastructure
│   ├── baselines/                   # NEW: Baseline renders for pixel-diff
│   │   ├── *.ppm                    # Rendered images (38 files)
│   │   └── metadata.json            # Baseline generation metadata
│   └── current/                     # NEW: Current renders for comparison
│
├── scenes/                          # Test scene files (38 .rt files)
│   ├── mandatory/                   # Required features
│   ├── edge_cases/                  # Boundary conditions
│   ├── error_cases/                 # Invalid inputs
│   └── bonus/                       # Additional features
│
├── scripts/                         # Validation automation
│   ├── pixel_diff.sh                # NEW: Pixel-diff validation tool
│   └── memory_check.sh              # NEW: Memory leak validation
│
├── build/                           # Build artifacts (gitignored)
├── logs/                            # Build and test logs (gitignored)
└── Makefile                         # Build system (may need updates)
```

**Structure Decision**: 

This refactoring follows the **single project structure** as miniRT is a standalone graphics application. The existing modular organization (bvh_vis/, lighting/, parser/, etc.) is preserved and enhanced through:

1. **New Utility Module** (`src/utils/memory.c`): Centralized location for custom memory functions (ft_memcpy, ft_memset) replacing forbidden functions. This module is reusable across all other modules.

2. **Testing Infrastructure** (`tests/baselines/`, `tests/current/`): New directories for automated pixel-diff validation, ensuring refactored code produces identical visual output.

3. **Validation Scripts** (`scripts/pixel_diff.sh`, `scripts/memory_check.sh`): Automation tools for regression detection and memory leak validation.

4. **Tracking System** (`specs/016-compliance-refactoring/tracking/`): Temporary JSON files for progress monitoring during refactoring, discarded after completion.

The structure maintains clear separation between:
- **Production code** (src/, includes/): Subject to refactoring
- **Testing infrastructure** (tests/, scripts/): Validation tools
- **Documentation** (specs/): Planning and contracts
- **Build artifacts** (build/, logs/): Transient files

No changes to the fundamental architecture—all modifications are surgical improvements to existing modules for compliance, not feature additions.

## Complexity Tracking

**No constitution violations requiring justification.**

This refactoring project maintains simplicity by:
- Using existing single-project structure (no additional projects)
- Adding minimal new code (only custom memory utilities)
- Preserving all existing module boundaries
- Not introducing new architectural patterns
- Following 42 School constraints (no threading, no external libraries)

The refactoring focuses on compliance adjustments to existing code rather than architectural changes, keeping complexity minimal and maintainability high.
