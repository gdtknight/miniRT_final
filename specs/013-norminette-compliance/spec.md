# Feature Specification: Complete Norminette Compliance

**Feature Branch**: `013-norminette-compliance`  
**Created**: 2026-01-03  
**Status**: Draft  
**Input**: User description: "노미넷(norminette) 에러 77개를 0개로 완전 제거하기 위한 파일 분할 및 리팩토링 작업"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - File Function Count Compliance (Priority: P1)

As a developer maintaining the miniRT codebase, I need all source files to comply with the 42 norminette rule limiting 5 functions per file, so that the code passes automated compliance checks and maintains the required coding standards.

**Why this priority**: This addresses 69% of all errors (53 out of 77). Files with too many functions violate the fundamental organizational constraint of the 42 norm. Without resolving this, the project cannot achieve compliance certification.

**Independent Test**: Can be fully tested by running `norminette src/` and verifying zero TOO_MANY_FUNCS errors. Delivers immediate measurable progress toward 100% compliance.

**Acceptance Scenarios**:

1. **Given** hud_render.c contains 13 functions (8 over limit), **When** file is split into logical modules, **Then** each resulting file contains ≤5 functions and all HUD rendering functionality works identically
2. **Given** metrics.c contains 8 functions (3 over limit), **When** functions are reorganized into separate files by concern, **Then** norminette reports zero TOO_MANY_FUNCS errors and metrics tracking operates correctly
3. **Given** window.c contains 7 functions (2 over limit), **When** window initialization and event handling are separated, **Then** each file has ≤5 functions and window management behaves as before
4. **Given** timer.c and bvh_build.c each contain 6 functions (1 over limit), **When** one helper function is extracted to a new file in each case, **Then** both files pass norminette function count validation

---

### User Story 2 - Function Length Compliance (Priority: P2)

As a code reviewer, I need all functions to be ≤25 lines so that each function remains focused on a single responsibility and is easily reviewable and testable.

**Why this priority**: This addresses 6% of errors (5 TOO_MANY_LINES violations). Long functions indicate complex logic that should be decomposed for better maintainability and testing.

**Independent Test**: Run `norminette` on affected files and verify zero TOO_MANY_LINES errors. Each refactored function can be unit tested independently.

**Acceptance Scenarios**:

1. **Given** a function exceeds 25 lines due to complex calculations, **When** calculation logic is extracted to helper functions, **Then** the original function is ≤25 lines and produces identical output
2. **Given** a function exceeds 25 lines due to multiple conditional branches, **When** each branch is extracted to a named helper function, **Then** the function passes norminette length validation and logic remains unchanged
3. **Given** initialization functions exceed 25 lines, **When** initialization steps are grouped and extracted, **Then** each function is ≤25 lines and initialization succeeds completely

---

### User Story 3 - Header File Formatting Compliance (Priority: P2)

As a developer using the codebase, I need all header files to follow consistent tab/space formatting rules so that the code maintains visual consistency and passes automated style checks.

**Why this priority**: This addresses 12% of errors (10 TOO_FEW_TAB and SPC_BEFORE_NL violations). Header files are included across the codebase, so formatting issues here affect code readability widely.

**Independent Test**: Run `norminette includes/` and verify zero TOO_FEW_TAB and SPC_BEFORE_NL errors. Header compilation remains unchanged.

**Acceptance Scenarios**:

1. **Given** header files contain spaces where tabs are required, **When** proper tab indentation is applied, **Then** norminette reports zero TOO_FEW_TAB errors and headers compile successfully
2. **Given** header files contain trailing whitespace, **When** all trailing spaces are removed, **Then** norminette reports zero SPC_BEFORE_NL errors
3. **Given** function declarations have inconsistent indentation, **When** standardized tab indentation is applied, **Then** all declarations are visually aligned and pass norminette checks

---

### User Story 4 - Parameter and Variable Count Compliance (Priority: P3)

As a function implementer, I need to reduce functions with >4 parameters or >5 local variables by using parameter structures, so that function signatures remain manageable and comply with norminette rules.

