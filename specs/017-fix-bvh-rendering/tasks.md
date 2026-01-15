# Tasks: Fix BVH Integration for Scene Rendering

**Input**: Design documents from `/specs/017-fix-bvh-rendering/`  
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `- [ ] [ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

---

## Phase 1: Setup & Diagnosis

**Purpose**: Establish baseline and diagnose current BVH integration issues

- [X] T001 Verify project builds successfully with `make re` from repository root
- [ ] T002 [P] Run test suite with `./test_miniRT.sh` to establish baseline test results
- [X] T003 [P] Document current BVH initialization sequence by tracing code flow from `main()` through `scene_build_bvh()`
- [ ] T004 Add debug logging to `src/render/trace.c` to log BVH status (enabled, root pointer, intersection result)
- [ ] T005 Test current implementation with `scenes/simple_sphere.rt` and confirm black screen issue

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core BVH infrastructure fixes that MUST be complete before ANY user story can be validated

**⚠️ CRITICAL**: No user story work can proceed until this phase is complete

- [X] T006 Fix BVH default initialization in `src/render/render_state.c` line 37: change `bvh_enabled = 0` to `bvh_enabled = 1`
- [X] T007 Fix trace_ray() early return logic in `src/render/trace.c` lines 159-164: remove early black return, set `hit_found` flag instead
- [X] T008 Add fallback logic wrapper in `src/render/trace.c` lines 165-173: wrap non-BVH checks in `if (!hit_found)` condition
- [X] T009 Verify norminette compliance for modified files: `norminette src/render/render_state.c src/render/trace.c`
- [X] T010 [P] Rebuild project with `make re` and verify no compilation warnings or errors

**Checkpoint**: Foundation ready - BVH should now initialize and fall back correctly

---

## Phase 3: User Story 1 - Basic Scene Rendering with BVH (Priority: P1) 🎯 MVP

**Goal**: Users can load and render simple scenes correctly with BVH enabled, seeing rendered objects instead of black screens

**Independent Test**: Load `scenes/simple_sphere.rt`, verify sphere renders with correct color and lighting, HUD shows "BVH: Enabled"

### Implementation for User Story 1

- [X] T011 [US1] Test basic scene rendering with single sphere in `scenes/simple_sphere.rt`
- [X] T012 [US1] Verify HUD displays "BVH: Enabled" status at application startup
- [X] T013 [US1] Capture baseline render output with BVH enabled (screenshot or pixel data)
- [X] T014 [US1] Manually toggle BVH off (or modify code temporarily to set `bvh->enabled = 0`)
- [X] T015 [US1] Render same scene with BVH disabled and compare output to baseline
- [X] T016 [US1] Verify pixel-identical output between BVH and non-BVH modes (visual inspection or automated diff)
- [X] T017 [US1] Test with multi-object scene containing sphere, plane, and cylinder from `scenes/` directory
- [X] T018 [US1] Verify all object types render correctly with proper lighting and shading
- [X] T019 [US1] Confirm HUD and key guide overlays display correctly alongside rendered scene
- [X] T020 [US1] Check debug logs confirm BVH path is taken and intersections found

**Checkpoint**: At this point, User Story 1 should be fully functional - basic scenes render correctly with BVH enabled

---

## Phase 4: User Story 2 - Performance Improvement with Complex Scenes (Priority: P2)

**Goal**: BVH acceleration provides measurable performance improvement on complex scenes without changing visual output

**Independent Test**: Create/load scene with 50+ objects, measure frame time with BVH disabled, enable BVH and verify 2x+ speedup with identical output

### Performance Testing for User Story 2

- [ ] T021 [US2] Identify or create test scene with 50-100 geometric objects in `scenes/many_objects.rt`
- [ ] T022 [US2] Modify code to temporarily disable BVH (set `bvh->enabled = 0`) and measure baseline frame time
- [ ] T023 [US2] Record baseline metrics: frame time (ms), ray count, render time from HUD
- [ ] T024 [US2] Re-enable BVH and measure frame time with acceleration enabled
- [ ] T025 [US2] Calculate speedup ratio: baseline_time / bvh_time (target: >= 2.0x)
- [ ] T026 [US2] Verify visual output is identical between BVH and non-BVH renders (screenshot comparison)
- [ ] T027 [US2] Test performance with different camera angles and positions to verify consistent speedup
- [ ] T028 [US2] Document performance results in `specs/017-fix-bvh-rendering/performance_results.md`

### BVH Statistics Implementation for User Story 2

- [ ] T029 [P] [US2] Add BVH node count display to HUD in `src/hud/hud_performance.c`
- [ ] T030 [P] [US2] Add BVH max depth display to HUD in `src/hud/hud_performance.c`
- [ ] T031 [US2] Verify BVH statistics update correctly when scene loads

**Checkpoint**: At this point, User Stories 1 AND 2 should both work - basic rendering works and performance improvement is measurable

---

## Phase 5: User Story 3 - Fallback and Error Handling (Priority: P3)

**Goal**: System gracefully falls back to non-BVH rendering when BVH is unavailable or fails, ensuring rendering always works

**Independent Test**: Manually disable BVH initialization or corrupt BVH pointer, verify scene still renders using fallback path with appropriate log messages

### Fallback Testing for User Story 3

- [ ] T032 [US3] Test empty scene (0 objects) with BVH enabled - verify no crash, black screen expected
- [ ] T033 [US3] Test single-object scene with BVH enabled - verify BVH builds and renders correctly
- [ ] T034 [US3] Simulate BVH initialization failure by modifying `scene_build_bvh()` to return early
- [ ] T035 [US3] Verify rendering falls back to brute-force path when BVH initialization fails
- [ ] T036 [US3] Check debug logs show appropriate fallback messages when BVH unavailable
- [ ] T037 [US3] Test with `bvh_enabled` flag set to 0 - verify non-BVH path works correctly
- [ ] T038 [US3] Simulate BVH tree corruption (set `bvh->root = NULL` after initialization)
- [ ] T039 [US3] Verify fallback path handles NULL root gracefully without crashes
- [ ] T040 [US3] Test scene where all objects are outside camera view - verify both paths return black correctly
- [ ] T041 [US3] Verify BVH disabled mode produces identical results to pre-BVH implementation

### Error Handling Validation for User Story 3

- [ ] T042 [P] [US3] Add comprehensive NULL pointer checks in `src/spatial/bvh_traverse.c` if not present
- [ ] T043 [P] [US3] Add error logging for BVH initialization failures in `src/spatial/bvh_init.c`
- [ ] T044 [US3] Document fallback behavior and error messages in `specs/017-fix-bvh-rendering/quickstart.md`

**Checkpoint**: All user stories should now be independently functional - rendering works in all scenarios with proper fallbacks

---

## Phase 6: Validation & Quality Assurance

**Purpose**: Comprehensive validation across all user stories and quality gates

### Memory Safety Validation

- [ ] T045 Run valgrind memory leak check on simple scene: `valgrind --leak-check=full ./miniRT scenes/simple_sphere.rt`
- [ ] T046 Verify zero memory leaks reported in valgrind summary (0 bytes definitely lost)
- [ ] T047 [P] Run valgrind on complex scene with 50+ objects to verify BVH memory management
- [ ] T048 [P] Test memory cleanup by loading multiple scenes sequentially and verifying no accumulation

### Code Quality Validation

- [ ] T049 Run norminette on all modified files: `norminette src/render/render_state.c src/render/trace.c`
- [ ] T050 Verify zero norminette errors or warnings
- [ ] T051 [P] Check for compilation warnings with `make re` and verify clean build
- [ ] T052 [P] Review code changes for adherence to 42 School function usage constraints

### Integration Testing

- [ ] T053 Test with all scene files in `scenes/` directory to verify no regressions
- [ ] T054 Run full test suite with `./test_miniRT.sh` and verify all tests pass
- [ ] T055 Compare test results to Phase 1 baseline - ensure no new failures
- [ ] T056 Test HUD performance metrics display correctly with various scene complexities
- [ ] T057 Verify BVH toggle functionality if implemented (press 'B' key handler)

### Performance Validation

- [ ] T058 Benchmark small scenes (1-5 objects) - verify BVH doesn't slow down simple cases significantly
- [ ] T059 Benchmark medium scenes (10-30 objects) - verify 2-5x speedup
- [ ] T060 Benchmark large scenes (50-100 objects) - verify 5-10x speedup
- [ ] T061 Document all performance measurements in `specs/017-fix-bvh-rendering/performance_results.md`

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Final improvements and documentation updates

### Documentation Updates

- [ ] T062 [P] Update quickstart.md with final BVH usage instructions
- [ ] T063 [P] Create troubleshooting section in quickstart.md for common BVH issues
- [ ] T064 [P] Document BVH performance characteristics for different scene sizes
- [ ] T065 [P] Update Korean documentation in `docs/specs/` to match English version
- [ ] T066 Add BVH feature status to main README.md if applicable

### Code Cleanup

- [ ] T067 Remove temporary debug logging added in T004 if not needed for production
- [ ] T068 [P] Review code comments in modified files for accuracy and clarity
- [ ] T069 [P] Ensure consistent code style across all BVH-related files

### Final Validation

- [ ] T070 Run complete quickstart.md verification workflow from start to finish
- [ ] T071 Create before/after performance comparison chart or table
- [ ] T072 Verify all success criteria from spec.md are met (SC-001 through SC-006)
- [ ] T073 Tag release or create pull request with feature branch `017-fix-bvh-rendering`

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup & Diagnosis (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Phase 1 diagnosis - BLOCKS all user stories
- **User Stories (Phase 3-5)**: All depend on Foundational phase completion
  - User Story 1 (P1): Can start after Phase 2 - No dependencies on other stories
  - User Story 2 (P2): Can start after Phase 2 AND US1 baseline established
  - User Story 3 (P3): Can start after Phase 2 - Independent of US1/US2
- **Validation (Phase 6)**: Depends on all user stories being complete
- **Polish (Phase 7)**: Depends on successful validation

### User Story Dependencies

- **User Story 1 (P1)**: Foundation Phase 2 complete → Independent testing possible
- **User Story 2 (P2)**: US1 baseline needed for comparison → Tests performance improvement
- **User Story 3 (P3)**: Foundation Phase 2 complete → Independent testing possible

### Within Each User Story

- Diagnosis before fixes (Phase 1 → Phase 2)
- Core fixes before testing (Phase 2 → Phase 3)
- Baseline renders before comparisons (T011-T013 before T014-T016)
- BVH disabled tests before BVH enabled tests for comparisons
- Performance baselines before speedup measurements (T022-T023 before T024-T025)

### Parallel Opportunities

- Phase 1: T002, T003 can run in parallel (different testing approaches)
- Phase 2: T010 can run in parallel with T009 (different validation)
- User Story 1: T011-T012 can run in parallel (different verification aspects)
- User Story 2: T029, T030 can run in parallel (different HUD additions)
- User Story 3: T032-T033 can run in parallel (different edge cases), T042-T043 can run in parallel (different files)
- Phase 6: T047-T048 can run in parallel (different memory tests), T051-T052 can run in parallel (different quality checks)
- Phase 7: T062-T066 can run in parallel (different documentation files)

---

## Parallel Example: User Story 1

```bash
# Parallel verification tasks for basic rendering:
Task T011: "Test basic scene rendering with single sphere in scenes/simple_sphere.rt"
Task T012: "Verify HUD displays BVH: Enabled status at application startup"

