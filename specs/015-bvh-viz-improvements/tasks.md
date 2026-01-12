# Tasks: BVH Tree Visualization Improvements

**Feature**: 015-bvh-viz-improvements  
**Generated**: 2026-01-12  
**Input**: Design documents from `/specs/015-bvh-viz-improvements/`

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

**Tests**: Tests are NOT explicitly requested in the feature specification, so test tasks are omitted per agent instructions.

## Format: `- [ ] [ID] [P?] [Story?] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2)
- Include exact file paths in descriptions

## Path Conventions

This is a single C project with structure:
- Source: `src/` at repository root
- Headers: `includes/` at repository root
- Tests: `tests/` and `scenes/` at repository root

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project structure verification and build environment preparation

- [X] T001 Verify norminette is installed and working (run `norminette --version`)
- [X] T002 Verify project builds successfully with current structure (run `make`)
- [X] T003 [P] Create backup of existing HUD rendering code in `src/hud/hud_obj_render.c`
- [X] T004 [P] Create backup of existing BVH visualization code in `src/bvh_vis/bvh_vis_node.c`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

### Object Identifier Storage Foundation

- [X] T005 Add `char id[8]` field to `t_sphere` struct in `includes/objects.h`
- [X] T006 Add `char id[8]` field to `t_plane` struct in `includes/objects.h`
- [X] T007 Add `char id[8]` field to `t_cylinder` struct in `includes/objects.h`

### Object Type Enum Foundation

- [X] T008 Define or verify `t_obj_type` enum in `includes/objects.h` with values (OBJ_SPHERE, OBJ_PLANE, OBJ_CYLINDER)

### Identifier API Foundation

- [X] T009 Create header file `includes/format_object_id.h` with function declarations
- [ ] T010 Implement `format_object_id()` function in `src/utils/format_object_id.c`
- [ ] T011 Implement `get_object_type_prefix()` helper function in `src/utils/format_object_id.c`
- [ ] T012 Update `src/Makefile` or root `Makefile` to include `src/utils/format_object_id.c` in compilation

### Parser Integration Foundation

- [ ] T013 Add object ID generation for spheres during parsing in `src/parser/parse_objects.c` (use snprintf with format "sp-%d")
- [ ] T014 Add object ID generation for planes during parsing in `src/parser/parse_objects.c` (use snprintf with format "pl-%d")
- [ ] T015 Add object ID generation for cylinders during parsing in `src/parser/parse_objects.c` (use snprintf with format "cy-%d")
- [ ] T016 Add validation for >99 objects of same type in `src/parser/parse_validation.c` (fail scene load with clear error message)

### Build and Validation

- [ ] T017 Build project and fix any compilation errors (run `make`)
- [ ] T018 Run norminette on modified parser files and fix any norm violations
- [ ] T019 Run norminette on new utility files and fix any norm violations
- [ ] T020 Test parser with scene file containing multiple objects and verify IDs assigned correctly

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Unified Object Identification Format (Priority: P1) 🎯 MVP

**Goal**: Display consistent lowercase object identifiers (sp-1, pl-2, cy-3) in both HUD and BVH tree visualization, enabling immediate correlation between displays

**Independent Test**: Load a scene with multiple objects (3 spheres, 2 planes, 1 cylinder), view HUD and verify format is "sp-1, sp-2, sp-3, pl-1, pl-2, cy-1", then view BVH tree visualization and verify leaf nodes show matching identifiers

### HUD Implementation for User Story 1

- [ ] T021 [P] [US1] Modify `render_sphere_obj()` in `src/hud/hud_obj_render.c` to use `format_object_id()` instead of manual formatting
- [ ] T022 [P] [US1] Modify `render_plane_obj()` in `src/hud/hud_obj_render.c` to use `format_object_id()` instead of manual formatting
- [ ] T023 [P] [US1] Modify `render_cylinder_obj()` in `src/hud/hud_obj_render.c` to use `format_object_id()` instead of manual formatting

### BVH Visualization Implementation for User Story 1

- [ ] T024 [US1] Modify `format_object_list()` or equivalent function in `src/bvh_vis/bvh_vis_node.c` to use `format_object_id()` for displaying object identifiers in leaf nodes

### Build and Validation for User Story 1

- [ ] T025 [US1] Build project and verify no compilation errors (run `make`)
- [ ] T026 [US1] Run norminette on modified HUD files and fix any norm violations
- [ ] T027 [US1] Run norminette on modified BVH visualization files and fix any norm violations
- [ ] T028 [US1] Test with `scenes/test_scene.rt` and verify HUD shows unified format (sp-1, pl-2, cy-3)
- [ ] T029 [US1] Test with same scene and verify BVH tree leaf nodes show matching format
- [ ] T030 [US1] Verify object identifiers remain stable throughout navigation session
- [ ] T031 [US1] Run valgrind memory check and verify zero leaks (run `valgrind --leak-check=full ./miniRT scenes/test_scene.rt`)

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently - HUD and BVH show consistent identifiers