**Why this priority**: This addresses 9% of errors (7 TOO_MANY_ARGS and TOO_MANY_VARS_FUNC violations). While less critical than file organization, these violations indicate opportunities for better data encapsulation.

**Independent Test**: Run norminette on affected functions and verify zero TOO_MANY_ARGS and TOO_MANY_VARS_FUNC errors. Function behavior remains identical with structured parameters.

**Acceptance Scenarios**:

1. **Given** a function has 6 parameters, **When** related parameters are grouped into a parameter structure, **Then** the function signature has ≤4 parameters and produces identical results
2. **Given** a function declares 7 local variables, **When** related variables are grouped or computation is refactored, **Then** the function has ≤5 local variables and executes the same logic
3. **Given** mlx library functions inherently have >4 parameters, **When** wrapper functions use structures for additional parameters, **Then** custom code complies with norminette while mlx violations are documented as external dependencies

---

### User Story 5 - Miscellaneous Compliance Issues (Priority: P3)

As a project maintainer, I need to resolve remaining style issues (preprocessor constants, function declaration alignment) so that the codebase achieves 100% norminette compliance.

**Why this priority**: This addresses 2% of errors (2 violations). These are minor issues that prevent achieving the final goal of zero errors.

**Independent Test**: Run `norminette` on the entire project and verify zero errors across all categories.

**Acceptance Scenarios**:

1. **Given** a preprocessor constant violates naming conventions, **When** the constant is renamed per norminette rules, **Then** norminette reports zero PREPROC_CONSTANT errors and all references are updated
2. **Given** function declarations are misaligned, **When** declarations are reformatted with proper spacing, **Then** norminette reports zero MISALIGNED_FUNC_DECL errors and compilation succeeds

---

### Edge Cases

- What happens when splitting a file causes circular dependencies between the new files?
- How does the build system handle new files added to deeply nested directory structures?
- What happens when refactoring changes function call patterns in performance-critical rendering code?
- How are header file dependencies updated when functions move between files?
- What happens when parameter structures introduce padding that affects memory alignment in real-time rendering?
- How does the system handle files that are at exactly 5 functions after refactoring (boundary case)?
- What happens if helper function extraction increases the total function count in the project?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST reduce files with >5 functions to ≤5 functions per file through logical splitting
- **FR-002**: System MUST refactor functions exceeding 25 lines into multiple functions each ≤25 lines
- **FR-003**: System MUST correct all tab/space formatting issues in header files per norminette rules
- **FR-004**: System MUST eliminate trailing whitespace from all source and header files
- **FR-005**: System MUST reduce functions with >4 parameters by introducing parameter structures where applicable
- **FR-006**: System MUST reduce functions with >5 local variables through refactoring or variable grouping
- **FR-007**: System MUST rename preprocessor constants to comply with norminette naming conventions
- **FR-008**: System MUST align function declarations according to norminette spacing rules
- **FR-009**: System MUST update Makefile to include all newly created source files
- **FR-010**: System MUST update header files with declarations for all newly extracted functions
- **FR-011**: System MUST maintain all existing functionality without behavior changes
- **FR-012**: System MUST preserve build success across all compilation targets
- **FR-013**: System MUST maintain performance characteristics of rendering pipeline (no measurable regression)
- **FR-014**: System MUST document all file splits and function extractions in commit messages
- **FR-015**: System MUST verify zero norminette errors across all source files after refactoring

### Key Entities