# These can run simultaneously as they verify different aspects
```

---

## Parallel Example: User Story 2

```bash
# Parallel HUD enhancement tasks:
Task T029: "Add BVH node count display to HUD in src/hud/hud_performance.c"
Task T030: "Add BVH max depth display to HUD in src/hud/hud_performance.c"

# These modify different parts of same file but add independent features
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup & Diagnosis → Understand the problem
2. Complete Phase 2: Foundational Fixes → Fix the root cause
3. Complete Phase 3: User Story 1 → Validate basic rendering works
4. **STOP and VALIDATE**: Test independently with simple scenes
5. Run memory checks and norminette
6. Deploy/demo if ready

### Incremental Delivery

1. Complete Phase 1-2 → Foundation fixed
2. Add User Story 1 (Phase 3) → Test independently → Basic rendering works (MVP!)
3. Add User Story 2 (Phase 4) → Test independently → Performance validated
4. Add User Story 3 (Phase 5) → Test independently → Robustness confirmed
5. Phase 6 Validation → Comprehensive quality assurance
6. Phase 7 Polish → Production ready
7. Each story adds value without breaking previous stories

### Single Developer Strategy

**Week 1: Core Fix**
- Day 1: Phase 1 (Diagnosis) - Understand the problem
- Day 2: Phase 2 (Foundation) - Implement core fixes
- Day 3: Phase 3 (US1) - Validate basic rendering
- Day 4: Phase 4 (US2) - Performance testing
- Day 5: Phase 5 (US3) - Edge cases and fallbacks