---

## Phase 4: User Story 2 - Smooth Keyboard Interaction (Priority: P2)

**Goal**: Implement 1-second debounce for all keyboard inputs to eliminate stuttering during rapid key presses, providing smooth interaction with final render showing accumulated changes

**Independent Test**: Rapidly press any keyboard keys (holding down for 3 seconds), observe no stuttering during keypresses, verify single smooth render occurs 1 second after stopping

### Debounce Data Structures for User Story 2

- [ ] T032 [P] [US2] Create header file `includes/debounce.h` with state machine enums (TIMER_IDLE, TIMER_PENDING, TIMER_RENDERING)
- [ ] T033 [P] [US2] Define `t_timer_slot` struct in `includes/debounce.h` (in_use, state, start_time, context, input_queue fields)
- [ ] T034 [P] [US2] Define `t_timer_pool` struct in `includes/debounce.h` (4 timer slots, active_count)
- [ ] T035 [P] [US2] Define `t_input_queue` struct in `includes/debounce.h` (keys[8], head, tail, count)
- [ ] T036 [US2] Add `t_timer_pool timer_pool` field to `t_render` struct in `includes/window.h`

### Timer Pool Implementation for User Story 2

- [ ] T037 [P] [US2] Create `src/debounce/debounce_init.c` and implement `timer_pool_init()` function
- [ ] T038 [P] [US2] Implement `timer_pool_acquire()` in `src/debounce/debounce_init.c` (linear search for free slot)
- [ ] T039 [P] [US2] Implement `timer_pool_release()` in `src/debounce/debounce_init.c`
- [ ] T040 [P] [US2] Implement `timer_pool_reset()` in `src/debounce/debounce_init.c` (for scene reload)
- [ ] T041 [P] [US2] Implement `timer_pool_get()` in `src/debounce/debounce_init.c` (retrieve slot by index)

### State Machine Implementation for User Story 2

- [ ] T042 [P] [US2] Create `src/debounce/debounce_state.c` and implement `get_timestamp_usec()` using gettimeofday()
- [ ] T043 [P] [US2] Implement `reset_timer()` in `src/debounce/debounce_state.c` (set start_time, increment accumulated)
- [ ] T044 [P] [US2] Implement `is_timer_expired()` in `src/debounce/debounce_state.c` (check if elapsed >= 1,000,000 microseconds)
- [ ] T045 [P] [US2] Implement `start_rendering()` in `src/debounce/debounce_state.c` (transition to RENDERING state)
- [ ] T046 [P] [US2] Implement `finish_rendering()` in `src/debounce/debounce_state.c` (transition to IDLE or PENDING based on queue status)

### Input Queue Implementation for User Story 2

- [ ] T047 [P] [US2] Create `src/debounce/debounce_queue.c` and implement `queue_init()` function
- [ ] T048 [P] [US2] Implement `queue_enqueue()` in `src/debounce/debounce_queue.c` (circular buffer, reject on overflow)
- [ ] T049 [P] [US2] Implement `queue_dequeue()` in `src/debounce/debounce_queue.c` (FIFO retrieval)
- [ ] T050 [P] [US2] Implement `queue_is_full()` in `src/debounce/debounce_queue.c`
- [ ] T051 [P] [US2] Implement `queue_is_empty()` in `src/debounce/debounce_queue.c`
- [ ] T052 [P] [US2] Implement `queue_clear()` in `src/debounce/debounce_queue.c` (for mouse action cancellation)
- [ ] T053 [US2] Implement `process_input_queue()` in `src/debounce/debounce_queue.c` (drain queue and apply inputs)

### Window Event Integration for User Story 2

- [ ] T054 [US2] Initialize timer pool in window initialization function in `src/window/handle_key.c` or equivalent init file
- [ ] T055 [US2] Modify keyboard handler in `src/window/handle_key.c` to route navigation inputs through debounce (check timer state, enqueue if RENDERING, reset timer if IDLE/PENDING)
- [ ] T056 [US2] Add timer expiration check in render loop in `src/window/handle_key.c` or render loop file (check is_timer_expired() and render->dirty flag)
- [ ] T057 [US2] Add render state transitions in render loop (call start_rendering() before render, finish_rendering() after)
- [ ] T058 [US2] Add input queue processing after render completes in render loop (call process_input_queue())