- **Source File Module**: Represents a .c file containing related functionality, must have ≤5 functions, organized by feature concern (e.g., HUD rendering, metrics tracking, window management)
- **Function Unit**: Represents a single function, must be ≤25 lines, have ≤4 parameters, and ≤5 local variables
- **Parameter Structure**: Represents a collection of related parameters grouped into a struct to reduce function signature complexity
- **Header File**: Represents a .h file declaring interfaces, must follow norminette tab/space rules and contain no trailing whitespace
- **Norminette Error**: Represents a specific rule violation with category (TOO_MANY_FUNCS, TOO_MANY_LINES, etc.), file location, and line number

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Norminette reports exactly 0 errors when run on the entire project (down from 77 errors)
- **SC-002**: All source files contain ≤5 functions (currently 8 files exceed this limit)
- **SC-003**: All functions are ≤25 lines (currently 5 functions exceed this limit)
- **SC-004**: All header files pass norminette formatting checks (currently 10 formatting violations)
- **SC-005**: Project compiles successfully with `make` and `make bonus` producing zero compiler errors
- **SC-006**: All existing test scenes render identically to pre-refactoring output (pixel-perfect comparison)
- **SC-007**: Rendering performance remains within 5% of baseline (no significant regression)
- **SC-008**: Code review confirms improved readability with smaller, focused functions
- **SC-009**: Project achieves 42 School norminette compliance certification requirement (0 errors)
- **SC-010**: Documentation clearly maps which functions were moved from which original files for traceability

## Assumptions

- The 42 norminette rules are fixed and well-defined (5 functions per file, 25 lines per function, 4 parameters per function, 5 variables per function)
- The mlx library functions that violate TOO_MANY_ARGS (4 instances) are external dependencies and cannot be modified
- Existing functionality is correctly implemented and does not require bug fixes during this refactoring
- Build system uses standard Make and can accommodate new source files without complex configuration
- Test scenes accurately represent all features and can validate behavior preservation
- Performance baseline can be established using existing timer/metrics infrastructure
- File splitting follows logical feature boundaries (e.g., HUD rendering, metrics collection, window management)
- Header file organization follows include guards and forward declaration best practices
- The Constitution document referenced contains coding standards that align with norminette rules
- Refactoring work can be done incrementally and tested at each stage (file splits, function extractions, formatting fixes)

## Dependencies

- norminette tool (version used by 42 School for compliance checking)
- Make build system (for compiling new file structure)
- mlx library (external dependency with unavoidable TOO_MANY_ARGS violations)
- Existing test infrastructure (test scenes, validation scripts)
- Git version control (for tracking changes and enabling rollback if needed)

## Scope Boundaries

**In Scope**:
- Splitting files to meet 5 functions per file limit
- Extracting helper functions to meet 25 lines per function limit
- Fixing tab/space formatting in headers
- Removing trailing whitespace
- Creating parameter structures for functions with >4 parameters
- Refactoring functions with >5 local variables
- Renaming non-compliant preprocessor constants
- Aligning misaligned function declarations
- Updating Makefile and headers for new file structure
- Verifying norminette compliance reaches 0 errors
- Validating build success and functionality preservation

**Out of Scope**:
- Modifying mlx library functions (external dependency)
- Adding new features or functionality
- Fixing unrelated bugs or issues
- Performance optimization beyond maintaining baseline
- Changing algorithm implementations or data structures
- Modifying test infrastructure or adding new tests
- Updating documentation beyond commit messages and code comments
- Refactoring code that already complies with norminette
- Changing project architecture or design patterns
- Adding new coding standards beyond norminette compliance

## Risk Analysis

**High Risk**:
- File splitting introduces compilation errors due to missing includes or dependencies
- Function extraction changes behavior in subtle ways not caught by existing tests
- Performance regression in rendering pipeline due to increased function call overhead
- Circular dependencies between newly split files prevent compilation

**Medium Risk**:
- Parameter structures add memory overhead affecting performance
- Increased number of files makes codebase navigation more complex
- Helper functions proliferate without clear naming conventions
- Build time increases with more translation units

**Low Risk**:
- Formatting changes introduce whitespace-related merge conflicts
- Documentation becomes outdated as functions move between files
- Code review takes longer with many small changes across multiple files

**Mitigation Strategies**:
- Test build after each file split to catch dependency issues immediately
- Run existing test scenes after each refactoring batch to validate behavior
- Profile rendering performance before and after to detect regressions
- Use clear naming conventions for extracted helper functions (e.g., original_func_helper_1)
- Update header files immediately when moving functions between files
- Commit changes in logical batches (file splits, function extractions, formatting) for easier review and rollback