**Week 2: Quality & Documentation**
- Day 1-2: Phase 6 (Validation) - Comprehensive testing
- Day 3: Phase 7 (Polish) - Documentation and cleanup
- Day 4-5: Final validation and release preparation

---

## Success Criteria Tracking

Map tasks to success criteria from spec.md:

### SC-001: 100% scene rendering success rate
- **Validated by**: T011, T017-T018, T053 (test multiple scene files)
- **Status**: Pass when all scenes render without black screens

### SC-002: Pixel-identical output BVH vs non-BVH
- **Validated by**: T015-T016, T026 (output comparison)
- **Status**: Pass when visual diff shows zero differences

### SC-003: 2x+ speedup on complex scenes
- **Validated by**: T025, T059-T060 (performance benchmarks)
- **Status**: Pass when speedup ratio >= 2.0x for 50+ object scenes

### SC-004: 100% uptime regardless of BVH state
- **Validated by**: T032-T041 (fallback testing)
- **Status**: Pass when rendering succeeds in all BVH states

### SC-005: Clear BVH status in logs/HUD
- **Validated by**: T012, T020, T031, T036 (logging and display)
- **Status**: Pass when BVH state is visible and accurate

### SC-006: BVH toggle within 1 second (if implemented)
- **Validated by**: T057 (toggle functionality)
- **Status**: Optional - pass if toggle implemented and responsive

---

## Notes

- **Total Tasks**: 73 tasks across 7 phases
- **Critical Path**: Phase 1 → Phase 2 → Phase 3 (US1) → Validation
- **MVP Scope**: Phases 1-3 only (Tasks T001-T020) = 20 tasks for basic fix
- **Parallel Opportunities**: 17 tasks marked [P] can run in parallel with other tasks
- **User Story Distribution**:
  - US1 (Basic Rendering): 10 tasks (T011-T020)
  - US2 (Performance): 11 tasks (T021-T031)
  - US3 (Fallback): 13 tasks (T032-T044)
- **Format Validation**: All tasks follow `- [ ] [TID] [P?] [Story?] Description with file path` format
- Each user story is independently testable with clear checkpoints
- Commit after completing each phase or logical task group
- Stop at any checkpoint to validate story independently
- Debug logging can be kept if useful, or removed in Phase 7 cleanup
