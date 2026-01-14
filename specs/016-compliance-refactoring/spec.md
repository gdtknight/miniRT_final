# Feature Specification: miniRT Complete Compliance Refactoring

**Feature Branch**: `001-compliance-refactoring`  
**Created**: 2026-01-14  
**Status**: Draft  
**Input**: User description: "miniRT 프로젝트 전체 리팩토링 및 규정 준수"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Norminette Full Compliance (Priority: P1)

As a 42 School student submitting miniRT for evaluation, I need all source files to pass norminette 3.3.50 checks with zero errors so that my project meets the mandatory coding standards for evaluation.

**Why this priority**: This is the most critical requirement - without passing norminette, the project receives an automatic fail at 42 School evaluation, regardless of functionality. All other features are irrelevant if this requirement is not met.

**Independent Test**: Can be fully tested by running `norminette src/ includes/` and verifying zero errors across all files. Success is binary - either all files pass or the project fails evaluation.

**Acceptance Scenarios**:

1. **Given** all source and header files in the project, **When** I run `norminette src/ includes/`, **Then** the output shows 0 errors and 0 warnings for all files
2. **Given** a fresh clone of the repository, **When** I run the norminette check as part of CI/CD, **Then** the build pipeline passes the style check stage
3. **Given** any individual source file, **When** I run norminette on that specific file, **Then** it reports "OK!" with no violations

---

### User Story 2 - Allowed Functions Compliance (Priority: P1)

As a 42 School student, I need the codebase to use only functions explicitly listed in miniRT.pdf's allowed functions list so that my project doesn't violate technical requirements and receive penalties during evaluation.

**Why this priority**: Using forbidden functions (like memset, memcpy, memmove) results in automatic evaluation penalties or failure. This is as critical as norminette compliance for project acceptance.

**Independent Test**: Can be tested by auditing all source files for function calls and cross-referencing with the allowed functions list from miniRT.pdf. A simple grep search for common violations (memset, memcpy, memmove, etc.) provides immediate verification.

**Acceptance Scenarios**:

1. **Given** the allowed functions list from miniRT.pdf, **When** I audit all source files for external function calls, **Then** every function call matches an entry in the allowed list or is a project-defined function
2. **Given** the current 3 violations (memcpy in bvh_vis_prefix.c, memset in main.c), **When** I implement compliant replacements, **Then** these functions are replaced with either allowed alternatives or custom implementations
3. **Given** a grep search for common forbidden functions, **When** I run `grep -r "memset\|memcpy\|memmove\|strcpy\|strcat" src/ includes/`, **Then** the output shows 0 matches
4. **Given** any file using banned functions, **When** I replace them with compliant alternatives, **Then** the program behavior remains identical (same output for identical inputs)

---

### User Story 3 - Mandatory Feature Verification (Priority: P2)

As a 42 School evaluator, I need to verify that all mandatory features from miniRT.pdf are correctly implemented so that I can grade the project according to the official evaluation criteria.

**Why this priority**: After ensuring code compliance (P1), the next critical step is verifying that all required features exist and work correctly. Missing mandatory features result in incomplete project grading.

**Independent Test**: Can be tested by going through the miniRT.pdf requirements checklist and testing each mandatory feature (scene parsing, ray tracing for sphere/plane/cylinder, lighting model, window management) with provided test scenes.

**Acceptance Scenarios**:

1. **Given** a scene file with ambient lighting, **When** I render it with miniRT, **Then** all objects display the correct ambient color
2. **Given** a scene file with a camera, **When** I render it, **Then** the view matches the specified camera position, orientation, and FOV
3. **Given** a scene file with point lights, **When** I render it, **Then** diffuse lighting is correctly applied to all objects
4. **Given** scene files containing spheres, planes, and cylinders, **When** I render each, **Then** all object types are correctly displayed with proper intersection calculations
5. **Given** a valid scene file, **When** I press ESC or close the window, **Then** the program exits cleanly without crashes or memory leaks

---

### User Story 4 - Build System Integrity (Priority: P2)

As a developer working on miniRT, I need the Makefile to follow 42 School standards and build the project without errors so that the project can be compiled and evaluated in any standard 42 environment.

**Why this priority**: The Makefile is checked during evaluation. It must follow specific rules (correct targets, no relinking, proper flags) to pass evaluation requirements.

