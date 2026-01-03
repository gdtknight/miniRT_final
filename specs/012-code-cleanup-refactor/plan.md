# Implementation Plan: Code Cleanup and Refactoring

**Branch**: `012-code-cleanup-refactor` | **Date**: 2026-01-03 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/012-code-cleanup-refactor/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

This feature focuses on comprehensive code cleanup and refactoring to improve maintainability and comply with 42 norminette standards. The work includes: (1) removing unused functions and unnecessary code, (2) moving all `#define` statements from source files to headers per 42 norminette requirements, (3) reorganizing scattered code into logically grouped header/source pairs by functional domain, and (4) establishing a clear file organization structure. The approach emphasizes preserving all existing functionality while reducing code complexity and improving developer navigation.

## Technical Context

**Language/Version**: C (C99/C11 compatible with 42 norminette standards)  
**Primary Dependencies**: MiniLibX (graphics library), libm (math library), libft (custom C library)  
**Storage**: Scene files (.rt format), N/A for runtime  
**Testing**: Manual testing with scene files, norminette validation, compilation tests  
**Target Platform**: Linux (X11) and macOS (OpenGL/AppKit)  
**Project Type**: Single project (graphics raytracer)  
**Performance Goals**: Real-time rendering for static scenes, BVH optimization support  
**Constraints**: 42 norminette compliance, no multithreading/multiprocessing, allowed functions only (libc, math, mlx, libft, GNL)  
**Scale/Scope**: ~12 header files, ~11 source directories, mixed functional domains across files

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Project Structure Standards** (Principle I):
- ✅ No impact - This is a code organization feature that improves structure
- ✅ Documentation updates will follow `docs/` structure requirements
- ✅ No CI scripts affected, no root-level tool changes

**Code Quality Automation** (Principle II):
- ✅ All changes will pass norminette validation
- ✅ No memory leak introduction - only removing/reorganizing existing code
- ✅ Compilation must succeed after all changes
- ⚠️ CRITICAL: Must validate no functional changes (all scenes render identically)

**Documentation and Wiki Synchronization** (Principle III):
- ✅ No impact - Internal code refactoring only
- ✅ Documentation updates will be made to `docs/` if user-facing changes occur

**Workflow System** (Principle IV):
- ✅ Standard PR workflow applies
- ✅ All automated checks will be enforced

**Tools and Environment Standards** (Principle V):
- ✅ No platform-specific changes
- ✅ Makefile structure remains unchanged
- ✅ Test scenes will be used to validate behavior preservation

**Bilingual Specification Management** (Principle VI):
- ✅ This spec exists in English (`specs/012-code-cleanup-refactor/`)
- ⚠️ MUST create Korean version in `docs/specs/012-code-cleanup-refactor/` (NEEDS CLARIFICATION on timing)

**42 School Constraints Check** (Principle VII):
- ✅ Refactoring will maintain all existing function usage (no new functions introduced)
- ✅ No pthread, fork, or prohibited functions will be introduced
- ✅ Existing BVH optimization uses allowed techniques only
- ✅ Focus is on code organization, not new functionality
- ⚠️ MUST validate no prohibited functions exist in current codebase before proceeding

## Project Structure

### Documentation (this feature)

```text
specs/012-code-cleanup-refactor/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output - Code analysis, domain identification, refactoring strategy
├── data-model.md        # Phase 1 output - File organization model, domain mapping
├── quickstart.md        # Phase 1 output - Guide for maintaining clean code organization
├── contracts/           # Phase 1 output - Norminette compliance contract, function usage contract
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

**Current Structure**:
```text
includes/                    # 12 header files (mixed domains)
├── hud.h                   # HUD/overlay functionality
├── keyguide.h              # Key guide display
├── metrics.h               # Performance metrics
├── minirt.h                # Main program structures
├── objects.h               # Scene objects (sphere, plane, cylinder)
├── parser.h                # Scene file parsing
├── ray.h                   # Ray tracing structures
├── render_state.h          # Rendering state management
├── shadow.h                # Shadow calculations
├── spatial.h               # BVH spatial structures
├── vec3.h                  # Vector math
└── window.h                # Window/MLX management

src/                        # 11 subdirectories
├── hud/                    # HUD rendering (7 files)
├── keyguide/               # Key guide display (3 files)
├── lighting/               # Lighting/shadow (4 files)
├── main.c                  # Entry point
├── math/                   # Vector operations (2 files)
├── parser/                 # Scene parsing (4 files)
├── ray/                    # Ray operations (2 files)
├── render/                 # Rendering engine (6 files)
├── spatial/                # BVH implementation (5 files)
├── utils/                  # Utilities (5 files)
└── window/                 # Window management (2 files)
```

**Identified Issues**:
1. **Scattered definitions**: Functions and structures not grouped by domain
2. **Mixed concerns**: Some headers contain multiple functional domains
3. **Norminette violations**: Need to verify no `#define` in `.c` files
4. **Unused code**: Potential unused functions (NEEDS RESEARCH)
5. **Unclear boundaries**: Some domain boundaries overlap (e.g., rendering vs ray tracing)

**Structure Decision**: Maintain existing single-project structure but reorganize headers/sources by functional domain. Create clear domain boundaries: core (minirt.h), math (vec3.h), rendering (ray.h, render_state.h), objects (objects.h), spatial (spatial.h), parsing (parser.h), lighting (shadow.h), UI (window.h, hud.h, keyguide.h), metrics (metrics.h). Each domain should have one primary header and corresponding source directory.

## Complexity Tracking

> **Constitutional gates passed - no violations requiring justification**

This refactoring maintains the existing single-project structure and complies with all constitutional principles. The work focuses on improving code organization without introducing new complexity, dependencies, or architectural patterns.
