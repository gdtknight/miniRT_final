# Tasks: Debounced Rendering with Preview Mode

**Input**: Design documents from `/specs/018-debounced-rendering/`
**Prerequisites**: plan.md ✅, spec.md ✅, research.md ✅, data-model.md ✅, contracts/ ✅

**Tests**: All user stories include comprehensive testing tasks as requested.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project structure**: `src/`, `includes/`, `tests/` at repository root
- New files: `src/render/render_debounce*.c`, `includes/render_debounce.h`
- Modified files: `src/window/window_events.c`, `src/render/render.c`, `includes/window.h`

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and file structure for debounced rendering

- [X] T001 Create header file includes/render_debounce.h with type definitions and constants
- [X] T002 [P] Create source file src/render/render_debounce_timer.c for timer utilities
- [X] T003 [P] Create source file src/render/render_debounce.c for state machine implementation
- [X] T004 Update Makefile to include new debounce source files in compilation
- [ ] T005 Add debounce module documentation to docs/architecture.md

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core timer and state machine infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

### Timer Infrastructure

- [X] T006 [P] Implement timer_start() in src/render/render_debounce_timer.c
- [X] T007 [P] Implement timer_reset() in src/render/render_debounce_timer.c
- [X] T008 [P] Implement timer_stop() in src/render/render_debounce_timer.c
- [X] T009 Implement timer_expired() with microsecond precision in src/render/render_debounce_timer.c

### State Machine Core

- [X] T010 [P] Implement debounce_init() in src/render/render_debounce.c
- [X] T011 Implement debounce_on_input() state transition logic in src/render/render_debounce.c
- [X] T012 Implement debounce_update() state machine tick in src/render/render_debounce.c
- [X] T013 Implement debounce_cancel() for render abortion in src/render/render_debounce.c

### Integration Points Setup

- [X] T014 Add t_debounce_state field to t_render structure in includes/window.h
- [X] T015 Initialize debounce state in init_window() in src/window/window_init.c
- [X] T016 Add cancellation check in render_scene_to_buffer() scanline loop in src/render/render.c

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Smooth Camera Navigation (Priority: P1) 🎯 MVP

**Goal**: Eliminate stuttering during rapid WASD camera movements by deferring rendering until input pauses, then automatically showing preview→final renders

**Independent Test**: Hold down WASD keys for 2 seconds, then release. Success means no intermediate renders during key hold, and smooth preview→final transition after release.

### Tests for User Story 1

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [ ] T017 [P] [US1] Create manual test scene scenes/test_debounce_camera.rt for camera navigation testing
- [ ] T018 [P] [US1] Create test script tests/manual/test_camera_debounce.sh for automated validation
- [ ] T019 [P] [US1] Document test procedure in tests/manual/camera_navigation_test.md
- [ ] T020 [US1] Create timer validation test to verify 150ms delay accuracy in tests/manual/test_timer_accuracy.c
- [ ] T021 [US1] Create state machine validation test for IDLE→ACTIVE→PREVIEW→FINAL transitions in tests/manual/test_state_machine.c

### Implementation for User Story 1

- [ ] T022 [P] [US1] Integrate debounce_on_input() call in handle_camera_move() in src/window/window_key_handlers.c
- [X] T023 [P] [US1] Integrate debounce_on_input() call for W key handler in src/window/window_events.c
- [X] T024 [P] [US1] Integrate debounce_on_input() call for A key handler in src/window/window_events.c
- [X] T025 [P] [US1] Integrate debounce_on_input() call for S key handler in src/window/window_events.c
- [X] T026 [P] [US1] Integrate debounce_on_input() call for D key handler in src/window/window_events.c
- [X] T027 [US1] Integrate debounce_update() in render_loop() before render check in src/window/window_events.c
- [X] T028 [US1] Remove direct low_quality flag setting from camera key handlers in src/window/window_key_handlers.c
- [ ] T029 [US1] Verify cancel_requested flag clears after cancellation in debounce_cancel()
- [ ] T030 [US1] Add state transition validation logic in debounce_update()

### Edge Case Testing for User Story 1

- [ ] T031 [US1] Test continuous key hold for 10+ seconds (verify no memory issues)
- [ ] T032 [US1] Test rapid key press/release cycles under 100ms intervals
- [ ] T033 [US1] Test cancellation during preview render (mid-scanline abort)
- [ ] T034 [US1] Test cancellation during final render (mid-scanline abort)
- [ ] T035 [US1] Test debounce with delay_ms = 0 (legacy behavior mode)
- [ ] T036 [US1] Test timer overflow edge case with very long input pause
- [ ] T037 [US1] Verify no intermediate renders occur during 3-second continuous input

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Object Manipulation Without Lag (Priority: P2)

