# Tasks: miniRT Complete Compliance Refactoring

**Input**: Design documents from `/specs/016-compliance-refactoring/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/refactoring-contracts.md

**Tests**: Not explicitly requested in specification - focusing on validation through norminette, memory checks, and pixel-diff tools.

**Organization**: Tasks are grouped by user story (compliance requirements) to enable independent validation and incremental delivery.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1, US2, US3, US4, US5)
- Include exact file paths in descriptions

## Path Conventions

- Single project: `src/`, `includes/`, `tests/`, `scenes/` at repository root
- Tracking metadata: `specs/016-compliance-refactoring/tracking/`
- Baseline validation: `tests/baselines/`

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and baseline capture for regression detection

- [ ] T001 Create tracking directory at specs/016-compliance-refactoring/tracking/
- [ ] T002 Create baseline directory at tests/baselines/ and tests/current/
- [ ] T003 [P] Generate baseline renders for all 38 scenes in scenes/ directory to tests/baselines/
- [ ] T004 [P] Create baseline metadata file at tests/baselines/metadata.json with commit hash and timestamp
- [ ] T005 [P] Document current build configuration in specs/016-compliance-refactoring/tracking/build_baseline.json

**Checkpoint**: Baseline captured - refactoring can now proceed with regression detection capability

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure and audit that MUST be complete before ANY user story implementation

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [ ] T006 Run norminette audit on all files and generate violation report at specs/016-compliance-refactoring/tracking/norminette_violations.txt
- [ ] T007 Generate file hashes for all source files and save to specs/016-compliance-refactoring/tracking/file_hashes.txt
- [ ] T008 Scan codebase for forbidden functions and document findings at specs/016-compliance-refactoring/tracking/forbidden_usage.json
- [ ] T009 Categorize norminette violations by type (line count, parameters, variables, formatting) in specs/016-compliance-refactoring/tracking/violation_categories.json
- [ ] T010 Create custom memory utilities module skeleton at src/utils/memory.c with includes/utils.h

**Checkpoint**: Foundation ready - compliance work can now begin per user story

---

## Phase 3: User Story 5 - Code Deduplication (Priority: P2) 🎯 Early Win

**Goal**: Remove duplicate source files to improve maintainability and reduce build complexity

**Independent Test**: Re-run hash generation - should find zero duplicate file groups. Build succeeds, all scenes render.

**Rationale**: Starting with US5 (deduplication) instead of US1 (norminette) because it's lower risk, has clear pass/fail criteria, and reduces the file count before extensive refactoring work.

### Implementation for User Story 5

- [ ] T011 [P] [US5] Analyze file_hashes.txt to identify duplicate groups (100% byte-for-byte match via SHA256)
- [ ] T012 [US5] Document duplicate groups in specs/016-compliance-refactoring/tracking/duplicates.json with canonical file selection
- [ ] T013 [US5] For each duplicate group, update Makefile to remove duplicate file references
- [ ] T014 [US5] Remove duplicate source files from filesystem (git rm)
- [ ] T015 [US5] Verify build succeeds after consolidation with make clean && make
- [ ] T016 [US5] Test all 38 scenes render successfully after deduplication
- [ ] T017 [US5] Run memory check on representative scenes (mandatory/*.rt) with valgrind/leaks
- [ ] T018 [US5] Commit consolidation with detailed message documenting removed files and canonical versions
- [ ] T019 [US5] Re-run hash generation to verify zero duplicates remain

**Checkpoint**: At this point, codebase has zero duplicate files, build is clean, all tests pass

---

## Phase 4: User Story 2 - Allowed Functions Compliance (Priority: P1)

**Goal**: Replace all forbidden function usages with compliant alternatives

**Independent Test**: `grep -rn "memset\|memcpy\|memmove" src/ includes/` returns no matches. Build succeeds, memory checks pass, pixel-diff <0.1%.

### Implementation for User Story 2

- [ ] T020 [P] [US2] Implement ft_memcpy in src/utils/memory.c following contract specification
- [ ] T021 [P] [US2] Implement ft_memset in src/utils/memory.c following contract specification
- [ ] T022 [P] [US2] Add function prototypes to includes/utils.h
- [ ] T023 [US2] Update Makefile to compile src/utils/memory.c
- [ ] T024 [US2] Build and verify ft_memcpy/ft_memset compile without errors
- [ ] T025 [US2] Replace memcpy usage in src/bvh_vis/bvh_vis_prefix.c line 45 with ft_memcpy
- [ ] T026 [US2] Replace memcpy usage in src/bvh_vis/bvh_vis_prefix.c line 47 with ft_memcpy
- [ ] T027 [US2] Replace memset usage in src/main.c line 123 with ft_memset
- [ ] T028 [US2] Add #include "utils.h" to all files using ft_memcpy/ft_memset
- [ ] T029 [US2] Build project and verify zero compilation errors
- [ ] T030 [US2] Run forbidden function check to verify zero violations
- [ ] T031 [US2] Test BVH visualization scenes to validate ft_memcpy replacements
- [ ] T032 [US2] Run memory check on all mandatory scenes with valgrind/leaks
- [ ] T033 [US2] Run pixel-diff validation comparing current renders to baseline (<0.1% threshold)
- [ ] T034 [US2] Commit forbidden function replacements with detailed testing evidence

**Checkpoint**: All forbidden functions replaced, zero violations, functionality preserved, memory-safe

---

## Phase 5: User Story 1 - Norminette Full Compliance (Priority: P1) 🎯 Critical Path

**Goal**: Achieve 100% norminette compliance across all source and header files

**Independent Test**: `norminette src/ includes/` shows zero errors and zero warnings. All scenes render with pixel-diff <0.1%.

**Note**: This is the largest phase, broken into sub-phases by violation type

### Sub-Phase 5A: Function Splitting (Functions Exceeding 25 Lines)

- [ ] T035 [US1] Identify all functions with TOO_MANY_LINES violations from tracking/violation_categories.json
- [ ] T036 [US1] Prioritize functions by complexity (start with functions 26-35 lines, then 36-50, then 50+)
- [ ] T037 [P] [US1] Split long functions in src/parser/ module into static helper functions
- [ ] T038 [P] [US1] Split long functions in src/render/ module into static helper functions
- [ ] T039 [P] [US1] Split long functions in src/ray/ module into static helper functions
- [ ] T040 [P] [US1] Split long functions in src/object/ module into static helper functions
- [ ] T041 [P] [US1] Split long functions in src/bvh/ module into static helper functions
- [ ] T042 [P] [US1] Split long functions in src/camera/ module into static helper functions
- [ ] T043 [P] [US1] Split long functions in src/light/ module into static helper functions
- [ ] T044 [P] [US1] Split long functions in src/material/ module into static helper functions
- [ ] T045 [P] [US1] Split long functions in src/color/ module into static helper functions
- [ ] T046 [P] [US1] Split long functions in src/vector/ module into static helper functions
- [ ] T047 [P] [US1] Split long functions in src/hit/ module into static helper functions
- [ ] T048 [P] [US1] Split long functions in src/window/ module into static helper functions
- [ ] T049 [P] [US1] Split long functions in src/error/ module into static helper functions
- [ ] T050 [P] [US1] Split long functions in src/utils/ module into static helper functions
- [ ] T051 [US1] After each module split, run norminette on that module to verify TOO_MANY_LINES violations cleared
- [ ] T052 [US1] After each module split, test representative scenes for that module (parser: all scenes, render: mandatory/*.rt)
- [ ] T053 [US1] Run full build after all function splitting complete
- [ ] T054 [US1] Run memory check on all mandatory scenes after function splitting
- [ ] T055 [US1] Run pixel-diff validation after function splitting (<0.1% threshold)
- [ ] T056 [US1] Commit function splitting work with detailed per-module breakdown

### Sub-Phase 5B: Parameter Reduction (Functions with >4 Parameters)

- [ ] T057 [US1] Identify all functions with TOO_MANY_ARGS violations from tracking/violation_categories.json
- [ ] T058 [P] [US1] Reduce parameters in src/render/ module using context structures
- [ ] T059 [P] [US1] Reduce parameters in src/ray/ module using context structures
- [ ] T060 [P] [US1] Reduce parameters in src/parser/ module using context structures
- [ ] T061 [P] [US1] Reduce parameters in src/camera/ module using context structures
- [ ] T062 [P] [US1] Reduce parameters in other modules with TOO_MANY_ARGS violations
- [ ] T063 [US1] Define context structures in appropriate headers (includes/render.h, includes/ray.h, etc.)
- [ ] T064 [US1] Update all call sites for refactored functions
- [ ] T065 [US1] Run norminette on modified files to verify TOO_MANY_ARGS violations cleared
- [ ] T066 [US1] Build project after parameter reduction
- [ ] T067 [US1] Test all mandatory scenes after parameter reduction
- [ ] T068 [US1] Run memory check after parameter reduction
- [ ] T069 [US1] Run pixel-diff validation after parameter reduction (<0.1% threshold)
- [ ] T070 [US1] Commit parameter reduction work

### Sub-Phase 5C: Variable Declaration Compliance

- [ ] T071 [US1] Identify all VAR_DECL_START_FUNC violations from tracking/violation_categories.json
- [ ] T072 [P] [US1] Move variable declarations to function start in src/parser/ module
- [ ] T073 [P] [US1] Move variable declarations to function start in src/render/ module
- [ ] T074 [P] [US1] Move variable declarations to function start in src/ray/ module
- [ ] T075 [P] [US1] Move variable declarations to function start in src/object/ module
- [ ] T076 [P] [US1] Move variable declarations to function start in src/bvh/ module
- [ ] T077 [P] [US1] Move variable declarations to function start in remaining modules with violations
- [ ] T078 [US1] Run norminette on modified files to verify VAR_DECL_START_FUNC violations cleared
- [ ] T079 [US1] Build and test after variable declaration fixes
- [ ] T080 [US1] Commit variable declaration compliance work

### Sub-Phase 5D: Style and Formatting Fixes

- [ ] T081 [US1] Identify remaining style violations (indentation, spacing, line length) from norminette output
- [ ] T082 [P] [US1] Fix formatting violations in src/ files
- [ ] T083 [P] [US1] Fix formatting violations in includes/ files
- [ ] T084 [P] [US1] Fix function declarations spanning multiple lines
- [ ] T085 [P] [US1] Fix line length violations (80 column limit)
- [ ] T086 [P] [US1] Fix indentation and spacing issues
- [ ] T087 [US1] Run norminette on all files to verify zero remaining violations
- [ ] T088 [US1] Build and test after style fixes
- [ ] T089 [US1] Commit style and formatting fixes

### Sub-Phase 5E: Final Norminette Validation

- [ ] T090 [US1] Run full norminette check on src/ and includes/ directories
- [ ] T091 [US1] Verify output shows "OK!" for all 75 source and header files
- [ ] T092 [US1] Document final norminette status in specs/016-compliance-refactoring/tracking/norminette_final.txt
- [ ] T093 [US1] Run complete build with make fclean && make
- [ ] T094 [US1] Test all 38 scenes render successfully
- [ ] T095 [US1] Run memory check on all scenes
- [ ] T096 [US1] Run complete pixel-diff validation suite (<0.1% threshold on all 38 scenes)
- [ ] T097 [US1] Commit final norminette compliance achievement
- [ ] T098 [US1] Tag release as v1.0.0-norminette-compliant

**Checkpoint**: 100% norminette compliance achieved, all functionality preserved, memory-safe, visually identical

---

## Phase 6: User Story 3 - Mandatory Feature Verification (Priority: P2)

**Goal**: Verify all mandatory features still work correctly after refactoring

**Independent Test**: Complete evaluation checklist from miniRT_eval.pdf with zero failures

### Implementation for User Story 3

- [ ] T099 [P] [US3] Test ambient lighting feature with scenes/mandatory/ambient.rt
- [ ] T100 [P] [US3] Test camera positioning and FOV with scenes/mandatory/camera_*.rt files
- [ ] T101 [P] [US3] Test diffuse lighting with scenes/mandatory/lighting.rt
- [ ] T102 [P] [US3] Test sphere rendering with scenes/mandatory/sphere.rt
- [ ] T103 [P] [US3] Test plane rendering with scenes/mandatory/plane.rt
- [ ] T104 [P] [US3] Test cylinder rendering with scenes/mandatory/cylinder.rt
- [ ] T105 [P] [US3] Test window management (ESC key, close button) across multiple scenes
- [ ] T106 [P] [US3] Test error handling with scenes/error_cases/*.rt (invalid formats)
- [ ] T107 [P] [US3] Test edge cases with scenes/edge_cases/*.rt (FOV boundaries, extreme values)
- [ ] T108 [US3] Document mandatory feature test results in specs/016-compliance-refactoring/tracking/feature_validation.json
- [ ] T109 [US3] Verify all mandatory features from miniRT.pdf checklist pass
- [ ] T110 [US3] Commit feature verification results

**Checkpoint**: All mandatory features verified working, edge cases handled, error handling robust

---

## Phase 7: User Story 4 - Build System Integrity (Priority: P2)

**Goal**: Verify Makefile follows 42 School standards and all targets work correctly

**Independent Test**: All Makefile targets execute correctly with expected behavior

### Implementation for User Story 4

- [ ] T111 [US4] Verify Makefile contains required targets (all, clean, fclean, re, bonus if applicable)
- [ ] T112 [US4] Verify compilation flags include -Wall -Wextra -Werror
- [ ] T113 [US4] Test make all builds successfully with zero warnings
- [ ] T114 [US4] Test make clean removes object files but preserves executable
- [ ] T115 [US4] Test make fclean removes object files and executable
- [ ] T116 [US4] Test make re performs complete rebuild (fclean + all)
- [ ] T117 [US4] Test no-relink behavior: make twice should not recompile on second run
- [ ] T118 [US4] Verify incremental compilation: modify one file, only that file recompiles
- [ ] T119 [US4] Update Makefile if any violations found
- [ ] T120 [US4] Document build system verification in specs/016-compliance-refactoring/tracking/build_verification.txt
- [ ] T121 [US4] Commit any Makefile improvements

**Checkpoint**: Build system fully compliant with 42 School standards

---

## Phase 8: User Story 6 - Evaluation Checklist Readiness (Priority: P3)

**Goal**: Ensure project is ready for smooth 42 School evaluation

**Independent Test**: Complete miniRT_eval.pdf checklist point-by-point with zero issues

### Implementation for User Story 6

- [ ] T122 [P] [US6] Test all error messages display properly (invalid scene files, missing elements)
- [ ] T123 [P] [US6] Verify program exits gracefully for all error conditions (no segfaults)
- [ ] T124 [P] [US6] Test all window events (minimize, resize, multiple window opens)
- [ ] T125 [P] [US6] Test all keyboard controls (ESC and any bonus controls if implemented)
- [ ] T126 [P] [US6] Verify bonus features work if claimed (BVH, specular, shadows, etc.)
- [ ] T127 [US6] Test prolonged rendering sessions for memory stability
- [ ] T128 [US6] Test with extremely large scene files (stress test)
- [ ] T129 [US6] Document evaluation readiness in specs/016-compliance-refactoring/tracking/eval_checklist.json
- [ ] T130 [US6] Create evaluation preparation guide at docs/EVALUATION_GUIDE.md
- [ ] T131 [US6] Commit evaluation readiness documentation

**Checkpoint**: Project fully ready for 42 School evaluation with comprehensive testing evidence

---

## Phase 9: Polish & Cross-Cutting Concerns

**Purpose**: Final improvements, documentation, and release preparation

- [ ] T132 [P] Update README.md with compliance achievements and any build changes
- [ ] T133 [P] Update docs/CHANGELOG.md with comprehensive refactoring summary
- [ ] T134 [P] Review and update function comments for all refactored functions
- [ ] T135 [P] Verify all tracking JSON files are complete and accurate
- [ ] T136 Run final comprehensive validation suite (norminette, build, memory, pixel-diff, features)
- [ ] T137 Create refactoring completion report at specs/016-compliance-refactoring/COMPLETION_REPORT.md
- [ ] T138 Review quickstart.md and verify all procedures still accurate
- [ ] T139 Tag final release as v1.0.0-compliant
- [ ] T140 Archive tracking metadata for future reference
- [ ] T141 Run specs/016-compliance-refactoring/quickstart.md validation checklist
- [ ] T142 Final commit: documentation and release preparation

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Story 5 - Deduplication (Phase 3)**: Depends on Foundational (Phase 2) - Independent of other stories
- **User Story 2 - Forbidden Functions (Phase 4)**: Depends on Foundational (Phase 2) - Independent, but recommended after US5
- **User Story 1 - Norminette (Phase 5)**: Depends on US2 and US5 completion for cleaner codebase
- **User Story 3 - Features (Phase 6)**: Can run anytime after US1 complete - validation phase
- **User Story 4 - Build System (Phase 7)**: Can run anytime after US1 complete - validation phase
- **User Story 6 - Eval Readiness (Phase 8)**: Depends on all previous user stories complete
- **Polish (Phase 9)**: Depends on all user stories complete

### User Story Dependencies

- **User Story 5 (P2 - Deduplication)**: FIRST - No dependencies, reduces file count for subsequent work
- **User Story 2 (P1 - Forbidden Functions)**: SECOND - Depends on Foundational, independent of other stories
- **User Story 1 (P1 - Norminette)**: THIRD - Depends on US2 and US5 for cleaner baseline
- **User Story 3 (P2 - Feature Verification)**: FOURTH - Validation after core compliance achieved
- **User Story 4 (P2 - Build System)**: FIFTH - Can run parallel to US3
- **User Story 6 (P3 - Eval Readiness)**: SIXTH - Final validation before evaluation

### Within Each User Story

**US5 (Deduplication)**:
- Detection → Consolidation → Verification → Commit

**US2 (Forbidden Functions)**:
- Implement utilities → Build → Replace usages → Validate → Commit

**US1 (Norminette)**:
- Function splitting → Parameter reduction → Variable declarations → Formatting → Final validation
- Each sub-phase: Fix violations → Norminette check → Build → Test → Commit

**US3, US4, US6**: Test and document

### Parallel Opportunities

- **Phase 1**: Tasks T003, T004, T005 can run in parallel
- **Phase 2**: Tasks T006-T010 can run in parallel (different operations)
- **Phase 3 (US5)**: Task T011 parallelizable after T010
- **Phase 4 (US2)**: Tasks T020, T021, T022 can run in parallel; T025-T027 can run in parallel
- **Phase 5A (US1 Function Splitting)**: Tasks T037-T050 can run in parallel (different modules)
- **Phase 5B (US1 Parameters)**: Tasks T058-T062 can run in parallel (different modules)
- **Phase 5C (US1 Variables)**: Tasks T072-T077 can run in parallel (different modules)
- **Phase 5D (US1 Formatting)**: Tasks T082-T086 can run in parallel (different files)
- **Phase 6 (US3)**: Tasks T099-T107 can run in parallel (different test categories)
- **Phase 8 (US6)**: Tasks T122-T126 can run in parallel (different test areas)
- **Phase 9**: Tasks T132-T135 can run in parallel (different documentation)

---

## Parallel Example: User Story 1 - Function Splitting

```bash
# Launch function splitting for multiple modules in parallel:
Task T037: "Split long functions in src/parser/ module into static helper functions"
Task T038: "Split long functions in src/render/ module into static helper functions"
Task T039: "Split long functions in src/ray/ module into static helper functions"
Task T040: "Split long functions in src/object/ module into static helper functions"
# ... etc (all tasks T037-T050 are parallelizable)

