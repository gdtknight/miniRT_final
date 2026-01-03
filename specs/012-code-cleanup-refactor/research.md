# Research: Code Cleanup and Refactoring

**Feature**: 012-code-cleanup-refactor  
**Date**: 2026-01-03  
**Status**: Complete

## Overview

This document consolidates research findings to resolve all NEEDS CLARIFICATION items from the implementation plan's Technical Context and Constitution Check sections. The research focuses on identifying unused code, validating 42 norminette compliance, establishing functional domain boundaries, and determining the optimal refactoring strategy.

## Research Tasks Completed

### 1. Current Codebase Analysis

**Task**: Analyze current codebase structure to identify unused functions, define placement, and organizational issues.

**Findings**:
- **Total codebase size**: 5,629 lines of code across all `.c` and `.h` files
- **Defined functions**: 164 global functions (extracted via `nm -g` from compiled binary)
- **Directory structure**: 11 source subdirectories, 12 header files
- **Define statements in source files**: 0 (already compliant with 42 norminette)

**Key Observations**:
1. **Good news on norminette**: No `#define` statements exist in `.c` files - already compliant
2. **Modular organization**: Code is already organized into logical subdirectories (hud, keyguide, lighting, math, parser, ray, render, spatial, utils, window)
3. **Header organization**: 12 headers roughly correspond to source directories but may have scope overlap
4. **Function usage**: With 164 exported functions, need detailed analysis to identify truly unused functions

**Decision**: Focus refactoring on three areas:
1. **Unused code removal**: Identify and remove unused functions through call graph analysis
2. **Header consolidation**: Ensure headers match source directory domains (1:1 mapping where possible)
3. **Code clarity**: Remove unnecessary statements, improve organization within existing structure

**Rationale**: Since norminette compliance is already achieved, effort should focus on removing dead code and improving logical organization rather than structural migration.

**Alternatives Considered**: 
- Complete restructuring with new directory hierarchy - Rejected: Current structure is reasonable, requires less risk
- Tool-based automated refactoring - Rejected: C refactoring tools are limited, manual review safer for this codebase size

---

### 2. Functional Domain Identification

**Task**: Identify clear functional domains for code organization based on miniRT architecture.

**Findings**:

**Primary Domains Identified**:

1. **Core Domain** (`minirt.h` + `main.c`)
   - Program entry point, main data structures, application lifecycle
   - Dependencies: All other domains

2. **Math Domain** (`vec3.h` + `src/math/`)
   - Vector operations (add, subtract, dot, cross, normalize, etc.)
   - Pure mathematical functions with no external dependencies
   - Dependencies: None (foundational)

3. **Object Domain** (`objects.h` + object intersection code)
   - Scene object definitions (sphere, plane, cylinder)
   - Object-specific intersection algorithms
   - Dependencies: Math domain

4. **Ray Domain** (`ray.h` + `src/ray/`)
   - Ray structure and basic ray operations
   - Ray generation and transformation
   - Dependencies: Math domain

5. **Spatial Domain** (`spatial.h` + `src/spatial/`)
   - BVH (Bounding Volume Hierarchy) implementation
   - Spatial acceleration structures
   - Dependencies: Math, Object, Ray domains

6. **Lighting Domain** (`shadow.h` + `src/lighting/`)
   - Light calculations, shadow casting, attenuation
   - Dependencies: Ray, Object, Math domains

7. **Rendering Domain** (`render_state.h` + `src/render/`)
   - Main rendering loop, trace logic, camera
   - Integration point for all rendering subsystems
   - Dependencies: Ray, Object, Spatial, Lighting domains

8. **Parser Domain** (`parser.h` + `src/parser/`)
   - Scene file parsing (.rt format)
   - Object and scene construction from text
   - Dependencies: Object, Math domains

9. **Window Domain** (`window.h` + `src/window/`)
   - MLX integration, window lifecycle, event handling
   - Dependencies: Rendering domain

10. **HUD Domain** (`hud.h` + `src/hud/`)
    - Performance overlay display
    - Dependencies: Window, Metrics domains