### Mouse and View Cancellation for User Story 2

- [ ] T059 [US2] Modify mouse handler in `src/window/handle_mouse.c` to cancel timer and clear queue on mouse actions (set state to IDLE, call queue_clear())
- [ ] T060 [US2] Prevent HUD/BVH view toggle during debounce in view toggle handler (check timer state, only allow toggle in IDLE state)

### Build and Validation for User Story 2

- [ ] T061 [US2] Update `src/Makefile` or root `Makefile` to include all debounce source files in compilation
- [ ] T062 [US2] Build project and fix any compilation errors (run `make`)
- [ ] T063 [US2] Run norminette on all debounce implementation files and fix violations
- [ ] T064 [US2] Run norminette on modified window event files and fix violations
- [ ] T065 [US2] Test single key press and verify 1-second delay before render
- [ ] T066 [US2] Test rapid key presses (10+ within 2 seconds) and verify single render after last press
- [ ] T067 [US2] Test holding arrow key and verify single render 1 second after release
- [ ] T068 [US2] Test keyboard input during complex render and verify inputs are queued and processed after render
- [ ] T069 [US2] Test mouse click during debounce and verify timer cancellation with queued inputs discarded
- [ ] T070 [US2] Test HUD/BVH toggle during debounce and verify toggle is prevented
- [ ] T071 [US2] Run valgrind memory check and verify zero leaks (run `valgrind --leak-check=full ./miniRT scenes/test_scene.rt`)

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently - consistent identifiers AND smooth keyboard interaction with debouncing

---

## Phase 5: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories, final validation, and edge case handling

### Progress Bar Visual Feedback (Optional Enhancement)

- [ ] T072 [P] Create `src/hud/hud_progress.c` with progress bar rendering function
- [ ] T073 [P] Implement `render_progress_bar()` in `src/hud/hud_progress.c` (bottom-left 200x20px, green fill based on elapsed time)
- [ ] T074 Add progress bar rendering call in render loop when timer state is PENDING

### Edge Case Handling

- [ ] T075 [P] Add error logging for timer pool overflow in `src/debounce/debounce_init.c` (when acquire fails)
- [ ] T076 [P] Add error logging for input queue overflow in `src/debounce/debounce_queue.c` (when enqueue fails)
- [ ] T077 Add state transition logging in `src/debounce/debounce_state.c` (log IDLE→PENDING, PENDING→RENDERING, etc.)

### Documentation and Scene Files

- [ ] T078 [P] Create test scene file `scenes/test_object_ids.rt` with multiple objects for identifier testing
- [ ] T079 [P] Create test scene file `scenes/test_debounce.rt` with complex scene for debounce timing testing
- [ ] T080 [P] Update project README or documentation with feature description and usage instructions

### Final Validation

- [ ] T081 Run full build with all warnings enabled (run `make`)
- [ ] T082 Run norminette on all modified and new files and ensure zero violations
- [ ] T083 Run valgrind on multiple test scenarios and ensure zero leaks
- [ ] T084 Test complete quickstart.md validation scenarios from `/specs/015-bvh-viz-improvements/quickstart.md`
- [ ] T085 Verify User Story 1 acceptance scenarios (HUD format matches BVH format for all object types)
- [ ] T086 Verify User Story 2 acceptance scenarios (debounce timing, input queuing, mouse cancellation)
- [ ] T087 Test scene with >99 objects of same type and verify clear error message
- [ ] T088 Test BVH construction failure scenario and verify object IDs preserved with clear error message

### Code Quality

- [ ] T089 [P] Review all new code for 25-line function limit compliance
- [ ] T090 [P] Review all new code for norminette naming conventions
- [ ] T091 Perform code walkthrough of state machine transitions and verify correctness
- [ ] T092 Review memory management and ensure no leaks in error paths

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Story 1 (Phase 3)**: Depends on Foundational phase completion
- **User Story 2 (Phase 4)**: Depends on Foundational phase completion (can start in parallel with US1 if staffed)
- **Polish (Phase 5)**: Depends on both User Stories 1 and 2 being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on User Story 2
- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - No dependencies on User Story 1

### Within Each User Story

**User Story 1**:
- HUD modifications (T021-T023) can all run in parallel (different functions in same file)
- BVH visualization (T024) is independent and can run parallel with HUD work
- Build and validation (T025-T031) must run after implementation complete

