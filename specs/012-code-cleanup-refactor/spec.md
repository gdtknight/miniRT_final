# Feature Specification: Code Cleanup and Refactoring

**Feature Branch**: `012-code-cleanup-refactor`  
**Created**: 2026-01-03  
**Status**: Draft  
**Input**: User description: "헤더 파일, 소스 파일에 사용되지 않는 함수들, 불필요한 구문들 제거하고 관련성 있는 정의, 구현을 별도의 헤더와 소스 파일로 보기 좋게 구별할 필요가 있음. 너무 중구난방에 정의가 이곳 저곳 섞여있음. 그리고 42 norminette 에 따라 define 구문은 소스 파일에 존재해서는 안됨"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Remove Unused Code (Priority: P1)

As a developer maintaining the miniRT codebase, I need to remove all unused functions and unnecessary statements from header and source files so that the codebase is lean, maintainable, and easier to understand.

**Why this priority**: Removing dead code is foundational - it reduces maintenance burden, prevents confusion about what code is actually used, and makes subsequent refactoring safer and clearer. This must be done first before reorganization.

**Independent Test**: Can be fully tested by compiling the project after removal and verifying all existing functionality still works (scenes render correctly, controls respond, no compilation errors). Delivers immediate value by reducing code complexity.

**Acceptance Scenarios**:

1. **Given** header files with unused function declarations, **When** developer reviews and removes them, **Then** project compiles successfully and all existing features work
2. **Given** source files with functions containing no concrete internal logic (empty functions), **When** developer removes them and their call sites, **Then** no compilation errors occur and program behavior is unchanged
3. **Given** code with unnecessary or redundant statements, **When** developer removes them, **Then** program behavior is unchanged

---

### User Story 2 - Fix 42 Norminette Violations (Priority: P1)

As a 42 school student, I need all `#define` statements moved from source files to header files so that the code complies with 42 norminette rules and passes automated checks.

**Why this priority**: Compliance with 42 norminette is mandatory for project submission. This is P1 because it's a hard requirement that blocks acceptance. It's independently testable and doesn't require reorganization to be complete.

**Independent Test**: Can be fully tested by running norminette on all source files and verifying zero violations related to `#define` statements. Delivers compliance with school requirements.

**Acceptance Scenarios**:

1. **Given** source files containing `#define` statements, **When** developer moves them to appropriate header files, **Then** norminette shows no violations for those files
2. **Given** header files receiving moved `#define` statements, **When** developer organizes them logically, **Then** all definitions are properly guarded with header guards
3. **Given** the complete codebase, **When** norminette is run, **Then** zero violations related to `#define` placement exist

---

### User Story 3 - Reorganize Related Code (Priority: P2)

As a developer navigating the codebase, I need related definitions and implementations grouped together in logically organized header/source file pairs so that I can quickly find and understand code without hunting across multiple scattered locations.

**Why this priority**: Code organization improves long-term maintainability but can be done after removing dead code and fixing compliance issues. It requires understanding the full scope of what remains after cleanup.

**Independent Test**: Can be tested by having a developer unfamiliar with the codebase complete common tasks (e.g., "find all window-related functions", "locate vector math operations") and measuring time/file count. Delivers improved developer experience.

**Acceptance Scenarios**:

1. **Given** scattered definitions across multiple files, **When** developer consolidates them by functional domain, **Then** each header/source pair contains only related functionality
2. **Given** a specific feature domain (e.g., vector math, window management), **When** developer searches for related code, **Then** all relevant code is found in a single header/source pair
3. **Given** newly organized files, **When** developer needs to modify a feature, **Then** all relevant code is in one location

---

### User Story 4 - Improve File Organization Structure (Priority: P3)

As a team working on the codebase, we need a clear and consistent file organization structure so that any developer can predict where to find or add new code based on established patterns.

**Why this priority**: Structural improvements have long-term benefits but don't block immediate functionality or compliance. This should be done after the code is clean and logically grouped.

**Independent Test**: Can be tested by documenting the organization pattern and having new developers successfully locate/add code following the pattern without guidance. Delivers long-term scalability.

**Acceptance Scenarios**:

1. **Given** the reorganized codebase, **When** a new developer joins, **Then** they can locate specific functionality within 2 minutes based on file naming conventions
2. **Given** a new feature to implement, **When** developer decides where to place code, **Then** the organization pattern clearly indicates the correct location
3. **Given** the complete file structure, **When** reviewed, **Then** similar functionality follows consistent organizational patterns

---

### Edge Cases

- What happens when a function appears unused but is actually called through function pointers or dynamic dispatch?
- How does the system handle `#define` statements that are used across multiple source files?
- What if removing "unused" code breaks compilation due to header include dependencies?
- How to handle `#define` constants that are file-specific versus project-wide?
- What happens when related code spans multiple abstraction layers (e.g., low-level math used in high-level rendering)?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST identify and remove all function declarations in header files that have no corresponding usage in the codebase
- **FR-002**: System MUST identify and remove all function implementations in source files that are never called
- **FR-003**: System MUST remove empty functions (functions with no concrete internal logic) and all call sites that invoke these empty functions while preserving all functional behavior
- **FR-004**: System MUST move all `#define` statements from source files (`.c` files) to appropriate header files (`.h` files)
- **FR-005**: System MUST ensure all moved `#define` statements are placed in headers with proper include guards
- **FR-006**: System MUST group related definitions and implementations into logically coherent header/source file pairs
- **FR-007**: System MUST separate scattered definitions by functional domain (e.g., vector operations, window management, rendering, parsing)
- **FR-008**: System MUST ensure each header file contains only declarations/definitions related to a single functional domain
- **FR-009**: System MUST ensure each source file implements only the functionality declared in its corresponding header
- **FR-010**: System MUST maintain all existing functionality - no behavioral changes to the program
- **FR-011**: System MUST pass 42 norminette checks with zero violations related to `#define` placement
- **FR-012**: System MUST compile successfully after all cleanup and reorganization
- **FR-013**: System MUST preserve all existing include dependencies required for compilation
- **FR-014**: System MUST organize `#define` constants by scope (file-specific vs project-wide) and visibility
- **FR-015**: System MUST ensure no duplicate definitions exist after reorganization
- **FR-016**: System MUST validate that only allowed functions per 42 School constraints are used (libc, math.h, mlx_*, libft, get_next_line)
- **FR-017**: System MUST ensure no prohibited functions (pthread, fork, external libraries) are introduced during reorganization

### Key Entities *(include if feature involves data)*

- **Header File (.h)**: Contains type definitions, macros, function declarations, and structure definitions for a specific functional domain
- **Source File (.c)**: Contains function implementations corresponding to declarations in the associated header file
- **Define Statement**: Preprocessor macro definition that must reside only in header files per 42 norminette
- **Function Declaration**: Function prototype in header file that may or may not be used
- **Function Implementation**: Actual function code in source file that may or may not be called
- **Functional Domain**: Logical grouping of related code (e.g., vector math, window management, ray tracing, parser)
- **Include Dependency**: Relationship where one file requires definitions from another file to compile

## Clarifications

### Session 2026-01-03

- Q: What should be removed as "unnecessary statements"? → A: 내부에 구체적인 로직이 없는 함수와 해당 함수를 호출하는 라인을 삭제 (Delete functions with no concrete internal logic and lines that call those functions)

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: All source files pass 42 norminette validation with zero violations related to `#define` placement
- **SC-002**: Project compiles successfully with zero warnings or errors after all changes
- **SC-003**: All existing test cases pass without modification (behavior preservation)
- **SC-004**: Lines of code reduced by at least 10% through removal of unused functions and unnecessary statements
- **SC-005**: Number of header files containing mixed-domain code reduced to zero (each header has single responsibility)
- **SC-006**: Developer can locate all code for a specific functional domain by examining at most 2 files (one header, one source)
- **SC-007**: All `#define` statements exist only in header files (zero in source files)
- **SC-008**: No duplicate function declarations exist across header files
- **SC-009**: Code organization review by peer shows 90% agreement on logical grouping
- **SC-010**: Time to locate specific functionality for unfamiliar developer reduces by at least 40%
- **SC-011**: All functions used in codebase are validated against 42 School allowed function list (zero prohibited functions)
