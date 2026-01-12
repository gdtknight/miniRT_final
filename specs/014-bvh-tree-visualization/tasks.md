# Tasks: BVH Tree Visualization

**Input**: Design documents from `/specs/014-bvh-tree-visualization/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/visualization_api.md, quickstart.md

**Tests**: No test tasks included - feature specification does not explicitly request TDD approach

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: `src/`, `includes/`, `tests/` at repository root
- All paths are relative to /home/yoshin/work/miniRT

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure for visualization module

- [X] T001 Create new visualization module directory structure at src/bvh_vis/
- [X] T002 Create header file includes/bvh_vis.h with function prototypes and structure declarations
- [X] T003 [P] Update Makefile to include new bvh_vis source files in compilation
- [X] T004 [P] Add depth field (int depth) to t_bvh_node structure in includes/spatial.h
- [X] T005 [P] Add visualize flag (int visualize) to t_bvh structure in includes/spatial.h

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T006 Implement depth tracking during BVH construction in src/spatial/bvh_build_core.c (set root depth to 0)
- [X] T007 Propagate depth to children during node splitting in src/spatial/bvh_build_split.c (child depth = parent depth + 1)
- [X] T008 [P] Implement prefix buffer initialization function in src/bvh_vis/bvh_vis_prefix.c (initial capacity 40 chars)
- [X] T009 [P] Implement prefix_push function in src/bvh_vis/bvh_vis_prefix.c (adds branch characters to buffer)
- [X] T010 [P] Implement prefix_pop function in src/bvh_vis/bvh_vis_prefix.c (removes last prefix segment)
- [X] T011 [P] Implement prefix_print function in src/bvh_vis/bvh_vis_prefix.c (outputs current prefix to stdout)
- [X] T012 [P] Implement prefix_destroy function in src/bvh_vis/bvh_vis_prefix.c (frees buffer and resets state)
- [X] T013 [P] Parse --bvh-vis command-line flag in src/parser/ (set scene->bvh.visualize = 1 when present)

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Basic Tree Visualization (Priority: P1) 🎯 MVP

**Goal**: Display hierarchical BVH tree structure with ASCII art showing parent-child relationships

**Independent Test**: Run miniRT with any valid scene file and --bvh-vis flag, verify tree structure appears with correct ASCII formatting (├──, └──, │)

### Implementation for User Story 1

- [X] T014 [P] [US1] Implement format_node_info function in src/bvh_vis/bvh_vis_node.c (creates t_node_info structure for any node)
- [X] T015 [P] [US1] Implement is_leaf_node helper function in src/bvh_vis/bvh_vis_node.c (checks if node has no children)
- [X] T016 [P] [US1] Implement print_node_line function in src/bvh_vis/bvh_vis_print.c (outputs single node with prefix and tree characters)
- [X] T017 [US1] Implement recursive tree traversal function in src/bvh_vis/bvh_vis_tree.c (traverses tree with prefix management)
- [X] T018 [US1] Implement bvh_visualize_tree function in src/bvh_vis/bvh_vis_tree.c (entry point that calls traversal with initial state)
- [X] T019 [US1] Add visualization call in src/main.c after BVH construction (if scene->bvh.visualize is enabled)
- [X] T020 [US1] Test with scenes/simple.rt (3-5 objects) and verify correct tree hierarchy display

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Node Information Display (Priority: P2)

**Goal**: Display detailed information for each node (bounding boxes, depth, object IDs) and summary statistics

**Independent Test**: Run miniRT with known scene, verify each node shows type, bounds (2 decimal places), depth, and leaf nodes show object IDs; verify statistics summary appears after tree

### Implementation for User Story 2

- [X] T021 [P] [US2] Implement format_bounding_box function in src/bvh_vis/bvh_vis_node.c (formats bounds to 2 decimal places)
- [X] T022 [P] [US2] Implement format_object_list function in src/bvh_vis/bvh_vis_node.c (formats object IDs as comma-separated list)
- [X] T023 [P] [US2] Implement check_extreme_coordinates function in src/bvh_vis/bvh_vis_node.c (detects values needing scientific notation)
- [X] T024 [US2] Update format_node_info to include bounding box coordinates in src/bvh_vis/bvh_vis_node.c
- [X] T025 [US2] Update format_node_info to include object IDs for leaf nodes in src/bvh_vis/bvh_vis_node.c
- [X] T026 [P] [US2] Implement collect_stats_recursive function in src/bvh_vis/bvh_vis_stats.c (traverses tree accumulating metrics)
- [X] T027 [P] [US2] Implement calculate_derived_stats function in src/bvh_vis/bvh_vis_stats.c (computes avg objects per leaf)
- [X] T028 [P] [US2] Implement print_statistics_summary function in src/bvh_vis/bvh_vis_stats.c (outputs formatted statistics)
- [X] T029 [US2] Implement bvh_collect_statistics wrapper function in src/bvh_vis/bvh_vis_stats.c (public API entry point)
- [X] T030 [US2] Update bvh_visualize_tree to collect and display statistics after tree output in src/bvh_vis/bvh_vis_tree.c
- [X] T031 [US2] Test with scenes/medium.rt (10-20 objects) and verify all node information is displayed correctly

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - Controlled Output Options (Priority: P3)

**Goal**: Enable control over visualization output (depth limiting, compact mode, enable/disable flag) for large scenes

**Independent Test**: Run miniRT with different command-line flags and verify output adapts accordingly (disabled mode has zero overhead, compact mode reduces verbosity)

### Implementation for User Story 3

- [X] T032 [P] [US3] Implement bvh_vis_default_config function in src/bvh_vis/bvh_vis_init.c (returns default configuration structure)
- [X] T033 [P] [US3] Implement bvh_vis_get_terminal_width function in src/bvh_vis/bvh_vis_init.c (uses ioctl TIOCGWINSZ)
- [X] T034 [P] [US3] Implement check_edge_cases function in src/bvh_vis/bvh_vis_init.c (detects deep trees, narrow terminals)
- [X] T035 [P] [US3] Implement print_warning_message function in src/bvh_vis/bvh_vis_print.c (outputs warnings to stderr)
- [X] T036 [P] [US3] Implement format_node_compact function in src/bvh_vis/bvh_vis_node.c (abbreviated format for compact mode)
- [X] T037 [US3] Update bvh_visualize_tree to accept t_vis_config parameter in src/bvh_vis/bvh_vis_tree.c
- [X] T038 [US3] Add max_depth_display limit check in traversal function in src/bvh_vis/bvh_vis_tree.c
- [X] T039 [US3] Implement compact_mode switch in format_node_info in src/bvh_vis/bvh_vis_node.c
- [X] T040 [US3] Implement main bvh_visualize API function in src/bvh_vis/bvh_vis_init.c (entry point with config and validation)
- [X] T041 [US3] Update src/main.c to use new bvh_visualize API with configuration
- [X] T042 [US3] Test visualization disabled mode (without --bvh-vis flag) and verify zero performance overhead
- [X] T043 [US3] Test with very deep tree scene and verify warning messages appear
- [X] T044 [US3] Test with narrow terminal (resize to < 80 chars) and verify warning message

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [X] T045 [P] Run norminette on all new source files in src/bvh_vis/ and includes/bvh_vis.h
- [X] T046 [P] Run valgrind memory leak check with visualization enabled on test scenes
- [X] T047 [P] Verify build with make and ensure no compilation warnings or errors
- [X] T048 [P] Test with edge case scenes (1 object, 2 objects, 100 objects, empty scene)
- [X] T049 [P] Validate quickstart.md examples work as documented
- [X] T050 Code review and refactoring for consistency across bvh_vis module files
- [X] T051 Performance profiling to confirm visualization overhead < 1% of BVH construction time
- [ ] T052 [P] Create Korean translation of design documents in docs/specs/014-bvh-tree-visualization/

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3-5)**: All depend on Foundational phase completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P2 → P3)
- **Polish (Phase 6)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - Extends US1 but independently testable
- **User Story 3 (P3)**: Can start after Foundational (Phase 2) - Adds control options, independently testable

### Within Each User Story

- Models/structures before services
- Core implementation before integration
- Story complete before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel (T003, T004, T005)
- All Foundational tasks marked [P] can run in parallel within constraints (T008-T012 can run together, T013 independent)
- Once Foundational phase completes, all user stories can start in parallel (if team capacity allows)
- Within each user story, tasks marked [P] can run in parallel (e.g., T014, T015, T016 in US1)
- Different user stories can be worked on in parallel by different team members

---

## Parallel Example: User Story 1

```bash
# Launch all helper functions for User Story 1 together:
Task: "Implement format_node_info function in src/bvh_vis/bvh_vis_node.c"
Task: "Implement is_leaf_node helper function in src/bvh_vis/bvh_vis_node.c"
Task: "Implement print_node_line function in src/bvh_vis/bvh_vis_print.c"