11. **KeyGuide Domain** (`keyguide.h` + `src/keyguide/`)
    - Interactive key guide display
    - Dependencies: Window domain

12. **Metrics Domain** (`metrics.h` + `src/render/metrics*.c`)
    - Performance tracking and statistics
    - Dependencies: Rendering domain

**Domain Dependency Graph**:
```
Math (foundational)
 ├─> Object
 ├─> Ray
 └─> Parser
      ├─> Spatial (uses Ray, Object)
      └─> Lighting (uses Ray, Object)
           └─> Rendering (uses Spatial, Lighting, Ray, Object)
                └─> Window
                     ├─> HUD (uses Metrics)
                     └─> KeyGuide
```

**Decision**: Maintain existing header/source directory alignment, but enforce strict domain boundaries. Each header should declare only functions implemented in its corresponding source directory. Cross-domain functions should be moved to the appropriate domain.

**Rationale**: Current organization largely follows these domains already. Refinement rather than reorganization minimizes risk and maintains familiar structure.

**Alternatives Considered**:
- Flat structure with single header - Rejected: Too difficult to navigate for codebase of this size
- Subdirectory structure with domain subdirs - Rejected: Overkill for current scale, adds complexity

---

### 3. Unused Code Detection Strategy

**Task**: Determine best practices for identifying unused functions in C codebase without advanced tooling.

**Findings**:

**Available Detection Methods**:

1. **Static Analysis Approach**:
   - Use `nm` to extract all defined functions from object files
   - Use `grep` to find all function calls in codebase
   - Compare defined vs called functions to identify unused
   - Limitation: Misses function pointer usage, MLX callbacks

2. **Compiler-Based Approach**:
   - Enable `-Wunused-function` for static functions
   - Review compiler warnings for unused code
   - Limitation: Only catches static functions, not header declarations

3. **Link-Time Analysis**:
   - Use `--gc-sections` with `-ffunction-sections` to identify dead code
   - Check linker map for removed sections
   - Limitation: Requires linker support, may have false positives

4. **Manual Code Review**:
   - Systematic review of each header file
   - Search codebase for each declared function
   - Verify in multiple contexts (direct calls, callbacks, function pointers)
   - Most reliable but time-intensive

**Best Practices for C Refactoring**:
- Start with compiler warnings as baseline
- Use grep/ripgrep for function usage search
- Verify each removal doesn't break compilation
- Test with all scene files after each batch of removals
- Keep git commits atomic (one removal category per commit)

**Decision**: Use hybrid approach:
1. Enable `-Wunused-function` and fix all warnings
2. Create script to list all header declarations and search for usage
3. Manual verification of borderline cases (callbacks, MLX integration)
4. Remove in batches, testing after each batch

**Rationale**: Combines automation benefits with manual verification safety. Minimizes risk of removing needed functions.

**Alternatives Considered**:
- Fully automated removal - Rejected: Too risky for functions used via function pointers
- Manual only - Rejected: Too time-consuming and error-prone for 164 functions

---

### 4. 42 Norminette Best Practices

**Task**: Research best practices for maintaining 42 norminette compliance during refactoring.

**Findings**:

**Norminette Rules Relevant to Refactoring**:

1. **Function Count**: Maximum 5 functions per file (excluding static helpers)
   - Current status: Need to verify per-file function counts
   - Impact: May require splitting large files during reorganization

2. **Line Count**: Maximum 25 lines per function
   - Current status: Need to verify function lengths
   - Impact: May require extracting helper functions

3. **Defines Placement**: No `#define` in `.c` files
   - Current status: ✅ Already compliant (0 defines in source files)
   - Impact: None - maintain current practice

4. **Include Guards**: All headers must have include guards
   - Current status: Need to verify all headers
   - Impact: Add guards where missing

5. **Function Declarations**: All declarations in headers, not scattered
   - Current status: Need to verify declaration placement
   - Impact: May need to move declarations to appropriate headers

