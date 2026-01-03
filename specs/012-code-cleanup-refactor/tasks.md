---

description: "Task list for code cleanup and refactoring feature"
---

# Tasks: Code Cleanup and Refactoring

**Input**: Design documents from `/specs/012-code-cleanup-refactor/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/, quickstart.md

**Tests**: Tests are NOT explicitly requested in the feature specification. This feature focuses on code organization without changing functionality.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: `src/`, `includes/` at repository root
- Repository structure: C project with MiniLibX graphics library
- Source directories: 11 subdirectories under `src/`
- Headers: 12 header files in `includes/`

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Prepare for refactoring with validation tools and baseline metrics

- [X] T001 Create baseline metrics by running line count analysis on all source files (src/**/*.c includes/*.h)
- [X] T002 Run norminette validation on entire codebase and document current compliance status
- [X] T003 [P] Create validation script scripts/validate_cleanup.sh for behavior preservation testing
- [X] T004 [P] Document current project structure with file count and LOC per domain in BASELINE.md
- [X] T005 Test all scene files to establish baseline rendering behavior (scenes/*.rt)

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core validation infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T006 Create function usage analysis script scripts/analyze_function_usage.sh using grep for call graph
- [X] T007 Generate initial function usage report listing all declared functions and usage counts
- [X] T008 [P] Create 42 function constraint validation script to check for prohibited functions
- [X] T009 [P] Run validation to confirm zero prohibited functions (pthread, fork, external libs)
- [X] T010 Establish git checkpoint with tag "cleanup-start" for safe rollback

**Checkpoint**: ✓ Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Remove Unused Code (Priority: P1) 🎯 MVP

**Goal**: Remove all unused functions and unnecessary statements to reduce codebase complexity and maintenance burden

**Independent Test**: Project compiles successfully with `make`, all scene files render identically to baseline, norminette passes with zero violations

### Implementation for User Story 1

- [ ] T011 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/hud/
- [ ] T012 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/keyguide/
- [ ] T013 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/lighting/
- [ ] T014 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/math/
- [ ] T015 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/parser/
- [ ] T016 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/ray/
- [ ] T017 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/render/
- [ ] T018 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/spatial/
- [ ] T019 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/utils/
- [ ] T020 [P] [US1] Identify all static functions with -Wunused-function compiler flag in src/window/
- [ ] T021 [US1] Analyze function usage report to identify unused global functions in includes/hud.h
- [ ] T022 [US1] Analyze function usage report to identify unused global functions in includes/keyguide.h
- [ ] T023 [US1] Analyze function usage report to identify unused global functions in includes/metrics.h
- [ ] T024 [US1] Analyze function usage report to identify unused global functions in includes/minirt.h
- [ ] T025 [US1] Analyze function usage report to identify unused global functions in includes/objects.h
- [ ] T026 [US1] Analyze function usage report to identify unused global functions in includes/parser.h
- [ ] T027 [US1] Analyze function usage report to identify unused global functions in includes/ray.h
- [ ] T028 [US1] Analyze function usage report to identify unused global functions in includes/render_state.h
- [ ] T029 [US1] Analyze function usage report to identify unused global functions in includes/shadow.h
- [ ] T030 [US1] Analyze function usage report to identify unused global functions in includes/spatial.h
- [ ] T031 [US1] Analyze function usage report to identify unused global functions in includes/vec3.h
- [ ] T032 [US1] Analyze function usage report to identify unused global functions in includes/window.h
- [ ] T033 [US1] Verify potential unused functions are not MLX callbacks or function pointer targets
- [ ] T034 [US1] Remove batch 1 unused functions from src/hud/ and corresponding declarations from includes/hud.h
- [ ] T035 [US1] Compile and test batch 1 removals with make and scene rendering validation
- [ ] T036 [US1] Remove batch 2 unused functions from src/keyguide/ and corresponding declarations from includes/keyguide.h
- [ ] T037 [US1] Compile and test batch 2 removals with make and scene rendering validation
- [ ] T038 [US1] Remove batch 3 unused functions from src/lighting/ and corresponding declarations from includes/shadow.h
- [ ] T039 [US1] Compile and test batch 3 removals with make and scene rendering validation
- [ ] T040 [US1] Remove batch 4 unused functions from src/math/ and corresponding declarations from includes/vec3.h
- [ ] T041 [US1] Compile and test batch 4 removals with make and scene rendering validation
- [ ] T042 [US1] Remove batch 5 unused functions from src/parser/ and corresponding declarations from includes/parser.h
- [ ] T043 [US1] Compile and test batch 5 removals with make and scene rendering validation
- [ ] T044 [US1] Remove batch 6 unused functions from src/ray/ and corresponding declarations from includes/ray.h
- [ ] T045 [US1] Compile and test batch 6 removals with make and scene rendering validation
- [ ] T046 [US1] Remove batch 7 unused functions from src/render/ and corresponding declarations from includes/render_state.h
- [ ] T047 [US1] Compile and test batch 7 removals with make and scene rendering validation
- [ ] T048 [US1] Remove batch 8 unused functions from src/spatial/ and corresponding declarations from includes/spatial.h
- [ ] T049 [US1] Compile and test batch 8 removals with make and scene rendering validation
- [ ] T050 [US1] Remove batch 9 unused functions from src/utils/ and corresponding declarations from includes/minirt.h
- [ ] T051 [US1] Compile and test batch 9 removals with make and scene rendering validation
- [ ] T052 [US1] Remove batch 10 unused functions from src/window/ and corresponding declarations from includes/window.h
- [ ] T053 [US1] Compile and test batch 10 removals with make and scene rendering validation
- [ ] T054 [US1] Identify and remove empty functions with no concrete logic from src/hud/
- [ ] T055 [US1] Identify and remove empty functions with no concrete logic from src/keyguide/
- [ ] T056 [US1] Identify and remove empty functions with no concrete logic from src/lighting/
- [ ] T057 [US1] Identify and remove empty functions with no concrete logic from src/math/
- [ ] T058 [US1] Identify and remove empty functions with no concrete logic from src/parser/
- [ ] T059 [US1] Identify and remove empty functions with no concrete logic from src/ray/
- [ ] T060 [US1] Identify and remove empty functions with no concrete logic from src/render/
- [ ] T061 [US1] Identify and remove empty functions with no concrete logic from src/spatial/
- [ ] T062 [US1] Identify and remove empty functions with no concrete logic from src/utils/
- [ ] T063 [US1] Identify and remove empty functions with no concrete logic from src/window/
- [ ] T064 [US1] Remove call sites for all removed empty functions across codebase
- [ ] T065 [US1] Run final compilation test with make to verify zero errors
- [ ] T066 [US1] Run norminette on all modified files to verify zero violations
- [ ] T067 [US1] Validate all scene files still render identically to baseline (behavior preservation)
- [ ] T068 [US1] Calculate LOC reduction and verify at least 10% reduction achieved (SC-004)
- [ ] T069 [US1] Git commit all US1 changes with message "feat(cleanup): remove unused code and empty functions"

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Fix 42 Norminette Violations (Priority: P1)

**Goal**: Move all #define statements from source files to headers to achieve 42 norminette compliance

**Independent Test**: Run norminette on all files and verify zero violations related to #define placement

**STATUS**: ✓ ALREADY COMPLIANT - Zero #define statements found in source files

### Implementation for User Story 2

- [X] T070 [P] [US2] Search for any #define statements in src/hud/*.c files - VERIFIED: 0 found
- [X] T071 [P] [US2] Search for any #define statements in src/keyguide/*.c files - VERIFIED: 0 found
- [X] T072 [P] [US2] Search for any #define statements in src/lighting/*.c files - VERIFIED: 0 found
- [X] T073 [P] [US2] Search for any #define statements in src/math/*.c files - VERIFIED: 0 found
- [X] T074 [P] [US2] Search for any #define statements in src/parser/*.c files - VERIFIED: 0 found
- [X] T075 [P] [US2] Search for any #define statements in src/ray/*.c files - VERIFIED: 0 found
- [X] T076 [P] [US2] Search for any #define statements in src/render/*.c files - VERIFIED: 0 found
- [X] T077 [P] [US2] Search for any #define statements in src/spatial/*.c files - VERIFIED: 0 found
- [X] T078 [P] [US2] Search for any #define statements in src/utils/*.c files - VERIFIED: 0 found
- [X] T079 [P] [US2] Search for any #define statements in src/window/*.c files - VERIFIED: 0 found
- [X] T080 [P] [US2] Search for any #define statements in main.c - VERIFIED: 0 found (main.c is in src/)
- [X] T081 [US2] Document all found #define statements - RESULT: Zero statements found
- [X] T082 [US2] Categorize #define statements - RESULT: N/A - already compliant
- [X] T083 [US2] Move file-specific #define statements - RESULT: N/A - already compliant
- [X] T084 [US2] Move project-wide #define statements - RESULT: N/A - already compliant
- [X] T085 [US2] Verify all moved #define statements compile - RESULT: N/A - no changes needed
- [X] T086 [US2] Run norminette on modified source files - RESULT: No violations (grep verified)
- [X] T087 [US2] Run norminette on modified header files - RESULT: No modifications needed
- [X] T088 [US2] Validate scene files render identically - RESULT: No changes made, behavior preserved
- [X] T089 [US2] Git commit - RESULT: No commit needed, already compliant

**Checkpoint**: ✓ User Story 2 complete - codebase was already compliant with zero #define in source files

---

## Phase 5: User Story 3 - Reorganize Related Code (Priority: P2)

**Goal**: Group related definitions and implementations into logically organized header/source file pairs by functional domain

**Independent Test**: Developer can locate all code for a specific domain in at most 2 files (one header, one source directory); peer review shows 90% agreement on logical grouping

### Implementation for User Story 3

- [ ] T090 [US3] Audit includes/hud.h to identify any functions belonging to other domains
- [ ] T091 [US3] Audit includes/keyguide.h to identify any functions belonging to other domains
- [ ] T092 [US3] Audit includes/metrics.h to identify any functions belonging to other domains
- [ ] T093 [US3] Audit includes/minirt.h to identify any functions belonging to other domains
- [ ] T094 [US3] Audit includes/objects.h to identify any functions belonging to other domains
- [ ] T095 [US3] Audit includes/parser.h to identify any functions belonging to other domains
- [ ] T096 [US3] Audit includes/ray.h to identify any functions belonging to other domains
- [ ] T097 [US3] Audit includes/render_state.h to identify any functions belonging to other domains
- [ ] T098 [US3] Audit includes/shadow.h to identify any functions belonging to other domains
- [ ] T099 [US3] Audit includes/spatial.h to identify any functions belonging to other domains
- [ ] T100 [US3] Audit includes/vec3.h to identify any functions belonging to other domains
- [ ] T101 [US3] Audit includes/window.h to identify any functions belonging to other domains
- [ ] T102 [US3] Create domain boundary map showing which functions should move to which headers
- [ ] T103 [US3] Move misplaced Math Domain functions to includes/vec3.h and src/math/
- [ ] T104 [US3] Compile and test Math Domain reorganization with make
- [ ] T105 [US3] Move misplaced Object Domain functions to includes/objects.h and appropriate source files
- [ ] T106 [US3] Compile and test Object Domain reorganization with make
- [ ] T107 [US3] Move misplaced Ray Domain functions to includes/ray.h and src/ray/
- [ ] T108 [US3] Compile and test Ray Domain reorganization with make
- [ ] T109 [US3] Move misplaced Spatial Domain functions to includes/spatial.h and src/spatial/
- [ ] T110 [US3] Compile and test Spatial Domain reorganization with make
- [ ] T111 [US3] Move misplaced Lighting Domain functions to includes/shadow.h and src/lighting/
- [ ] T112 [US3] Compile and test Lighting Domain reorganization with make
- [ ] T113 [US3] Move misplaced Rendering Domain functions to includes/render_state.h and src/render/
- [ ] T114 [US3] Compile and test Rendering Domain reorganization with make
- [ ] T115 [US3] Move misplaced Parser Domain functions to includes/parser.h and src/parser/
- [ ] T116 [US3] Compile and test Parser Domain reorganization with make
- [ ] T117 [US3] Move misplaced Window Domain functions to includes/window.h and src/window/
- [ ] T118 [US3] Compile and test Window Domain reorganization with make
- [ ] T119 [US3] Move misplaced HUD Domain functions to includes/hud.h and src/hud/
- [ ] T120 [US3] Compile and test HUD Domain reorganization with make
- [ ] T121 [US3] Move misplaced KeyGuide Domain functions to includes/keyguide.h and src/keyguide/
- [ ] T122 [US3] Compile and test KeyGuide Domain reorganization with make
- [ ] T123 [US3] Move misplaced Metrics Domain functions to includes/metrics.h and src/render/metrics*.c
- [ ] T124 [US3] Compile and test Metrics Domain reorganization with make
- [ ] T125 [US3] Verify each header contains only single-domain functionality (SC-005)
- [ ] T126 [US3] Run norminette on all modified files to verify zero violations
- [ ] T127 [US3] Validate all scene files still render identically after reorganization
- [ ] T128 [US3] Conduct peer review of domain boundaries for 90% agreement (SC-009)
- [ ] T129 [US3] Git commit all US3 changes with message "refactor(structure): reorganize code by functional domain"

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: User Story 4 - Improve File Organization Structure (Priority: P3)

**Goal**: Establish clear and consistent file organization structure that any developer can predict and follow

**Independent Test**: New developer can locate specific functionality within 2 minutes; organization pattern clearly indicates correct location for new features

### Implementation for User Story 4

- [X] T130 [US4] Verify header-to-source directory mapping consistency (12 headers to 11 source dirs) - VERIFIED
- [X] T131 [US4] Document any exceptions to 1:1 header-source mapping in data-model.md - DOCUMENTED  
- [ ] T132 [US4] Review all include statements to ensure minimal necessary dependencies
- [X] T133 [US4] Identify and eliminate duplicate function declarations across headers (SC-008) - FIXED: Removed intersect_* duplicates from minirt.h
- [X] T134 [US4] Ensure all headers have proper include guards following naming convention - VERIFIED: All 12 headers have guards
- [ ] T135 [US4] Standardize header file structure (guards, includes, defines, typedefs, declarations)
- [ ] T136 [US4] Organize functions within each header alphabetically or by logical grouping
- [ ] T137 [US4] Ensure source files follow standard structure (includes, defines-removed, static helpers, globals)
- [ ] T138 [US4] Verify function count per file is ≤5 non-static functions per norminette (SC-002)
- [ ] T139 [US4] Verify function length is ≤25 lines per norminette for all functions
- [ ] T140 [US4] Split any files exceeding function count limits into logical subfiles
- [ ] T141 [US4] Compile and test after any file splitting with make
- [ ] T142 [US4] Update Makefile if new source files were created during splitting
- [ ] T143 [US4] Verify compile_commands.json reflects new structure for IDE support
- [ ] T144 [US4] Run norminette on entire codebase to verify zero violations (SC-001, SC-007)
- [ ] T145 [US4] Validate all scene files still render identically after structural improvements
- [ ] T146 [US4] Test unfamiliar developer can locate functionality within 2 minutes (SC-010)
- [ ] T147 [US4] Git commit all US4 changes with message "refactor(organization): improve file structure consistency"

**Checkpoint**: All user stories should now be independently functional

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Final improvements that affect multiple user stories

- [ ] T148 [P] Review all modified files for code style consistency and readability
- [ ] T149 [P] Ensure all header comments follow 42 School header format
- [ ] T150 [P] Update documentation in docs/ if any user-facing changes occurred
- [ ] T151 [P] Verify all function names follow snake_case convention
- [ ] T152 Run complete norminette validation on all files to confirm zero violations (SC-001)
- [ ] T153 Run final compilation with make to confirm zero errors and warnings (SC-002)
- [ ] T154 Test all scene files in scenes/ directory to verify identical rendering (SC-003)
- [ ] T155 Calculate final LOC metrics and verify ≥10% reduction from baseline (SC-004)
- [ ] T156 Validate zero mixed-domain headers exist (SC-005)
- [ ] T157 Verify developer can locate domain functionality in ≤2 files (SC-006)
- [ ] T158 Confirm zero #define statements in source files (SC-007)
- [ ] T159 Confirm zero duplicate function declarations (SC-008)
- [ ] T160 Verify zero prohibited functions exist (SC-011)
- [ ] T161 Run memory leak check with valgrind on sample scenes
- [ ] T162 Update quickstart.md with any new patterns or guidelines discovered
- [ ] T163 Execute all validation steps from specs/012-code-cleanup-refactor/quickstart.md
- [ ] T164 Create final refactoring report with metrics and achievements
- [ ] T165 Git commit polish changes with message "docs(cleanup): finalize refactoring documentation"
- [ ] T166 Create git tag "cleanup-complete" marking refactoring completion

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3-6)**: All depend on Foundational phase completion
  - User Story 1 (P1): Can start after Foundational - No dependencies on other stories
  - User Story 2 (P1): Should start after User Story 1 to work on clean codebase
  - User Story 3 (P2): Should start after User Stories 1 and 2 to reorganize cleaned code
  - User Story 4 (P3): Should start after User Story 3 to finalize structure
- **Polish (Phase 7)**: Depends on all user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 2 (P1)**: Recommended after US1 but technically independent - Can start after Foundational
- **User Story 3 (P2)**: Should complete after US1 and US2 - Works best on cleaned, compliant code
- **User Story 4 (P3)**: Should complete after US3 - Finalizes structure improvements

### Within Each User Story

- Identification tasks before removal tasks
- Batch removals in sequence (compile and test between batches)
- Verification and testing after implementation changes
- Git commit after each user story completion
- User Story 1: Identify → Remove in batches → Test → Commit
- User Story 2: Search → Categorize → Move → Test → Commit
- User Story 3: Audit → Map → Move by domain → Test → Commit
- User Story 4: Verify → Standardize → Split if needed → Test → Commit

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel (T003, T004)
- All Foundational tasks marked [P] can run in parallel (T008, T009)
- Within US1: All identification tasks (T011-T020) can run in parallel
- Within US1: All header analysis tasks (T021-T032) can run in parallel
- Within US2: All #define search tasks (T070-T081) can run in parallel
- Within US3: All header audit tasks (T090-T101) can run in parallel
- Within Phase 7: All review tasks (T148-T151) can run in parallel
- User stories CAN proceed sequentially as recommended, but technically US1 and US2 are independent

---

## Parallel Example: User Story 1

```bash
# Launch all static function identification tasks together:
Task: "Identify all static functions with -Wunused-function in src/hud/"
Task: "Identify all static functions with -Wunused-function in src/keyguide/"
Task: "Identify all static functions with -Wunused-function in src/lighting/"
Task: "Identify all static functions with -Wunused-function in src/math/"
Task: "Identify all static functions with -Wunused-function in src/parser/"
Task: "Identify all static functions with -Wunused-function in src/ray/"
Task: "Identify all static functions with -Wunused-function in src/render/"
Task: "Identify all static functions with -Wunused-function in src/spatial/"
Task: "Identify all static functions with -Wunused-function in src/utils/"
Task: "Identify all static functions with -Wunused-function in src/window/"