**Goal**: Extend debouncing to arrow key object manipulation, preventing render queue buildup during fine-tuned adjustments

**Independent Test**: Press arrow keys 10 times rapidly. Success means only one preview+final render cycle occurs after the last keypress.

### Tests for User Story 2

- [ ] T038 [P] [US2] Create manual test scene scenes/test_debounce_objects.rt with movable objects
- [ ] T039 [P] [US2] Create test script tests/manual/test_object_debounce.sh for arrow key validation
- [ ] T040 [US2] Document object manipulation test procedure in tests/manual/object_manipulation_test.md
- [ ] T041 [US2] Create test for rapid arrow key sequences (10+ presses in 500ms)

### Implementation for User Story 2

- [ ] T042 [P] [US2] Integrate debounce_on_input() call for UP arrow handler in src/window/window_events.c
- [ ] T043 [P] [US2] Integrate debounce_on_input() call for DOWN arrow handler in src/window/window_events.c
- [ ] T044 [P] [US2] Integrate debounce_on_input() call for LEFT arrow handler in src/window/window_events.c
- [ ] T045 [P] [US2] Integrate debounce_on_input() call for RIGHT arrow handler in src/window/window_events.c
- [ ] T046 [US2] Remove direct low_quality flag setting from object manipulation handlers in src/window/window_key_handlers.c
- [ ] T047 [US2] Verify object position updates correctly after preview render completes
- [ ] T048 [US2] Test state machine handles mixed camera+object input correctly

### Edge Case Testing for User Story 2

- [ ] T049 [US2] Test switching from object manipulation to camera movement mid-render
- [ ] T050 [US2] Test alternating arrow keys rapidly (UP→DOWN→UP→DOWN)
- [ ] T051 [US2] Verify object transformation state consistency after cancellation
- [ ] T052 [US2] Test object manipulation with zero delay (immediate render mode)

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - Quick Scene Exploration (Priority: P3)

**Goal**: Handle mixed interaction patterns intelligently - alternating between camera movements and viewing pauses with responsive preview feedback

**Independent Test**: Perform sequence: move camera → pause 1 second → move again → pause 1 second. Success means each pause shows preview→final sequence without overlap.

### Tests for User Story 3

- [ ] T053 [P] [US3] Create complex test scene scenes/test_debounce_mixed.rt for mixed interaction testing
- [ ] T054 [P] [US3] Create test script tests/manual/test_mixed_interaction.sh for complex scenarios
- [ ] T055 [US3] Document mixed interaction test procedure in tests/manual/mixed_interaction_test.md
- [ ] T056 [US3] Create test for start-stop movement patterns (100ms movements with 1s pauses)

### Implementation for User Story 3

- [ ] T057 [US3] Verify debounce handles camera→pause→camera sequences without state corruption
- [ ] T058 [US3] Verify debounce handles object→pause→camera sequences correctly
- [ ] T059 [US3] Test rapid final render cancellations (thrashing scenario protection)
- [ ] T060 [US3] Verify state machine prevents render overlap during quick transitions
- [ ] T061 [US3] Test preview→final transition interrupted by new input
- [ ] T062 [US3] Verify dirty flag synchronization across state transitions

### Edge Case Testing for User Story 3

- [ ] T063 [US3] Test debounce timer expiration during active render (race condition)
- [ ] T064 [US3] Test preview render completion exactly at moment of new input
- [ ] T065 [US3] Test state machine behavior when preview_enabled = 0 (direct to final)
- [ ] T066 [US3] Test state machine behavior when auto_upgrade = 0 (stay in preview)
- [ ] T067 [US3] Verify multiple sequential input types maintain timer correctly
- [ ] T068 [US3] Test extreme delay values (1ms, 1000ms, boundary conditions)

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: Performance Validation & Edge Cases

**Purpose**: Comprehensive performance testing and edge case verification across all user stories

### Performance Testing

- [ ] T069 [P] Measure and validate preview render starts within 200ms (SC-002 requirement)
- [ ] T070 [P] Measure and validate cancellation occurs within 16ms (SC-004 requirement)
- [ ] T071 [P] Verify timer_expired() overhead is under 100ns per call
- [ ] T072 Benchmark debounce_update() state machine overhead per frame
- [ ] T073 Test performance with complex scene (verify no slowdown from debouncing)
- [ ] T074 Measure memory footprint of t_debounce_state structure (should be ~48 bytes)

### Edge Case Validation