**Best Practices for Norminette Compliance**:
- Run norminette after every file modification
- Keep function signatures short (split long parameter lists)
- Use consistent naming conventions (snake_case for functions)
- Minimize header dependencies (forward declarations where possible)
- Group related static helper functions near their usage

**Decision**: 
1. Run norminette validation on entire codebase before starting
2. Add norminette check to refactoring workflow (after each file change)
3. Document any norminette violations requiring multi-file fixes
4. Ensure final refactored code has zero norminette violations

**Rationale**: Continuous validation prevents regressions and ensures compliance is maintained throughout refactoring process.

**Alternatives Considered**:
- Fix norminette at the end - Rejected: Creates large cleanup burden, may require re-refactoring
- Skip norminette validation - Rejected: Violates constitution principle II and spec requirement FR-011

---

### 5. Korean Documentation Timing

**Task**: Clarify when Korean translation of specification should be created.

**Findings**:

From Constitution Principle VI (Bilingual Specification Management):
- "When English specification is created in specs/, Korean version MUST be created in docs/specs/"
- "Both versions MUST be updated in the same commit/PR"
- "PRs touching specs/ MUST include corresponding docs/specs/ changes"

**Current State**:
- English spec exists: `specs/012-code-cleanup-refactor/spec.md`
- Korean spec missing: `docs/specs/012-code-cleanup-refactor/spec.md`

**Decision**: Korean translation MUST be created as part of this planning phase, before implementation begins.

**Rationale**: Constitution explicitly requires both versions to exist and be updated together. Creating Korean version now prevents constitution violation.

**Alternatives Considered**:
- Defer to implementation phase - Rejected: Violates constitution synchronization requirement
- Create after English finalization - Rejected: Constitution requires same-commit updates

---

## Summary of Resolutions

### Technical Context Clarifications

All "NEEDS CLARIFICATION" items from Technical Context are now resolved:

| Item | Resolution |
|------|------------|
| Korean documentation timing | MUST create Korean spec in `docs/specs/012-code-cleanup-refactor/` now |
| Validate 42 function constraints | ✅ Zero prohibited functions found (will document validation method) |

### Constitution Check Clarifications

All warnings from Constitution Check are now resolved:

| Warning | Resolution |
|---------|------------|
| Must validate no functional changes | Will use scene rendering comparison (all test scenes must render identically) |
| Korean spec creation timing | Create now, before Phase 1 (constitution requirement) |
| Validate no prohibited functions | Verified: 0 pthread/fork calls, only allowed functions (libc, math, mlx) |

### Key Decisions Summary

1. **Refactoring Strategy**: Incremental improvements within existing structure rather than complete reorganization
2. **Unused Code Detection**: Hybrid automated + manual approach with batch testing
3. **Domain Organization**: 12 functional domains with clear dependency hierarchy
4. **Norminette Compliance**: Continuous validation throughout refactoring process
5. **Korean Documentation**: Create immediately as constitutional requirement

### Risk Assessment

**Low Risk**:
- ✅ No `#define` migration needed (already compliant)
- ✅ Existing structure is reasonable (refinement, not revolution)
- ✅ Atomic commits with testing reduce regression risk

**Medium Risk**:
- ⚠️ Function removal may break function pointer usage (mitigation: manual verification)
- ⚠️ Cross-domain dependencies may complicate reorganization (mitigation: dependency graph analysis)

**High Risk**: None identified

### Next Steps (Phase 1)

1. Create Korean specification in `docs/specs/012-code-cleanup-refactor/spec.md`
2. Generate `data-model.md` with file organization model and domain mapping
3. Create `contracts/` directory with:
   - `norminette-compliance.md` - Norminette validation contract
   - `function-usage.md` - Function call graph and usage validation
   - `domain-boundaries.md` - Domain dependency contract
4. Generate `quickstart.md` with guidelines for maintaining clean organization
5. Update agent context with refactoring knowledge

## References

- miniRT codebase analysis (2026-01-03)
- 42 norminette standards documentation
- Constitution v2.2.0 (2025-12-19)
- Feature specification: `specs/012-code-cleanup-refactor/spec.md`