# Launch all header analysis tasks together:
Task: "Analyze function usage report for includes/hud.h"
Task: "Analyze function usage report for includes/keyguide.h"
Task: "Analyze function usage report for includes/metrics.h"
Task: "Analyze function usage report for includes/minirt.h"
Task: "Analyze function usage report for includes/objects.h"
Task: "Analyze function usage report for includes/parser.h"
Task: "Analyze function usage report for includes/ray.h"
Task: "Analyze function usage report for includes/render_state.h"
Task: "Analyze function usage report for includes/shadow.h"
Task: "Analyze function usage report for includes/spatial.h"
Task: "Analyze function usage report for includes/vec3.h"
Task: "Analyze function usage report for includes/window.h"
```

---

## Parallel Example: User Story 2

```bash
# Launch all #define search tasks together:
Task: "Search for #define statements in src/hud/*.c"
Task: "Search for #define statements in src/keyguide/*.c"
Task: "Search for #define statements in src/lighting/*.c"
Task: "Search for #define statements in src/math/*.c"
Task: "Search for #define statements in src/parser/*.c"
Task: "Search for #define statements in src/ray/*.c"
Task: "Search for #define statements in src/render/*.c"
Task: "Search for #define statements in src/spatial/*.c"
Task: "Search for #define statements in src/utils/*.c"
Task: "Search for #define statements in src/window/*.c"
Task: "Search for #define statements in main.c"
```

---

## Parallel Example: User Story 3

```bash
# Launch all header audit tasks together:
Task: "Audit includes/hud.h for misplaced functions"
Task: "Audit includes/keyguide.h for misplaced functions"
Task: "Audit includes/metrics.h for misplaced functions"
Task: "Audit includes/minirt.h for misplaced functions"
Task: "Audit includes/objects.h for misplaced functions"
Task: "Audit includes/parser.h for misplaced functions"
Task: "Audit includes/ray.h for misplaced functions"
Task: "Audit includes/render_state.h for misplaced functions"
Task: "Audit includes/shadow.h for misplaced functions"
Task: "Audit includes/spatial.h for misplaced functions"
Task: "Audit includes/vec3.h for misplaced functions"
Task: "Audit includes/window.h for misplaced functions"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1 (Remove Unused Code)
4. **STOP and VALIDATE**: Test User Story 1 independently
5. Measure LOC reduction, verify compilation, test scenes

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Commit (MVP - leaner codebase!)
3. Add User Story 2 → Test independently → Commit (norminette compliance!)
4. Add User Story 3 → Test independently → Commit (domain organization!)
5. Add User Story 4 → Test independently → Commit (structural consistency!)
6. Complete Polish → Final validation → Tag release
7. Each story adds value without breaking previous stories