# Each module can be worked on independently, then:
# - Run norminette on that module
# - Test scenes relevant to that module
# - Commit that module's changes
```

---

## Implementation Strategy

### MVP First (Minimum Viable Compliance)

1. Complete Phase 1: Setup (baseline capture)
2. Complete Phase 2: Foundational (audit and preparation)
3. Complete Phase 3: User Story 5 - Deduplication
4. Complete Phase 4: User Story 2 - Forbidden Functions
5. **STOP and VALIDATE**: Verify zero forbidden functions, build clean
6. This gives us a smaller, cleaner codebase for the heavy norminette work

### Critical Path (Full Compliance)

1. Setup + Foundational → Foundation ready
2. Add User Story 5 (Deduplication) → Fewer files to refactor
3. Add User Story 2 (Forbidden Functions) → Compliance achieved
4. Add User Story 1 (Norminette) → Full compliance achieved (MVP for 42 eval!)
5. Add User Story 3 (Feature Verification) → Confidence before eval
6. Add User Story 4 (Build System) → Professional polish
7. Add User Story 6 (Eval Readiness) → Prepared for evaluation

### Incremental Delivery Milestones

- **Milestone 1**: Baseline captured (Phase 1) - Can begin refactoring safely
- **Milestone 2**: Duplicates removed (Phase 3) - Cleaner codebase
- **Milestone 3**: Forbidden functions eliminated (Phase 4) - Compliance checkpoint
- **Milestone 4**: Norminette compliance achieved (Phase 5) - READY FOR EVALUATION
- **Milestone 5**: Full validation complete (Phases 6-8) - Evaluation confidence
- **Milestone 6**: Polished and documented (Phase 9) - Release ready

### Estimated Timeline

- **Phase 1 (Setup)**: 2-3 hours
- **Phase 2 (Foundational)**: 2-3 hours
- **Phase 3 (US5 - Deduplication)**: 0.5-1 day (depends on duplicate count)
- **Phase 4 (US2 - Forbidden Functions)**: 0.5-1 day
- **Phase 5 (US1 - Norminette)**: 5-7 days (largest effort)
  - Sub-Phase 5A (Function Splitting): 2-3 days
  - Sub-Phase 5B (Parameter Reduction): 1-2 days
  - Sub-Phase 5C (Variables): 1 day
  - Sub-Phase 5D (Formatting): 0.5-1 day
  - Sub-Phase 5E (Validation): 0.5 day
- **Phase 6 (US3 - Features)**: 0.5 day
- **Phase 7 (US4 - Build System)**: 0.25 day
- **Phase 8 (US6 - Eval Readiness)**: 0.5 day
- **Phase 9 (Polish)**: 0.5-1 day

**Total Estimate**: 8-11 days (matches plan.md estimate)

---

## Validation Checkpoints

After each phase, verify:

- [ ] Code compiles without errors or warnings
- [ ] `norminette` passes on all modified files (or shows expected reduction in violations)
- [ ] No forbidden functions in modified files
- [ ] Memory leaks = 0 bytes (run valgrind/leaks on representative scenes)
- [ ] Test scenes render successfully
- [ ] Visual output matches baseline (pixel-diff <0.1% for key scenes)
- [ ] Changes committed with clear message

---

## Success Verification (Final Checklist)

Before marking refactoring complete, verify all success criteria from spec.md:

- [ ] **SC-001**: 100% of files pass norminette (run: `norminette src/ includes/ | grep Error` expects no output)
- [ ] **SC-002**: Zero forbidden functions (run: `grep -rn "memset\|memcpy\|memmove" src/ includes/` expects no matches)
- [ ] **SC-003**: Build succeeds with -Wall -Wextra -Werror producing zero warnings
- [ ] **SC-004**: Zero memory leaks on all test scenes (valgrind/leaks)
- [ ] **SC-005**: All 38 scenes render with pixel-diff <0.1% vs baseline
- [ ] **SC-006**: Makefile passes all target tests (all, clean, fclean, re, no-relink)
- [ ] **SC-007**: 100% mandatory features demonstrable
- [ ] **SC-008**: 100% evaluation checklist items verifiable
- [ ] **SC-009**: Handles 20+ error scenarios gracefully
- [ ] **SC-010**: Compilation time increase <20% from baseline
- [ ] **SC-011**: All existing functionality operational
- [ ] **SC-012**: Ready for 42 School peer evaluation

---

## Notes

- [P] tasks = different files/modules, no dependencies - can run in parallel
- [Story] label maps task to specific user story for traceability (US1-US6)
- Each user story should be independently completable and testable
- Commit after each task or logical group of tasks
- Stop at any checkpoint to validate independently
- Use `git tag` to mark validation milestones
- Refer to quickstart.md for detailed command examples
- Refer to contracts/refactoring-contracts.md for behavior specifications
- Tracking files in specs/016-compliance-refactoring/tracking/ provide audit trail

---

**Total Task Count**: 142 tasks
**User Story Breakdown**:
- Setup (Phase 1): 5 tasks
- Foundational (Phase 2): 5 tasks
- US5 - Deduplication (Phase 3): 9 tasks
- US2 - Forbidden Functions (Phase 4): 15 tasks
- US1 - Norminette (Phase 5): 64 tasks (largest - critical path)
- US3 - Feature Verification (Phase 6): 12 tasks
- US4 - Build System (Phase 7): 11 tasks
- US6 - Eval Readiness (Phase 8): 10 tasks
- Polish (Phase 9): 11 tasks

**Parallel Opportunities**: 50+ tasks marked [P] for parallel execution
**Independent Test Criteria**: Defined for each user story phase
**Suggested MVP Scope**: Phases 1-4 (Setup → Foundational → US5 → US2 → US1) = Core compliance

**Format Validation**: ✅ All tasks follow checklist format with checkbox, ID, optional [P] marker, [Story] label (where applicable), and file paths
