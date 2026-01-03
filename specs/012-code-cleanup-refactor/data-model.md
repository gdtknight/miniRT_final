# Data Model: Code Cleanup and Refactoring

**Feature**: 012-code-cleanup-refactor  
**Date**: 2026-01-03  
**Status**: Draft

## Overview

This document defines the organizational model for the miniRT codebase after cleanup and refactoring. It maps functional domains to file structures, establishes clear boundaries between components, and defines the relationships between headers and source directories.

## Entities

### 1. Functional Domain

**Definition**: A logically cohesive area of functionality that groups related types, functions, and data structures.

**Attributes**:
- **Name**: Domain identifier (e.g., "Math Domain", "Rendering Domain")
- **Primary Header**: Single header file that declares domain's public API
- **Source Directory**: Directory containing implementation files
- **Dependencies**: List of other domains this domain depends on
- **Responsibility**: Clear description of domain's purpose and scope

**Validation Rules**:
- Each domain MUST have exactly one primary header
- Header filename SHOULD match source directory name where applicable
- Dependencies MUST form a directed acyclic graph (no circular dependencies)
- All functions in domain MUST be related to domain's responsibility

**State Transitions**:
- Proposed → Validated → Implemented → Tested

---

### 2. Header File

**Definition**: C header file (`.h`) containing type definitions, macros, function declarations, and documentation for a specific domain.

**Attributes**:
- **Filename**: Header file name (e.g., `vec3.h`, `parser.h`)
- **Domain**: Associated functional domain
- **Include Guards**: Preprocessor guards preventing multiple inclusion
- **Dependencies**: List of other headers this header includes
- **Exports**: List of functions, types, and macros declared
- **Norminette Status**: Compliance status with 42 norminette

**Validation Rules**:
- MUST have include guards (e.g., `#ifndef VEC3_H` / `#define VEC3_H` / `#endif`)
- MUST NOT contain function implementations (inline functions excepted)
- All `#define` macros MUST be in headers (FR-004, FR-005)
- MUST NOT have circular dependencies with other headers
- All declarations MUST be used by at least one source file (no unused declarations)
- MUST pass norminette validation with zero violations

**Relationships**:
- One-to-one with Source Directory (in most cases)
- Many-to-many with Header Files (via includes)

---

### 3. Source Directory

**Definition**: Directory containing C implementation files (`.c`) for a specific domain.

**Attributes**:
- **Directory Name**: Source subdirectory name (e.g., `math/`, `parser/`)
- **Domain**: Associated functional domain
- **Files**: List of `.c` files in directory
- **Implements**: References to header file it implements
- **Internal Helpers**: List of static functions (file-local helpers)

**Validation Rules**:
- Directory name SHOULD match associated header name (e.g., `math/` ↔ `vec3.h`)
- All files MUST compile without warnings or errors
- MUST NOT contain `#define` statements (FR-004)
- Each file MUST have ≤5 functions per norminette rules (excluding static)
- Functions MUST be ≤25 lines per norminette rules

**Relationships**:
- One-to-one with Header File (primary mapping)
- May contain multiple `.c` files implementing different aspects of the domain

---

### 4. Function Declaration

**Definition**: Function prototype in a header file announcing function's existence and signature.

**Attributes**:
- **Name**: Function identifier
- **Return Type**: Function return type
- **Parameters**: List of parameter types and names
- **Declared In**: Header file containing declaration
- **Implemented In**: Source file containing implementation
- **Usage Count**: Number of call sites in codebase
- **Usage Type**: Direct call, callback, or function pointer

**Validation Rules**:
- Declaration and implementation signatures MUST match exactly
- MUST NOT be declared in multiple headers (no duplicates, FR-015)
- If `Usage Count == 0` and not a callback → candidate for removal (FR-001, FR-002)
- Function name MUST follow snake_case convention (norminette)

**State Transitions**:
- Declared → Implemented → Used (normal)
- Declared → Unused → Removed (cleanup)

---

### 5. Function Implementation

**Definition**: Actual function code in a source file.

**Attributes**:
- **Name**: Function identifier (matches declaration)
- **Implemented In**: Source file path
- **Line Count**: Number of lines in function body
- **Calls**: List of other functions this function calls
- **Called By**: List of functions that call this function
- **Visibility**: Static (file-local) or Global (exported)

**Validation Rules**:
- MUST have corresponding declaration in header (if global visibility)
- MUST be ≤25 lines (norminette rule)
- If `Called By == []` and not entry point/callback → unused (FR-002)
- Static functions should be near their usage point

**State Transitions**:
- Implemented → Called (active)
- Implemented → Uncalled → Removed (cleanup)

---

### 6. Code Organization Unit

**Definition**: Logical grouping of files representing a complete feature or subsystem.