**User Story 2**:
- Data structure definitions (T032-T036) should complete first
- Timer pool, state machine, and input queue implementations (T037-T053) can run in parallel (different files)
- Window integration (T054-T060) must run after core implementations complete
- Build and validation (T061-T071) must run after integration complete

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] within same category can run in parallel
- Once Foundational phase completes:
  - User Story 1 and User Story 2 can be worked on in parallel by different team members
  - Within User Story 1: HUD modifications can run parallel with BVH modifications
  - Within User Story 2: Timer pool, state machine, and queue implementations can run in parallel

---

## Parallel Example: User Story 1

```bash
# Launch all HUD modifications together:
Task T021: "Modify render_sphere_obj() in src/hud/hud_obj_render.c"
Task T022: "Modify render_plane_obj() in src/hud/hud_obj_render.c"
Task T023: "Modify render_cylinder_obj() in src/hud/hud_obj_render.c"

# Simultaneously with BVH work:
Task T024: "Modify format_object_list() in src/bvh_vis/bvh_vis_node.c"
```

## Parallel Example: User Story 2

```bash
# Launch all data structure definitions together:
Task T032: "Create includes/debounce.h with state machine enums"
Task T033: "Define t_timer_slot struct in includes/debounce.h"
Task T034: "Define t_timer_pool struct in includes/debounce.h"
Task T035: "Define t_input_queue struct in includes/debounce.h"

# Launch all core implementations together (after structs defined):
Task T037: "Create src/debounce/debounce_init.c and implement timer_pool_init()"
Task T042: "Create src/debounce/debounce_state.c and implement get_timestamp_usec()"
Task T047: "Create src/debounce/debounce_queue.c and implement queue_init()"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup (T001-T004)
2. Complete Phase 2: Foundational (T005-T020) - CRITICAL foundation
3. Complete Phase 3: User Story 1 (T021-T031)
4. **STOP and VALIDATE**: Test User Story 1 independently with test scenes
5. Deploy/demo unified object identifiers feature if ready

**Estimated Effort**: ~3-5 hours for experienced developer

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready (~2 hours)
2. Add User Story 1 → Test independently → Deploy/Demo (MVP!) (~2 hours)
3. Add User Story 2 → Test independently → Deploy/Demo (~5 hours)
4. Add Polish → Final validation → Release (~2 hours)
5. Each story adds value without breaking previous stories

**Total Estimated Effort**: ~11-14 hours for complete feature

### Parallel Team Strategy

With 2 developers:

1. Both complete Setup + Foundational together (~2 hours)
2. Once Foundational is done:
   - Developer A: User Story 1 (T021-T031) - ~2 hours
   - Developer B: User Story 2 (T032-T071) - ~5 hours
3. Both work on Polish together (~2 hours)
4. Stories integrate cleanly due to independent design

**Total Time with 2 Developers**: ~7-9 hours (parallelism benefit)

---

## Notes

- **[P] tasks**: Different files or different functions in same file with no sequential dependencies
- **[Story] label**: Maps task to specific user story for traceability and independent implementation
- **File paths**: All paths are specific and absolute from repository root
- **42 School compliance**: All tasks respect norminette requirements, allowed functions only, 25-line limit
- **Validation frequency**: Build and norminette checks after each major milestone to catch issues early
- **Memory safety**: Valgrind checks included at critical points (after US1, after US2, final polish)
- **Edge cases**: Addressed in Polish phase (>99 objects, timer overflow, queue overflow, BVH failure)
- **Test files**: Scene files created in Polish phase for comprehensive testing
- **Independent testing**: Each user story has explicit test criteria and can be validated independently

---

## Task Count Summary

- **Total Tasks**: 92
- **Phase 1 (Setup)**: 4 tasks
- **Phase 2 (Foundational)**: 16 tasks (BLOCKING)
- **Phase 3 (User Story 1)**: 11 tasks
- **Phase 4 (User Story 2)**: 40 tasks
- **Phase 5 (Polish)**: 21 tasks
- **Parallel Opportunities**: 52 tasks marked [P] (can run concurrently with others)

---

## Success Criteria Validation

From spec.md, these success criteria will be validated:

- **SC-001**: Users can identify same object in both HUD and BVH tree without mental conversion ✓ (Validated in T028-T030)
- **SC-002**: Rapid keyboard input results in only one rendering operation after input stops ✓ (Validated in T065-T067)
- **SC-003**: Visual stuttering eliminated with smooth perceived performance ✓ (Validated in T065-T071)
- **SC-004**: Users can interact using any keyboard controls with responsive feedback ✓ (Validated in T065-T071)
- **SC-005**: Debugging time for correlating objects reduces by at least 50% ✓ (Achieved through consistent format in US1)

---

**Implementation Ready**: All tasks are specific, executable, and have clear dependencies. Each user story can be implemented and tested independently.