**Independent Test**: Can be tested by running `make`, `make re`, `make clean`, `make fclean` in sequence and verifying expected behavior (no relinking, proper cleaning, correct compiler flags).

**Acceptance Scenarios**:

1. **Given** a clean repository, **When** I run `make`, **Then** the project compiles successfully with all required flags (-Wall -Wextra -Werror) and produces the miniRT executable
2. **Given** a compiled project, **When** I run `make` again, **Then** no recompilation occurs (no relinking)
3. **Given** a compiled project, **When** I modify one source file and run `make`, **Then** only that file and dependent files are recompiled
4. **Given** a compiled project, **When** I run `make clean`, **Then** all object files are removed but the executable remains
5. **Given** a compiled project, **When** I run `make fclean`, **Then** all object files and the executable are removed
6. **Given** the Makefile, **When** I verify it contains the required targets (all, clean, fclean, re), **Then** all standard targets are present and functional

---

### User Story 5 - Code Deduplication (Priority: P2)

As a developer maintaining the miniRT codebase, I need to identify and remove duplicate source files (files with identical or substantially similar content under different names) so that the codebase is maintainable, reduces confusion, and prevents divergent implementations of the same functionality.

**Why this priority**: Duplicate code increases maintenance burden, violates DRY principles, can lead to bugs when one version is updated but not others, and wastes build time. This is critical for long-term code quality and should be addressed during the refactoring phase.

**Independent Test**: Can be tested by generating MD5/SHA256 hashes of all source files and identifying duplicates, or by using code similarity tools to find files with >90% identical content. Verification includes checking that all references are updated and build succeeds.

**Acceptance Scenarios**:

1. **Given** all source files in the project, **When** I generate content hashes (MD5/SHA256), **Then** no two different filenames have identical hashes
2. **Given** identified duplicate files, **When** I consolidate them into a single canonical version, **Then** the build completes successfully with all tests passing
3. **Given** removed duplicate files, **When** I check the git history, **Then** commit messages document which files were removed and which canonical version was retained
4. **Given** the consolidated codebase, **When** I search for references to old duplicate filenames, **Then** all references point to the canonical version
5. **Given** similar but not identical files, **When** I analyze their differences, **Then** I either merge them (if differences are trivial) or document why they must remain separate

---

### User Story 6 - Evaluation Checklist Readiness (Priority: P3)

As a 42 School student preparing for evaluation, I need all items from miniRT_eval.pdf to be verifiable so that evaluators can successfully complete the grading process without issues.

**Why this priority**: While core functionality (P1-P2) is most critical, ensuring smooth evaluation prevents delays and demonstrates project quality. This includes proper error handling, clean exit, and bonus features if implemented.

**Independent Test**: Can be tested by following the miniRT_eval.pdf checklist point-by-point, simulating an actual evaluation session with various test cases.

**Acceptance Scenarios**:

1. **Given** an invalid scene file (wrong format, invalid values), **When** I run miniRT with it, **Then** the program displays a clear error message and exits gracefully without crashing
2. **Given** scene files with edge cases (FOV at boundaries 0/180, very large/small object sizes), **When** I render them, **Then** the program handles them correctly or provides clear error messages
3. **Given** the running program, **When** I test window management (minimize, resize, multiple opens), **Then** the program responds correctly to all window events
4. **Given** a rendered scene, **When** I test keyboard inputs (ESC, camera controls if implemented), **Then** all documented controls work as specified
5. **Given** bonus features (specular reflection, shadows, textures, multi-threading), **When** claimed, **Then** each is demonstrably working and properly documented

---

### Edge Cases

- **Empty or malformed scene files**: What happens when the scene file is empty, has missing required elements (A/C/L), or contains syntax errors?
- **Extreme values**: How does the system handle FOV values at boundaries (0, 180), extremely large object sizes, or coordinates at numerical limits?
- **Memory constraints**: What happens when parsing extremely large scene files (thousands of objects) or during prolonged rendering sessions?
- **Invalid color values**: How does parsing handle RGB values outside the 0-255 range or negative values?
- **Concurrent modifications**: What happens if the scene file is modified while the program is running?
- **Platform-specific differences**: How does the code handle differences between Linux (X11) and macOS (AppKit) implementations of MinilibX?
- **Norminette corner cases**: How are long function signatures split across lines? Are all variable declarations at function start? Are all functions properly prototyped?
- **Duplicate file edge cases**: How do we handle files that are 90% similar but have minor differences? What if duplicate files have different modification dates? What if one version has bug fixes the other doesn't?