**Attributes**:
- **Name**: Unit identifier (e.g., "Vector Math", "BVH Spatial Acceleration")
- **Header Files**: List of headers in this unit
- **Source Directories**: List of source dirs in this unit
- **Domain**: Associated functional domain
- **Dependencies**: Other organization units this unit depends on

**Validation Rules**:
- Dependencies MUST NOT be circular (acyclic graph)
- All files in unit MUST be related to unit's purpose
- Unit boundaries MUST be clear and well-documented

---

## Domain-to-File Mapping

### Proposed File Organization

| Domain | Primary Header | Source Directory | Files | Responsibility |
|--------|----------------|------------------|-------|----------------|
| **Core** | `minirt.h` | `main.c` (root) | 1 | Program lifecycle, main structures |
| **Math** | `vec3.h` | `src/math/` | 2 | Vector operations, mathematical utilities |
| **Object** | `objects.h` | *(embedded in render/ray)* | - | Scene object definitions and intersections |
| **Ray** | `ray.h` | `src/ray/` | 2 | Ray structures and operations |
| **Spatial** | `spatial.h` | `src/spatial/` | 5 | BVH implementation |
| **Lighting** | `shadow.h` | `src/lighting/` | 4 | Lighting calculations, shadows |
| **Rendering** | `render_state.h` | `src/render/` | 6 | Main rendering engine |
| **Parser** | `parser.h` | `src/parser/` | 4 | Scene file parsing |
| **Window** | `window.h` | `src/window/` | 2 | MLX window management |
| **HUD** | `hud.h` | `src/hud/` | 7 | Performance overlay |
| **KeyGuide** | `keyguide.h` | `src/keyguide/` | 3 | Interactive key guide |
| **Metrics** | `metrics.h` | *(in src/render/metrics*.c)* | 2 | Performance tracking |

**Total**: 12 domains, 12 headers, 11 source directories, ~38 source files

---

## Domain Dependency Model

### Dependency Graph

```
┌─────────────────────────────────────────────────────┐
│ Foundation Layer (No dependencies)                  │
│  • Math Domain (vec3.h)                             │
└────────────────────┬────────────────────────────────┘
                     │
                     ↓
┌─────────────────────────────────────────────────────┐
│ Core Data Layer                                      │
│  • Object Domain (objects.h) → depends on Math      │
│  • Ray Domain (ray.h) → depends on Math             │
└───────────────┬─────────────────────────────────────┘
                │
                ↓
┌─────────────────────────────────────────────────────┐
│ Algorithm Layer                                      │
│  • Spatial Domain (spatial.h) → Ray, Object, Math   │
│  • Lighting Domain (shadow.h) → Ray, Object, Math   │
│  • Parser Domain (parser.h) → Object, Math          │
└───────────────┬─────────────────────────────────────┘
                │
                ↓
┌─────────────────────────────────────────────────────┐
│ Rendering Layer                                      │
│  • Rendering (render_state.h) → All algorithm layer │
│  • Metrics (metrics.h) → Rendering                  │
└───────────────┬─────────────────────────────────────┘
                │
                ↓
┌─────────────────────────────────────────────────────┐
│ UI Layer                                             │
│  • Window Domain (window.h) → Rendering             │
│  • HUD Domain (hud.h) → Window, Metrics             │
│  • KeyGuide Domain (keyguide.h) → Window            │
└───────────────┬─────────────────────────────────────┘
                │
                ↓
┌─────────────────────────────────────────────────────┐
│ Application Layer                                    │
│  • Core Domain (minirt.h) → All domains             │
└─────────────────────────────────────────────────────┘
```

### Dependency Rules

1. **Acyclic Property**: No circular dependencies allowed (DAG)
2. **Layered Architecture**: Lower layers must not depend on higher layers
3. **Foundation Independence**: Math domain has zero dependencies
4. **Transitive Dependencies**: If A → B and B → C, then A transitively depends on C

---

## Cleanup Model

### Unused Code Identification

**Function State Model**:

```
┌──────────────┐
│  Declared    │ ←── In header file
└──────┬───────┘
       │
       ↓
┌──────────────┐
│ Implemented  │ ←── In source file
└──────┬───────┘
       │
       ├─→ Called by other functions → [KEEP]
       ├─→ Entry point (main) → [KEEP]
       ├─→ Callback (MLX, libft) → [KEEP]
       ├─→ Function pointer usage → [VERIFY MANUALLY]
       └─→ Never called → [REMOVE]
```

**Removal Categories**:

1. **Dead Declarations**: Function declared in header but never implemented
   - Action: Remove from header
   - Risk: Low (linker would catch if used)

2. **Dead Implementations**: Function implemented but never declared
   - Action: Should be static or removed
   - Risk: Low (unreachable code)

3. **Unused Functions**: Declared and implemented but never called
   - Action: Remove declaration + implementation
   - Risk: Medium (may be callback or future use)