- [ ] T075 [P] Test timer arithmetic with microsecond overflow scenarios
- [ ] T076 [P] Test state transitions with all 12 state machine paths from data-model.md
- [ ] T077 Test behavior when debounce_on_input() called with NULL pointer (defensive)
- [ ] T078 Test behavior when debounce_update() called with invalid state enum
- [ ] T079 Test rapid state changes (input every 10ms for extended period)
- [ ] T080 Verify no memory leaks with valgrind during 1000 render cycles
- [ ] T081 Test norminette compliance on all new files (render_debounce*.c, render_debounce.h)

### Integration Testing

- [ ] T082 Test debounce with existing quality toggle key (Q key integration)
- [ ] T083 Verify debounce doesn't interfere with window close/minimize events
- [ ] T084 Test debounce behavior when window loses/gains focus
- [ ] T085 Verify render cancellation doesn't leave partial buffers
- [ ] T086 Test compatibility with existing low_quality render mode
- [ ] T087 Verify dirty flag state consistency across all scenarios

**Checkpoint**: All performance and edge case tests pass

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Documentation, cleanup, and final validation

### Documentation

- [ ] T088 [P] Create Korean specification in docs/specs/018-debounced-rendering/
- [ ] T089 [P] Update README.md with debounced rendering feature announcement
- [ ] T090 [P] Add code comments with Doxygen format to all debounce functions
- [ ] T091 Update function call hierarchy documentation with debounce integration points
- [ ] T092 Create visual state machine diagram for docs/architecture/debounce_states.png

### Code Quality

- [ ] T093 [P] Run norminette on all modified and new files
- [ ] T094 [P] Run valgrind memory leak check with test scenes
- [ ] T095 Verify 42 School constraints compliance (no pthread, fork, pipe)
- [ ] T096 Code review: Verify all timer operations use gettimeofday() correctly
- [ ] T097 Code review: Verify no malloc/free calls in debounce implementation
- [ ] T098 Refactor any magic numbers to named constants in render_debounce.h

### Final Validation

- [ ] T099 Run all quickstart.md test procedures (Tests 1-6)
- [ ] T100 Validate success criteria SC-001 through SC-005 from spec.md
- [ ] T101 Test on both Linux and macOS platforms
- [ ] T102 User acceptance testing with real scene navigation scenarios
- [ ] T103 Create before/after comparison video demonstrating stuttering elimination
- [ ] T104 Update CHANGELOG.md with feature details and breaking changes (if any)

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3-5)**: All depend on Foundational phase completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P2 → P3)
- **Performance Validation (Phase 6)**: Depends on all user stories being complete
- **Polish (Phase 7)**: Depends on Phase 6 validation passing

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - Independently testable, may share integration code with US1
- **User Story 3 (P3)**: Can start after Foundational (Phase 2) - Validates interaction between US1 and US2, but independently testable

### Within Each User Story

- Tests MUST be written and FAIL before implementation
- Integration points before handler modifications
- Handler modifications can be parallelized (different key handlers)
- Edge case tests after core implementation
- Story validation before moving to next priority

### Parallel Opportunities

- **Phase 1**: T002 and T003 (different source files)
- **Phase 2**: 
  - T006, T007, T008 (different timer functions)
  - T010 can run parallel with timer implementations
- **Phase 3 (US1)**: 
  - T017, T018, T019 (test preparation)
  - T023, T024, T025, T026 (different key handlers)
- **Phase 4 (US2)**: T042, T043, T044, T045 (different arrow key handlers)
- **Phase 6**: T069, T070, T071 (different performance metrics), T075, T076 (different edge cases)
- **Phase 7**: T088, T089, T090 (documentation), T093, T094 (quality checks)

- Once Foundational phase completes, User Stories 1, 2, and 3 can all be worked on in parallel by different team members

---

## Parallel Example: User Story 1