## Requirements *(mandatory)*

### Functional Requirements

#### Code Style and Standards
- **FR-001**: System MUST pass norminette 3.3.50 checks with zero errors and zero warnings on all .c and .h files in src/ and includes/ directories
- **FR-002**: All functions MUST be limited to 25 lines maximum (norminette requirement)
- **FR-003**: Each source file MUST contain no more than 5 functions (norminette requirement)
- **FR-004**: All function declarations MUST be limited to 4 parameters maximum (norminette requirement)
- **FR-005**: All variable declarations MUST appear at the beginning of functions, before any executable statements
- **FR-006**: System MUST use only functions from the allowed functions list specified in miniRT.pdf
- **FR-007**: System MUST NOT use forbidden functions including but not limited to: memset, memcpy, memmove, strcpy, strcat, strncpy, strncat
- **FR-008**: All custom replacement functions for banned functions MUST maintain identical behavior to replaced functions

#### Build System Requirements
- **FR-009**: Makefile MUST include standard targets: all, clean, fclean, re, bonus
- **FR-010**: Makefile MUST compile with flags: -Wall -Wextra -Werror
- **FR-011**: Makefile MUST NOT relink when running `make` twice consecutively on unchanged source
- **FR-012**: `make clean` MUST remove all object files (.o) but preserve the executable
- **FR-013**: `make fclean` MUST remove all object files and the executable
- **FR-014**: `make re` MUST perform a complete rebuild (fclean + all)

#### Mandatory Feature Requirements
- **FR-015**: System MUST parse scene files (.rt format) containing ambient lighting, camera, and light definitions
- **FR-016**: System MUST render sphere objects with correct ray-sphere intersection calculations
- **FR-017**: System MUST render plane objects with correct ray-plane intersection calculations
- **FR-018**: System MUST render cylinder objects with correct ray-cylinder intersection calculations including caps
- **FR-019**: System MUST implement ambient lighting according to the specified ratio and RGB values
- **FR-020**: System MUST implement diffuse lighting (Lambertian reflection) for point light sources
- **FR-021**: System MUST render scenes with the camera positioned at the specified coordinates, orientation, and FOV (0-180 degrees)
- **FR-022**: System MUST create a window using MinilibX and display the rendered scene
- **FR-023**: System MUST handle ESC key to cleanly exit the program
- **FR-024**: System MUST handle window close button (red X) to cleanly exit the program

#### Error Handling Requirements
- **FR-025**: System MUST display "Error\n" followed by a descriptive message for invalid scene files
- **FR-026**: System MUST exit gracefully (no segfaults, no memory leaks) when encountering invalid input
- **FR-027**: System MUST validate all numeric values (FOV range, RGB values 0-255, normalized vectors) before use
- **FR-028**: System MUST reject scene files with missing required elements (A, C, or L)
- **FR-029**: System MUST reject scene files with syntax errors and provide clear error messages

#### Memory Management Requirements
- **FR-030**: System MUST free all dynamically allocated memory before program termination
- **FR-031**: System MUST pass Valgrind (Linux) or leaks (macOS) checks with zero memory leaks
- **FR-032**: System MUST properly close all file descriptors opened during execution
- **FR-033**: System MUST properly destroy all MinilibX resources (images, windows) on exit

#### Code Organization Requirements
- **FR-034**: Source code MUST be organized in logical modules with clear separation of concerns
- **FR-035**: Each function MUST have a single, well-defined responsibility
- **FR-036**: Long functions currently exceeding 25 lines MUST be split into helper functions
- **FR-037**: All helper functions MUST be static when not needed in other files
- **FR-038**: All prototypes MUST be declared in appropriate header files before use

#### Code Deduplication Requirements
- **FR-039**: System MUST identify and remove duplicate source files (files with identical or substantially similar content but different names)
- **FR-040**: System MUST consolidate redundant implementations of the same functionality into a single, well-named source file
- **FR-041**: System MUST update all references (includes, Makefile, build system) when removing duplicate files
- **FR-042**: Removed duplicate files MUST be documented in commit messages with references to the canonical version retained