4. **Unnecessary Statements**: Redundant code, commented blocks, dead branches
   - Action: Remove on case-by-case basis
   - Risk: Low (preserve behavior)

### Removal Workflow

```
1. Identify unused function
   ↓
2. Search entire codebase for usage (grep)
   ↓
3. Verify not a callback (MLX hooks, etc.)
   ↓
4. Remove implementation
   ↓
5. Remove declaration
   ↓
6. Compile and test
   ↓
7. Commit atomically
```

---

## Norminette Compliance Model

### Compliance Checklist per File

**Header Files** (`*.h`):
- ✅ Include guards present and correct
- ✅ No implementations (declarations only)
- ✅ All `#define` statements properly placed
- ✅ Function declarations properly formatted
- ✅ Line length ≤80 characters
- ✅ Proper 42 header comment

**Source Files** (`*.c`):
- ✅ No `#define` statements (FR-004)
- ✅ ≤5 functions per file (excluding static)
- ✅ ≤25 lines per function
- ✅ Proper 42 header comment
- ✅ Includes only necessary headers
- ✅ Line length ≤80 characters

### Validation States

```
┌────────────┐
│  Modified  │ ←── File changed during refactoring
└─────┬──────┘
      │
      ↓
┌────────────┐
│ Norminette │ ←── Run validation
│   Check    │
└─────┬──────┘
      │
      ├─→ PASS → [COMMIT]
      └─→ FAIL → [FIX] → [RE-CHECK]
```

---

## Success Metrics Model

### Measurable Outcomes (from spec SC-001 to SC-011)

| Metric ID | Metric | Target | Measurement Method |
|-----------|--------|--------|-------------------|
| **SC-001** | Norminette violations | 0 | `norminette` command |
| **SC-002** | Compilation status | Success | `make` exit code |
| **SC-003** | Test case pass rate | 100% | Scene rendering comparison |
| **SC-004** | LOC reduction | ≥10% | `wc -l` before/after |
| **SC-005** | Mixed-domain headers | 0 | Manual domain review |
| **SC-006** | Files per domain | ≤2 | Directory structure audit |
| **SC-007** | Defines in source files | 0 | `grep '#define' src/` |
| **SC-008** | Duplicate declarations | 0 | Header cross-reference |
| **SC-009** | Org review agreement | ≥90% | Peer review feedback |
| **SC-010** | Locate time reduction | ≥40% | Developer time study |
| **SC-011** | Prohibited functions | 0 | `grep` for pthread/fork |

### Tracking Model

```
Before State → Refactoring Actions → After State → Verify Metrics
```

Example for SC-004 (LOC reduction):
```
Before: 5,629 LOC
Target: ≤5,066 LOC (10% reduction)
Actions: Remove unused functions, eliminate redundant code
After: [TBD during implementation]
Verification: wc -l src/**/*.c includes/*.h
```

---

## Relationships

### Header ↔ Source Directory

```
Header File (1) ←→ (1) Source Directory
  │                      │
  │ declares            │ implements
  ↓                      ↓
Function Declarations   Function Implementations
```

### Domain ↔ Files

```
Functional Domain (1) ←→ (1) Primary Header
                  (1) ←→ (1) Source Directory
                  (1) ←→ (0..*) Dependency Domains
```

### Function Declaration ↔ Implementation

```
Function Declaration (1) ←→ (0..1) Function Implementation
  │
  └─→ (0..*) Call Sites (usage locations)
```

---

## Validation Rules Summary

**From Functional Requirements**:

- **FR-001**: System MUST identify and remove unused header declarations → Track via usage count
- **FR-002**: System MUST remove unused implementations → Track via call graph
- **FR-006**: System MUST group related code → Enforce via domain boundaries
- **FR-007**: System MUST separate by functional domain → Enforce via dependency graph
- **FR-010**: System MUST maintain functionality → Verify via scene rendering tests
- **FR-011**: System MUST pass norminette → Verify via automated checks
- **FR-015**: System MUST eliminate duplicates → Cross-reference all declarations

**Enforcement**:
- Pre-commit: Norminette validation
- Post-modification: Compilation test
- Post-cleanup: Scene rendering comparison
- Final verification: All metrics SC-001 through SC-011

---

## Notes

- This model represents the **target state** after refactoring completion
- Current codebase analysis shows good foundation (no defines in source, modular structure)
- Focus is on refinement (removing unused, clarifying boundaries) rather than restructuring
- All changes must preserve existing functionality (FR-010)
- Domain boundaries may be adjusted during implementation based on actual dependencies discovered

---

## References

- Feature Spec: `specs/012-code-cleanup-refactor/spec.md`
- Research Document: `specs/012-code-cleanup-refactor/research.md`
- miniRT Constitution v2.2.0
- 42 School norminette standards