### Sequential Strategy (Recommended)

With single developer or small team:

1. Team completes Setup + Foundational together
2. Complete User Story 1 (remove dead code first)
3. Complete User Story 2 (fix norminette on clean code)
4. Complete User Story 3 (reorganize cleaned code)
5. Complete User Story 4 (finalize structure)
6. Complete Polish phase (validate everything)

This sequential approach is recommended because each story builds on the previous one, making the work cleaner and more logical.

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Compile and test after each batch of changes to catch regressions early
- Commit after each user story completion with descriptive message
- Stop at any checkpoint to validate story independently
- Behavior preservation is critical: all scene files MUST render identically
- Research shows codebase already has good foundation (no #define in .c files, modular structure)
- Focus is on refinement (removing unused, clarifying boundaries) rather than restructuring
- US2 may have zero work if no #define statements found in source files (research suggests this is likely)

---

## Task Count Summary

- **Phase 1 (Setup)**: 5 tasks
- **Phase 2 (Foundational)**: 5 tasks
- **Phase 3 (User Story 1)**: 59 tasks
- **Phase 4 (User Story 2)**: 20 tasks
- **Phase 5 (User Story 3)**: 40 tasks
- **Phase 6 (User Story 4)**: 18 tasks
- **Phase 7 (Polish)**: 19 tasks

**Total**: 166 tasks

### Parallel Opportunities

- Setup phase: 2 tasks can run in parallel (T003, T004)
- Foundational phase: 2 tasks can run in parallel (T008, T009)
- User Story 1: 22 tasks can run in parallel (T011-T020 identification, T021-T032 analysis)
- User Story 2: 11 tasks can run in parallel (T070-T081 search tasks)
- User Story 3: 12 tasks can run in parallel (T090-T101 audit tasks)
- Polish phase: 4 tasks can run in parallel (T148-T151 review tasks)

**Total parallel opportunities**: ~51 tasks can be executed in parallel

---

## Success Criteria Mapping

Each success criterion from spec.md is addressed by specific tasks:

- **SC-001** (Norminette compliance): T002, T066, T086, T087, T126, T144, T152
- **SC-002** (Compilation success): T065, T141, T153
- **SC-003** (Test cases pass): T067, T127, T145, T154
- **SC-004** (LOC reduction ≥10%): T068, T155
- **SC-005** (Zero mixed-domain headers): T125, T156
- **SC-006** (Locate functionality in ≤2 files): T157
- **SC-007** (Zero defines in source): T086, T158
- **SC-008** (Zero duplicate declarations): T133, T159
- **SC-009** (90% organization agreement): T128
- **SC-010** (40% time reduction): T146
- **SC-011** (Zero prohibited functions): T009, T160

All success criteria are measurable and validated by specific tasks.