```bash
# Launch all test preparation tasks together:
Task: "Create manual test scene scenes/test_debounce_camera.rt"
Task: "Create test script tests/manual/test_camera_debounce.sh"
Task: "Document test procedure in tests/manual/camera_navigation_test.md"

# Launch all key handler integration tasks together:
Task: "Integrate debounce_on_input() for W key in src/window/window_events.c"
Task: "Integrate debounce_on_input() for A key in src/window/window_events.c"
Task: "Integrate debounce_on_input() for S key in src/window/window_events.c"
Task: "Integrate debounce_on_input() for D key in src/window/window_events.c"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup (T001-T005)
2. Complete Phase 2: Foundational (T006-T016) - CRITICAL - blocks all stories
3. Complete Phase 3: User Story 1 (T017-T037)
4. **STOP and VALIDATE**: Test User Story 1 independently with all acceptance scenarios
5. Run quickstart.md Tests 1-4 specifically for camera navigation
6. Deploy/demo if ready

### Incremental Delivery

1. **Foundation** (Setup + Foundational) → Core infrastructure ready
2. **MVP** (+ User Story 1) → Test independently → Deploy/Demo (camera navigation working!)
3. **Enhanced** (+ User Story 2) → Test independently → Deploy/Demo (object manipulation working!)
4. **Complete** (+ User Story 3) → Test independently → Deploy/Demo (mixed interactions validated!)
5. **Optimized** (+ Phase 6) → Performance validated → Ready for production
6. **Polished** (+ Phase 7) → Documentation complete → Release candidate

Each story adds value without breaking previous stories.

### Parallel Team Strategy

With multiple developers:

1. **Team completes Setup + Foundational together** (critical path)
2. **Once Foundational is done**:
   - Developer A: User Story 1 (T017-T037)
   - Developer B: User Story 2 (T038-T052)
   - Developer C: User Story 3 (T053-T068)
3. **After stories complete**:
   - Team collaborates on Phase 6 (Performance Validation)
   - Team collaborates on Phase 7 (Polish)

---

## Task Summary

| Phase | Task Count | Parallelizable | Critical Path |
|-------|-----------|----------------|---------------|
| Phase 1: Setup | 5 | 2 | Yes |
| Phase 2: Foundational | 11 | 4 | Yes (BLOCKING) |
| Phase 3: US1 (P1) | 21 | 9 | Yes (MVP) |
| Phase 4: US2 (P2) | 15 | 8 | No |
| Phase 5: US3 (P3) | 16 | 4 | No |
| Phase 6: Performance | 19 | 7 | Yes |
| Phase 7: Polish | 17 | 9 | Yes |
| **TOTAL** | **104 tasks** | **43 parallel** | **41.3%** |

**MVP Scope**: Phase 1 + Phase 2 + Phase 3 = **37 tasks** (35.6% of total)

**Independent Test Criteria**:
- **US1**: Hold WASD 2 seconds → no renders → release → preview+final in sequence
- **US2**: Press arrows 10 times fast → only one render cycle after last press
- **US3**: Move→pause→move→pause → each pause shows independent preview+final

**Suggested MVP Timeline**:
- Phase 1 (Setup): 1-2 hours
- Phase 2 (Foundational): 4-6 hours (CRITICAL - allocate sufficient time)
- Phase 3 (US1): 6-8 hours (includes all testing)
- **Total MVP**: 11-16 hours of development + testing

---

## Notes

- **[P] tasks**: Different files, no dependencies - can run in parallel
- **[Story] label**: Maps task to specific user story for traceability
- **Timer precision**: All timer implementations use gettimeofday() for microsecond precision
- **42 School compliance**: No pthread, fork, pipe, or external libraries - verified in T095
- **Memory safety**: Zero malloc/free in debounce code - verified in T097
- **Norminette**: All code must pass - verified in T093
- **Tests FIRST**: All test tasks must be completed and failing before implementation begins
- **Commit strategy**: Commit after each task or logical group (e.g., all key handlers)
- **Stop at any checkpoint**: Each user story checkpoint allows independent validation
- **Cancellation is critical**: Tasks T016, T029, T033, T034 ensure render abortion works correctly
- **State machine validation**: Task T021 validates all 12 state transitions from data-model.md
- **Performance targets**: 150ms delay, <200ms preview start, <16ms cancellation (validated in T069-T070)

---

## Configuration Notes

Default values (can be tuned):
- `DEBOUNCE_DEFAULT_DELAY_MS = 150` (optimal user experience per research.md)
- `DEBOUNCE_DEFAULT_PREVIEW = 1` (enable preview mode)
- `DEBOUNCE_DEFAULT_AUTO_UPGRADE = 1` (auto preview→final transition)

Set to `delay_ms = 0` to disable debouncing for debugging/comparison.

---

## Success Criteria Validation

**From spec.md** - must verify in final validation:

- ✅ **SC-001**: Users can hold navigation keys for any duration without intermediate renders (T037)
- ✅ **SC-002**: Preview rendering begins within 200ms after input stops (T069)
- ✅ **SC-003**: System automatically transitions from preview to final rendering (T030, T062)
- ✅ **SC-004**: New input cancels in-progress rendering within 16ms (T070)
- ✅ **SC-005**: Users experience smoother navigation with no buffering/stuttering (T099, T102)

All success criteria mapped to specific validation tasks in Phase 6 and Phase 7.