### Key Entities

- **Scene File (.rt)**: Text file containing scene description with required elements (Ambient, Camera, Light) and optional object definitions
- **Object**: Geometric primitive (sphere, plane, cylinder) with position, dimensions, and color properties
- **Light Source**: Point light with position, brightness ratio (0.0-1.0), and RGB color
- **Camera**: View point with position (x,y,z), orientation vector (normalized), and field of view (0-180 degrees)
- **Ray**: Mathematical construct representing a line from camera through a pixel into the scene
- **Intersection**: Point where a ray meets an object surface, includes distance and surface normal
- **Color**: RGB triplet with values 0-255 representing object or light colors
- **MinilibX Image**: Buffer for storing rendered pixel data before display
- **BVH Node**: Spatial acceleration structure node containing bounding box and object references

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of source and header files pass norminette 3.3.50 checks with zero errors (currently ~98 files)
- **SC-002**: Zero uses of forbidden functions (memset, memcpy, memmove, etc.) across entire codebase (currently 3 violations)
- **SC-003**: Build process completes successfully with -Wall -Wextra -Werror flags producing zero warnings
- **SC-004**: Program execution produces zero memory leaks when tested with Valgrind/leaks on all provided test scenes
- **SC-005**: All 40 test scenes render correctly with identical visual output before and after refactoring
- **SC-006**: Makefile passes all standard target tests (make, make re, make clean, make fclean) without errors
- **SC-007**: 100% of mandatory features from miniRT.pdf are demonstrable and functional
- **SC-008**: 100% of evaluation checklist items from miniRT_eval.pdf can be successfully verified
- **SC-009**: Program handles at least 20 common error scenarios gracefully (invalid files, wrong formats, edge cases) without crashes
- **SC-010**: Compilation time increases by less than 20% compared to pre-refactoring baseline (refactoring should not significantly impact build performance)
- **SC-011**: All existing functionality (rendering quality, interactive controls, BVH acceleration) remains operational without degradation
- **SC-012**: Code review by 42 School peer evaluators results in passing grade with no compliance violations noted

### Quality Targets

- **Norminette Compliance Rate**: 100% (zero tolerance for style violations)
- **Function Length Compliance**: 100% of functions ≤ 25 lines
- **Function Signature Compliance**: 100% of functions ≤ 4 parameters
- **Memory Leak Rate**: 0 bytes leaked (verified by automated tools)
- **Build Warning Count**: 0 warnings with strict compiler flags
- **Test Scene Pass Rate**: 100% of scenes render successfully
- **Documentation Coverage**: All modified functions have updated comments explaining changes
- **Regression Rate**: 0 functional regressions (all features work as before)

## Assumptions *(mandatory)*

### Development Environment
- **Assumption 1**: The development and evaluation environment is either Ubuntu 20.04+ (Linux) or macOS 10.14+ with standard 42 School toolchain
- **Assumption 2**: norminette version 3.3.50 or compatible is available and will be the reference standard
- **Assumption 3**: Standard compilation tools (gcc/clang, make, valgrind/leaks) are available
- **Assumption 4**: MinilibX library is properly installed and accessible

### Project State
- **Assumption 5**: The current codebase has functional implementations of all mandatory features (ray tracing, lighting, object rendering)
- **Assumption 6**: The existing test suite (40 scenes) represents comprehensive coverage of required functionality
- **Assumption 7**: Current violations are limited to style (norminette) and forbidden functions (3 instances), not fundamental design flaws
- **Assumption 8**: The existing BVH implementation, HUD system, and bonus features will be preserved if they comply with regulations
- **Assumption 9**: Duplicate source files (if any exist) are exact or near-exact copies, not intentionally similar implementations serving different purposes

### Technical Constraints
- **Assumption 10**: The allowed functions list from miniRT.pdf represents a complete and correct specification of permitted external functions
- **Assumption 11**: MinilibX API functions are implicitly allowed as they are the required graphics library
- **Assumption 12**: Standard library functions like malloc, free, open, read, write, close are allowed (verified against miniRT.pdf)
- **Assumption 13**: Mathematical operations and library functions (math.h) are allowed when explicitly listed