# These can all be developed simultaneously as they work on different files
```

---

## Parallel Example: User Story 2

```bash
# Launch all formatting functions for User Story 2 together:
Task: "Implement format_bounding_box function in src/bvh_vis/bvh_vis_node.c"
Task: "Implement format_object_list function in src/bvh_vis/bvh_vis_node.c"
Task: "Implement check_extreme_coordinates function in src/bvh_vis/bvh_vis_node.c"

# Launch all statistics functions in parallel:
Task: "Implement collect_stats_recursive function in src/bvh_vis/bvh_vis_stats.c"
Task: "Implement calculate_derived_stats function in src/bvh_vis/bvh_vis_stats.c"
Task: "Implement print_statistics_summary function in src/bvh_vis/bvh_vis_stats.c"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1
4. **STOP and VALIDATE**: Test User Story 1 independently with multiple scenes
5. Deploy/demo if ready

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Deploy/Demo (MVP!)
3. Add User Story 2 → Test independently → Deploy/Demo
4. Add User Story 3 → Test independently → Deploy/Demo
5. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: User Story 1 (Basic tree visualization)
   - Developer B: User Story 2 (Node information and statistics)
   - Developer C: User Story 3 (Control options and edge cases)
3. Stories complete and integrate independently

---

## Summary

**Total Tasks**: 52
- **Setup (Phase 1)**: 5 tasks
- **Foundational (Phase 2)**: 8 tasks (CRITICAL blocking phase)
- **User Story 1 (Phase 3)**: 7 tasks (MVP)
- **User Story 2 (Phase 4)**: 11 tasks
- **User Story 3 (Phase 5)**: 13 tasks
- **Polish (Phase 6)**: 8 tasks

**Task Distribution by Story**:
- User Story 1 (Basic Tree Visualization): 7 tasks
- User Story 2 (Node Information Display): 11 tasks
- User Story 3 (Controlled Output Options): 13 tasks
- Infrastructure/Setup: 13 tasks
- Polish: 8 tasks

**Parallel Opportunities Identified**: 31 tasks marked [P] can run in parallel within their phase

**Independent Test Criteria**:
- US1: Tree structure displays with correct ASCII formatting
- US2: All node details and statistics show correctly
- US3: Output adapts to configuration options

**Suggested MVP Scope**: Phase 1 + Phase 2 + Phase 3 (User Story 1 only) = 20 tasks

**Format Validation**: ✅ All tasks follow checklist format (checkbox, ID, labels, file paths)

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- Avoid: vague tasks, same file conflicts, cross-story dependencies that break independence
- No test tasks included per workflow rules: Tests are optional and not explicitly requested in feature specification