### Scope Boundaries
- **Assumption 14**: This refactoring focuses on compliance and code quality, not adding new features or improving rendering algorithms
- **Assumption 15**: Visual output must remain identical or equivalent after refactoring - no changes to rendering behavior
- **Assumption 16**: Performance characteristics (render times, memory usage) should remain similar, though minor variations are acceptable
- **Assumption 17**: Bonus features (if present) will be maintained only if they don't violate compliance requirements

### Validation Approach
- **Assumption 18**: Automated norminette checks provide definitive pass/fail criteria for style compliance
- **Assumption 19**: Manual code review simulating peer evaluation will catch edge cases not covered by automated tools
- **Assumption 20**: Regression testing with all 40 scenes will be sufficient to verify functional preservation
- **Assumption 21**: Memory leak detection with valgrind/leaks on representative scenes will be sufficient for memory safety verification

## Dependencies *(optional)*

### External Dependencies
- **norminette 3.3.50**: Required for style validation
- **MinilibX**: Graphics library (no version change expected)
- **X11/Xext libraries**: Required on Linux (no changes needed)
- **Math library (libm)**: Required for mathematical calculations
- **C standard library**: Subset of allowed functions as specified in miniRT.pdf

### Internal Dependencies
- **Makefile**: Must be updated to reflect any new source file organization
- **Header files**: May require reorganization if function signatures change during refactoring
- **Build system**: May need adjustments if modularization changes file structure
- **Documentation**: Must be updated to reflect compliance changes and new function organization

### Known Constraints
- **42 School Evaluation Timeline**: Refactoring must be completed before project evaluation deadline
- **Backward Compatibility**: Test scenes and evaluation checklist compatibility must be maintained
- **No Breaking Changes**: External interface (command-line arguments, scene file format) must remain unchanged
- **Toolchain Compatibility**: Must work with 42 School standard toolchain without requiring additional installations

## Implementation Notes *(optional)*

### Priority Order for Implementation
1. **Phase 1 - Duplicate File Detection**: Generate file hashes and identify duplicate source files
2. **Phase 2 - Duplicate File Consolidation**: Remove duplicates, update references, verify build
3. **Phase 3 - Forbidden Functions Audit**: Identify all uses of forbidden functions (current: 3 instances)
4. **Phase 4 - Function Replacement**: Replace memcpy, memset with compliant alternatives
5. **Phase 5 - Norminette First Pass**: Run norminette on all files, generate violation report
6. **Phase 6 - Function Splitting**: Split functions exceeding 25 lines into helper functions
7. **Phase 7 - Parameter Reduction**: Refactor functions with >4 parameters using structures or simplified interfaces
8. **Phase 8 - Style Fixes**: Fix remaining style issues (variable declarations, formatting, etc.)
9. **Phase 9 - Verification**: Run full test suite, memory checks, and final norminette validation
8. **Phase 8 - Documentation Update**: Update all affected documentation

### Common Refactoring Patterns
- **Long Functions**: Extract logic into static helper functions with clear names
- **Multiple Parameters**: Group related parameters into configuration structures
- **Forbidden Functions**: 
  - memcpy → manual loop copying or ft_memcpy if libft is allowed
  - memset → manual loop initialization
  - Replace only if not in allowed list
- **Variable Declarations**: Move all declarations to function start, initialize during declaration where possible

### Risk Mitigation
- **Risk**: Breaking existing functionality during refactoring
  - **Mitigation**: Run test suite after each major change
- **Risk**: Introducing memory leaks during refactoring
  - **Mitigation**: Run valgrind/leaks after each module refactoring
- **Risk**: Missing subtle norminette violations
  - **Mitigation**: Run norminette frequently, use automated pre-commit hooks
- **Risk**: Performance degradation from function splitting
  - **Mitigation**: Benchmark key rendering operations before/after changes

## Related Documentation

- **miniRT.pdf**: Official project requirements and allowed functions list
- **miniRT_eval.pdf**: Official evaluation checklist and grading criteria
- **42 Norminette Guidelines**: https://github.com/42School/norminette
- **Project README.md**: Current feature documentation and user guide
- **Project Wiki**: Technical documentation of algorithms and architecture
