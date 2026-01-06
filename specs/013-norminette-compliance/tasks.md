---
description: "Complete Norminette Compliance - Eliminate All 77 Errors"
---

# Tasks: Complete Norminette Compliance (0/77 Errors)

**Input**: Design documents from `/specs/013-norminette-compliance/`
**Prerequisites**: plan.md, spec.md, research.md
**Current Status**: 77 norminette errors across 18 files
**Target**: 0 norminette errors with 100% compliance

**Organization**: Tasks are grouped by user story priority to enable independent implementation and testing of each compliance category.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1-US5 from spec.md)
- Include exact file paths in descriptions

---

## Phase 1: Setup & Baseline Establishment

**Purpose**: Establish validation infrastructure and baseline measurements before refactoring

- [ ] T001 Establish norminette error baseline by running `norminette src/ includes/ 2>&1 | tee norminette_baseline.txt`
- [ ] T002 Count baseline errors: `grep "Error:" norminette_baseline.txt | wc -l` (should be 77)
- [ ] T003 Verify clean build with `make clean && make` and document exit code
- [ ] T004 [P] Create baseline_renders directory and render all test scenes for validation
- [ ] T005 [P] Measure performance baseline with `time ./miniRT scenes/complex_scene.rt > /dev/null`
- [ ] T006 [P] Create validation script scripts/validate_norminette.sh for incremental checking
- [ ] T007 [P] Create validation script scripts/compare_renders.sh for image diff validation
- [ ] T008 Document baseline metrics (77 errors, build time, render checksums) in BASELINE.md

---

## Phase 2: User Story 1 - File Function Count Compliance (Priority: P1) 🎯 MVP

**Goal**: Eliminate all 53 TOO_MANY_FUNCS errors by splitting files with >5 functions into logically organized files with ≤5 functions each

**Independent Test**: Run `norminette src/ includes/ | grep TOO_MANY_FUNCS | wc -l` should output 0

**Files Requiring Split**:
- hud_render.c (13 functions) → 3 files
- metrics.c (8 functions) → 3 files
- window.c (8 functions) → 3 files
- timer.c (7 functions) → 2 files
- aabb.c (7 functions) → 2 files
- bvh_build.c (7 functions) → 2 files
- hud_format.c (6 functions) → 2 files
- hud_transparency.c (6 functions) → 2 files
- render_state.c (5 functions) → needs extraction after other fixes
- render.c (6 functions) → needs extraction after other fixes
- hud_navigation.c (6 functions) → 2 files

### Split hud_render.c (13 functions → 3 files)

- [ ] T009 [US1] Create src/ui/hud/hud_text.c and includes/ui/hud/hud_text.h for text rendering functions
- [ ] T010 [US1] Create src/ui/hud/hud_metrics.c and includes/ui/hud/hud_metrics.h for metrics display functions
- [ ] T011 [US1] Create src/ui/hud/hud_controls.c and includes/ui/hud/hud_controls.h for control hints functions
- [ ] T012 [US1] Move text rendering functions (render_text_line, draw_text_background) to hud_text.c
- [ ] T013 [US1] Move metrics display functions (render_metrics_section, format_metrics_values) to hud_metrics.c
- [ ] T014 [US1] Move control hint functions (render_controls_section, format_key_hint) to hud_controls.c
- [ ] T015 [US1] Update includes in src/ui/hud_render.c to reference new headers
- [ ] T016 [US1] Update Makefile SRCS_UI section to include new hud/ files
- [ ] T017 [US1] Verify: `norminette src/ui/hud/ | grep "TOO_MANY_FUNCS"` outputs nothing
- [ ] T018 [US1] Build test: `make clean && make` succeeds

### Split metrics.c (8 functions → 3 files)

- [ ] T019 [P] [US1] Create src/core/metrics_tracking.c and includes/metrics_tracking.h
- [ ] T020 [P] [US1] Create src/core/metrics_display.c and includes/metrics_display.h
- [ ] T021 [P] [US1] Create src/core/metrics_fps.c and includes/metrics_fps.h
- [ ] T022 [US1] Move init_metrics, update_metrics, reset_metrics to metrics_tracking.c
- [ ] T023 [US1] Move render_metrics_overlay, format_metrics_text to metrics_display.c
- [ ] T024 [US1] Move calculate_fps, smooth_fps, get_frame_time to metrics_fps.c
- [ ] T025 [US1] Update src/core/metrics_calc.c includes to reference new headers
- [ ] T026 [US1] Update Makefile SRCS_CORE section to replace metrics.c with 3 new files
- [ ] T027 [US1] Verify: `norminette src/core/metrics*.c` reports 0 TOO_MANY_FUNCS errors
- [ ] T028 [US1] Build test: `make clean && make` succeeds

### Split window.c (8 functions → 3 files)

- [ ] T029 [P] [US1] Create src/ui/window_init.c and includes/window_init.h
- [ ] T030 [P] [US1] Create src/ui/window_events.c and includes/window_events.h
- [ ] T031 [P] [US1] Create src/ui/window_hooks.c and includes/window_hooks.h
- [ ] T032 [US1] Move create_window, init_mlx_connection, setup_window_properties, allocate_image_buffer to window_init.c
- [ ] T033 [US1] Move handle_key_press, handle_key_release, handle_mouse_event to window_events.c
- [ ] T034 [US1] Move register_mlx_hooks, setup_event_handlers to window_hooks.c
- [ ] T035 [US1] Update main.c includes to reference new window headers
- [ ] T036 [US1] Update Makefile SRCS_UI section to replace window.c with 3 new files
- [ ] T037 [US1] Verify: `norminette src/ui/window*.c` reports 0 TOO_MANY_FUNCS errors
- [ ] T038 [US1] Build test: `make clean && make` succeeds

### Split timer.c (7 functions → 2 files)

- [ ] T039 [P] [US1] Create src/core/timer_control.c and includes/timer_control.h
- [ ] T040 [P] [US1] Create src/core/timer_metrics.c and includes/timer_metrics.h
- [ ] T041 [US1] Move timer_start, timer_pause, timer_resume, timer_reset to timer_control.c
- [ ] T042 [US1] Move timer_get_elapsed, timer_get_fps, timer_update to timer_metrics.c
- [ ] T043 [US1] Update render.c includes to reference new timer headers
- [ ] T044 [US1] Update Makefile SRCS_CORE section to replace timer.c with 2 new files
- [ ] T045 [US1] Verify: `norminette src/core/timer*.c` reports 0 TOO_MANY_FUNCS errors

### Split aabb.c (7 functions → 2 files)

- [ ] T046 [P] [US1] Create src/bvh/aabb_basic.c and includes/aabb_basic.h
- [ ] T047 [P] [US1] Create src/bvh/aabb_advanced.c and includes/aabb_advanced.h
- [ ] T048 [US1] Move aabb_create, aabb_union, aabb_contains_point, aabb_surface_area to aabb_basic.c
- [ ] T049 [US1] Move aabb_intersect_ray, aabb_expand, aabb_merge to aabb_advanced.c
- [ ] T050 [US1] Update bvh_build.c and bvh_traverse.c includes to reference new aabb headers
- [ ] T051 [US1] Update Makefile SRCS_BVH section to replace aabb.c with 2 new files
- [ ] T052 [US1] Verify: `norminette src/bvh/aabb*.c` reports 0 TOO_MANY_FUNCS errors

### Split bvh_build.c (7 functions → 2 files)

- [ ] T053 [P] [US1] Create src/bvh/bvh_build_core.c and includes/bvh_build_core.h
- [ ] T054 [P] [US1] Create src/bvh/bvh_build_helpers.c and includes/bvh_build_helpers.h
- [ ] T055 [US1] Move build_bvh, build_bvh_recursive, partition_objects to bvh_build_core.c
- [ ] T056 [US1] Move calculate_partition_cost, find_best_split, choose_split_axis to bvh_build_helpers.c
- [ ] T057 [US1] Update bvh_init.c includes to reference new bvh_build headers
- [ ] T058 [US1] Update Makefile SRCS_BVH section to replace bvh_build.c with 2 new files
- [ ] T059 [US1] Verify: `norminette src/bvh/bvh_build*.c` reports 0 TOO_MANY_FUNCS errors

### Split hud_format.c (6 functions → 2 files)

- [ ] T060 [P] [US1] Create src/ui/hud/hud_format_numbers.c and includes/ui/hud/hud_format_numbers.h
- [ ] T061 [P] [US1] Create src/ui/hud/hud_format_strings.c and includes/ui/hud/hud_format_strings.h
- [ ] T062 [US1] Move format_int, format_float, format_percentage to hud_format_numbers.c
- [ ] T063 [US1] Move format_string, format_label, pad_string to hud_format_strings.c
- [ ] T064 [US1] Update hud_render.c includes to reference new format headers
- [ ] T065 [US1] Update Makefile SRCS_UI section to replace hud_format.c with 2 new files
- [ ] T066 [US1] Verify: `norminette src/ui/hud/hud_format*.c` reports 0 TOO_MANY_FUNCS errors

### Split hud_transparency.c (6 functions → 2 files)

- [ ] T067 [P] [US1] Create src/ui/hud/hud_alpha.c and includes/ui/hud/hud_alpha.h
- [ ] T068 [P] [US1] Create src/ui/hud/hud_blend.c and includes/ui/hud/hud_blend.h
- [ ] T069 [US1] Move calculate_alpha, set_transparency_level, get_transparency to hud_alpha.c
- [ ] T070 [US1] Move blend_colors, apply_alpha_blend, composite_layers to hud_blend.c
- [ ] T071 [US1] Update hud_render.c includes to reference new transparency headers
- [ ] T072 [US1] Update Makefile SRCS_UI section to replace hud_transparency.c with 2 new files
- [ ] T073 [US1] Verify: `norminette src/ui/hud/hud_transparency*.c` reports 0 TOO_MANY_FUNCS errors

### Split hud_navigation.c (6 functions → 2 files)

- [ ] T074 [P] [US1] Create src/ui/hud/hud_nav_state.c and includes/ui/hud/hud_nav_state.h
- [ ] T075 [P] [US1] Create src/ui/hud/hud_nav_render.c and includes/ui/hud/hud_nav_render.h
- [ ] T076 [US1] Move nav_init, nav_update, nav_get_current to hud_nav_state.c
- [ ] T077 [US1] Move nav_render_indicator, nav_render_menu, nav_highlight_item to hud_nav_render.c
- [ ] T078 [US1] Update hud_render.c includes to reference new navigation headers
- [ ] T079 [US1] Update Makefile SRCS_UI section to replace hud_navigation.c with 2 new files
- [ ] T080 [US1] Verify: `norminette src/ui/hud/hud_navigation*.c` reports 0 TOO_MANY_FUNCS errors

### Phase 2 Checkpoint - US1 Complete

- [ ] T081 [US1] Run full norminette check: `norminette src/ includes/ | grep TOO_MANY_FUNCS | wc -l` should be 0
- [ ] T082 [US1] Full build verification: `make clean && make && make bonus` all succeed
- [ ] T083 [US1] Render all test scenes and compare with baseline: `./scripts/compare_renders.sh`
- [ ] T084 [US1] Verify exit codes: All scenes exit with code 0
- [ ] T085 [US1] Document progress: Update norminette error count from 77 to remaining count

**Checkpoint**: File function count compliance achieved (53 errors eliminated)

---

## Phase 3: User Story 2 - Function Length Compliance (Priority: P2)

**Goal**: Eliminate all TOO_MANY_LINES errors by extracting long functions (>25 lines) into helper functions

**Independent Test**: Run `norminette src/ | grep TOO_MANY_LINES | wc -l` should output 0

**Functions Requiring Extraction**:
- main.c line 79 (main function)
- render.c line 99 (render_scene function)
- window.c line 273, 415, 519 (3 functions)
- parse_elements.c line 80 (parse_sphere function)
- keyguide_render.c line 79 (render_keyguide function)
- hud_render.c line 84, 218 (2 functions)
- hud_performance.c line 74 (render_performance function)
- aabb.c line 162 (aabb_intersect_ray function)
- bvh_build.c line 167 (build_bvh_recursive function)

### Extract main.c line 79

- [ ] T086 [US2] Extract initialization logic from main() to static function init_program() in src/core/main.c
- [ ] T087 [US2] Extract cleanup logic from main() to static function cleanup_program() in src/core/main.c
- [ ] T088 [US2] Verify: `norminette src/core/main.c | grep TOO_MANY_LINES` outputs nothing
- [ ] T089 [US2] Build and test: `make && ./miniRT scenes/simple.rt` succeeds

### Extract render.c line 99

- [ ] T090 [US2] Extract ray generation logic from render_scene() to static function generate_camera_rays() in src/core/render.c
- [ ] T091 [US2] Extract color calculation logic to static function calculate_pixel_colors() in src/core/render.c
- [ ] T092 [US2] Verify: `norminette src/core/render.c | grep TOO_MANY_LINES` outputs nothing

### Extract window.c functions (3 functions)

- [ ] T093 [P] [US2] Extract initialization steps from create_window() to static helper init_window_properties() in src/ui/window_init.c
- [ ] T094 [P] [US2] Extract event handler registration from setup_hooks() to static helper register_event_callbacks() in src/ui/window_hooks.c
- [ ] T095 [P] [US2] Extract rendering loop logic from window_render_loop() to static helper execute_render_cycle() in src/ui/window_hooks.c
- [ ] T096 [US2] Verify: `norminette src/ui/window*.c | grep TOO_MANY_LINES` outputs nothing

### Extract parse_elements.c line 80

- [ ] T097 [US2] Extract sphere validation logic from parse_sphere() to static function validate_sphere_params() in src/utils/parse_elements.c
- [ ] T098 [US2] Extract sphere construction to static function create_sphere_object() in src/utils/parse_elements.c
- [ ] T099 [US2] Verify: `norminette src/utils/parse_elements.c | grep TOO_MANY_LINES` outputs nothing

### Extract keyguide_render.c line 79

- [ ] T100 [US2] Extract key layout rendering from render_keyguide() to static function render_key_layout() in src/ui/keyguide_render.c
- [ ] T101 [US2] Extract key description rendering to static function render_key_descriptions() in src/ui/keyguide_render.c
- [ ] T102 [US2] Verify: `norminette src/ui/keyguide_render.c | grep TOO_MANY_LINES` outputs nothing

### Extract hud_render.c functions (2 functions)

- [ ] T103 [P] [US2] Extract HUD section rendering from render_hud() to static function render_hud_sections() in src/ui/hud/hud_render.c
- [ ] T104 [P] [US2] Extract HUD overlay drawing from draw_hud_overlay() to static function draw_overlay_background() in src/ui/hud/hud_render.c
- [ ] T105 [US2] Verify: `norminette src/ui/hud/hud_render.c | grep TOO_MANY_LINES` outputs nothing

### Extract hud_performance.c line 74

- [ ] T106 [US2] Extract performance metrics calculation from render_performance() to static function calculate_perf_metrics() in src/ui/hud_performance.c
- [ ] T107 [US2] Extract performance display formatting to static function format_perf_display() in src/ui/hud_performance.c
- [ ] T108 [US2] Verify: `norminette src/ui/hud_performance.c | grep TOO_MANY_LINES` outputs nothing

### Extract aabb.c line 162

- [ ] T109 [US2] Extract ray-box intersection calculation from aabb_intersect_ray() to static function calculate_intersection_t() in src/bvh/aabb_advanced.c
- [ ] T110 [US2] Extract intersection validation to static function validate_intersection_range() in src/bvh/aabb_advanced.c
- [ ] T111 [US2] Verify: `norminette src/bvh/aabb_advanced.c | grep TOO_MANY_LINES` outputs nothing

### Extract bvh_build.c line 167

- [ ] T112 [US2] Extract partition logic from build_bvh_recursive() to static function partition_primitives() in src/bvh/bvh_build_core.c
- [ ] T113 [US2] Extract child node creation to static function create_child_nodes() in src/bvh/bvh_build_core.c
- [ ] T114 [US2] Verify: `norminette src/bvh/bvh_build_core.c | grep TOO_MANY_LINES` outputs nothing

### Phase 3 Checkpoint - US2 Complete

- [ ] T115 [US2] Run full norminette check: `norminette src/ | grep TOO_MANY_LINES | wc -l` should be 0
- [ ] T116 [US2] Full build verification: `make clean && make` succeeds
- [ ] T117 [US2] Render all test scenes and compare with baseline for behavior preservation
- [ ] T118 [US2] Verify exit codes: All scenes exit with code 0
- [ ] T119 [US2] Document progress: Update norminette error count (should have eliminated ~12 TOO_MANY_LINES errors)

**Checkpoint**: Function length compliance achieved

---

## Phase 4: User Story 3 - Header File Formatting Compliance (Priority: P2)

**Goal**: Fix all formatting violations in header files (TOO_FEW_TAB, SPC_BEFORE_NL, LINE_TOO_LONG)

**Independent Test**: Run `norminette includes/ | grep "TOO_FEW_TAB\|SPC_BEFORE_NL\|LINE_TOO_LONG" | wc -l` should output 0

**Header Files Requiring Fixes**:
- includes/ui/hud.h (5 TOO_FEW_TAB + 5 SPC_BEFORE_NL errors at lines 90-147)
- window.c also has formatting issues (included here as related to window headers)

### Fix includes/ui/hud.h formatting

- [ ] T120 [P] [US3] Fix line 90: Remove trailing space and correct tab indentation in includes/ui/hud.h
- [ ] T121 [P] [US3] Fix line 104: Remove trailing space and correct tab indentation in includes/ui/hud.h
- [ ] T122 [P] [US3] Fix line 119: Remove trailing space and correct tab indentation in includes/ui/hud.h
- [ ] T123 [P] [US3] Fix line 132: Remove trailing space and correct tab indentation in includes/ui/hud.h
- [ ] T124 [P] [US3] Fix line 146: Remove trailing space and correct tab indentation in includes/ui/hud.h
- [ ] T125 [US3] Verify: `norminette includes/ui/hud.h` reports no TOO_FEW_TAB or SPC_BEFORE_NL errors

### Fix window.c formatting issues

- [ ] T126 [P] [US3] Fix line 139: Correct tab indentation in src/ui/window_init.c (after split)
- [ ] T127 [P] [US3] Fix line 172: Correct tab indentation in src/ui/window_events.c (after split)
- [ ] T128 [US3] Fix line 213: Break long line (>80 chars) into multiple lines in src/ui/window_events.c
- [ ] T129 [US3] Verify: `norminette src/ui/window*.c | grep "TOO_FEW_TAB\|LINE_TOO_LONG"` outputs nothing

### Phase 4 Checkpoint - US3 Complete

- [ ] T130 [US3] Run norminette on headers: `norminette includes/ | grep "Error:" | wc -l` should show reduced errors
- [ ] T131 [US3] Verify build still succeeds: `make clean && make`
- [ ] T132 [US3] Document progress: Formatting errors eliminated

**Checkpoint**: Header formatting compliance achieved

---

## Phase 5: User Story 4 - Parameter and Variable Count Compliance (Priority: P3)

**Goal**: Eliminate TOO_MANY_ARGS and TOO_MANY_VARS_FUNC errors through parameter structures and variable refactoring

**Independent Test**: Run `norminette src/ includes/ | grep "TOO_MANY_ARGS\|TOO_MANY_VARS"` should output only mlx library functions

**Violations to Fix**:
- render_state.c line 97: TOO_MANY_ARGS (5 parameters)
- keyguide_render.c line 18: TOO_MANY_ARGS (5 parameters)
- hud_render.c line 17: TOO_MANY_ARGS (5 parameters)
- hud_performance.c line 17: TOO_MANY_ARGS (5 parameters)
- hud_transparency.c line 50, 59: TOO_MANY_ARGS (5+ parameters)
- bvh_traverse.c line 58: TOO_MANY_ARGS (5 parameters)
- bvh_build.c line 74: TOO_MANY_ARGS (5 parameters)
- render.c line 75: TOO_MANY_VARS_FUNC (6 variables)
- bvh_traverse.c line 89: TOO_MANY_VARS_FUNC (6 variables)
- bvh_build.c line 137: TOO_MANY_VARS_FUNC (6 variables)

### Create parameter structures

- [ ] T133 [P] [US4] Create t_render_params structure in includes/structures.h for rendering parameters
- [ ] T134 [P] [US4] Create t_hud_params structure in includes/ui/hud.h for HUD rendering parameters
- [ ] T135 [P] [US4] Create t_bvh_traversal structure in includes/spatial.h for BVH traversal parameters
- [ ] T136 [P] [US4] Create t_bvh_build_params structure in includes/spatial.h for BVH build parameters

### Refactor render_state.c line 97

- [ ] T137 [US4] Refactor update_render_state() to use t_render_params structure in src/core/render_state.c
- [ ] T138 [US4] Update all call sites of update_render_state() to pass structure
- [ ] T139 [US4] Verify: `norminette src/core/render_state.c | grep TOO_MANY_ARGS` outputs nothing

### Refactor keyguide_render.c line 18

- [ ] T140 [US4] Refactor render_keyguide_with_params() to use t_hud_params in src/ui/keyguide_render.c
- [ ] T141 [US4] Update caller in main rendering loop to pass structure
- [ ] T142 [US4] Verify: `norminette src/ui/keyguide_render.c | grep TOO_MANY_ARGS` outputs nothing

### Refactor HUD functions (3 functions)

- [ ] T143 [P] [US4] Refactor render_hud_section() in src/ui/hud/hud_render.c to use t_hud_params
- [ ] T144 [P] [US4] Refactor render_performance_hud() in src/ui/hud_performance.c to use t_hud_params
- [ ] T145 [P] [US4] Refactor apply_transparency() in src/ui/hud/hud_alpha.c to use t_hud_params
- [ ] T146 [US4] Update all HUD function call sites to pass structure parameter
- [ ] T147 [US4] Verify: `norminette src/ui/hud/*.c | grep TOO_MANY_ARGS` outputs nothing

### Refactor BVH functions

- [ ] T148 [P] [US4] Refactor traverse_bvh_with_ray() in src/bvh/bvh_traverse.c to use t_bvh_traversal structure
- [ ] T149 [P] [US4] Refactor build_bvh_node() in src/bvh/bvh_build_core.c to use t_bvh_build_params structure
- [ ] T150 [US4] Update BVH call sites to pass structure parameters
- [ ] T151 [US4] Verify: `norminette src/bvh/*.c | grep TOO_MANY_ARGS` outputs nothing

### Reduce variable counts in functions

- [ ] T152 [P] [US4] Refactor render_scene() in src/core/render.c to reduce local variables from 6 to ≤5
- [ ] T153 [P] [US4] Refactor traverse_bvh() in src/bvh/bvh_traverse.c to reduce local variables from 6 to ≤5
- [ ] T154 [P] [US4] Refactor build_bvh_recursive() in src/bvh/bvh_build_core.c to reduce local variables from 6 to ≤5
- [ ] T155 [US4] Verify: `norminette src/ | grep TOO_MANY_VARS_FUNC` outputs nothing (except external libs)

### Phase 5 Checkpoint - US4 Complete

- [ ] T156 [US4] Run norminette check: `norminette src/ includes/ | grep "TOO_MANY_ARGS\|TOO_MANY_VARS"` 
- [ ] T157 [US4] Verify only mlx library functions remain in TOO_MANY_ARGS list (external dependencies)
- [ ] T158 [US4] Full build verification: `make clean && make` succeeds
- [ ] T159 [US4] Render test scenes and verify behavior unchanged
- [ ] T160 [US4] Document progress: Parameter/variable compliance achieved

**Checkpoint**: Parameter and variable count compliance achieved

---

## Phase 6: User Story 5 - Miscellaneous Compliance Issues (Priority: P3)

**Goal**: Fix remaining norminette errors (TERNARY_FBIDDEN, PREPROC_CONSTANT, MISALIGNED_FUNC_DECL, EMPTY_LINE_EOF)

**Independent Test**: Run `norminette src/ includes/` should report 0 errors

**Remaining Violations**:
- window.c: 18 TERNARY_FBIDDEN errors (lines 218-266)
- render_state.c line 57: TERNARY_FBIDDEN
- includes/constants.h line 21: PREPROC_CONSTANT
- includes/minirt.h line 237: MISALIGNED_FUNC_DECL
- hud_format.c line 152: EMPTY_LINE_EOF

### Fix ternary operators in window.c

- [ ] T161 [US5] Replace ternary at line 218 with if-else in src/ui/window_events.c
- [ ] T162 [US5] Replace ternary at line 219 with if-else in src/ui/window_events.c
- [ ] T163 [US5] Replace ternary at line 222 with if-else in src/ui/window_events.c
- [ ] T164 [US5] Replace ternary at line 223 with if-else in src/ui/window_events.c
- [ ] T165 [US5] Replace ternary at line 226 with if-else in src/ui/window_events.c
- [ ] T166 [US5] Replace ternary at line 227 with if-else in src/ui/window_events.c
- [ ] T167 [US5] Replace ternary at line 243 with if-else in src/ui/window_events.c
- [ ] T168 [US5] Replace ternary at line 244 with if-else in src/ui/window_events.c
- [ ] T169 [US5] Replace ternary at line 246 with if-else in src/ui/window_events.c
- [ ] T170 [US5] Replace ternary at line 248 with if-else in src/ui/window_events.c
- [ ] T171 [US5] Replace ternary at line 254 with if-else in src/ui/window_events.c
- [ ] T172 [US5] Replace ternary at line 255 with if-else in src/ui/window_events.c
- [ ] T173 [US5] Replace ternary at line 257 with if-else in src/ui/window_events.c
- [ ] T174 [US5] Replace ternary at line 263 with if-else in src/ui/window_events.c
- [ ] T175 [US5] Replace ternary at line 264 with if-else in src/ui/window_events.c
- [ ] T176 [US5] Replace ternary at line 266 with if-else in src/ui/window_events.c
- [ ] T177 [US5] Verify: `norminette src/ui/window_events.c | grep TERNARY_FBIDDEN` outputs nothing

### Fix ternary in render_state.c

- [ ] T178 [US5] Replace ternary at line 57 with if-else in src/core/render_state.c
- [ ] T179 [US5] Verify: `norminette src/core/render_state.c | grep TERNARY_FBIDDEN` outputs nothing

### Fix preprocessor constant

- [ ] T180 [US5] Fix PREPROC_CONSTANT violation at line 21 in includes/constants.h
- [ ] T181 [US5] Verify: `norminette includes/constants.h | grep PREPROC_CONSTANT` outputs nothing

### Fix function declaration alignment

- [ ] T182 [US5] Fix MISALIGNED_FUNC_DECL at line 237 in includes/minirt.h
- [ ] T183 [US5] Verify: `norminette includes/minirt.h | grep MISALIGNED_FUNC_DECL` outputs nothing

### Fix empty line at EOF

- [ ] T184 [US5] Remove empty line at EOF (line 152) in src/ui/hud/hud_format_numbers.c
- [ ] T185 [US5] Verify: `norminette src/ui/hud/hud_format_numbers.c | grep EMPTY_LINE_EOF` outputs nothing

### Phase 6 Checkpoint - US5 Complete

- [ ] T186 [US5] Run complete norminette check: `norminette src/ includes/ | grep "Error:"` should output nothing
- [ ] T187 [US5] Count errors: `norminette src/ includes/ 2>&1 | grep "Error:" | wc -l` should be 0
- [ ] T188 [US5] Full build verification: `make clean && make && make bonus` all succeed
- [ ] T189 [US5] Document progress: All miscellaneous compliance issues resolved

**Checkpoint**: All norminette compliance issues resolved (0 errors)

---

## Phase 7: Final Validation & Performance Verification

**Purpose**: Comprehensive validation that all changes preserve functionality and meet success criteria

- [ ] T190 Verify zero norminette errors: `norminette src/ includes/ 2>&1 | tee norminette_final.txt`
- [ ] T191 Confirm error count: `grep "Error:" norminette_final.txt | wc -l` outputs 0
- [ ] T192 Full build test: `make clean && make && make bonus` all succeed with zero errors
- [ ] T193 [P] Render all test scenes: `for scene in scenes/*.rt; do ./miniRT "$scene" > "final_renders/$(basename "$scene" .rt).ppm"; done`
- [ ] T194 [P] Compare renders with baseline: `./scripts/compare_renders.sh baseline_renders/ final_renders/`
- [ ] T195 Verify all exit codes are 0 for all test scenes
- [ ] T196 [P] Measure final performance: `time ./miniRT scenes/complex_scene.rt > /dev/null` (record time)
- [ ] T197 Calculate performance delta: Compare final vs baseline (must be ≤5% regression)
- [ ] T198 [P] Memory leak check: `valgrind --leak-check=full ./miniRT scenes/simple.rt` reports 0 leaks
- [ ] T199 Code review: Verify all file splits maintain logical cohesion and clear naming
- [ ] T200 Documentation review: Verify all commit messages document changes clearly
- [ ] T201 Generate comparison report showing before/after error counts by category
- [ ] T202 Update project README with new file structure documentation
- [ ] T203 Create NORMINETTE_COMPLIANCE_COMPLETE.md documenting achievement of 0 errors

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - start immediately
- **US1 File Splitting (Phase 2)**: Depends on Setup completion - highest priority
- **US2 Function Extraction (Phase 3)**: Depends on US1 completion (files must be split first)
- **US3 Formatting (Phase 4)**: Can start after US1 - independent of US2
- **US4 Parameters (Phase 5)**: Depends on US1 and US2 completion (functions must exist in correct files)
- **US5 Miscellaneous (Phase 6)**: Depends on US1 completion (ternaries may be in split files)
- **Final Validation (Phase 7)**: Depends on all user stories being complete

### User Story Dependencies

- **US1 (File Splitting)**: MUST complete first - all other work depends on correct file structure
- **US2 (Function Extraction)**: Depends on US1 - functions must be in correct files before extraction
- **US3 (Formatting)**: Independent after US1 - can run parallel with US2
- **US4 (Parameters)**: Depends on US1 and US2 - functions must be short and in correct files
- **US5 (Miscellaneous)**: Mostly independent - some ternaries may be in split files from US1

### Parallel Opportunities

**Within US1 (File Splitting)**:
- All file creation tasks (T009-T011, T019-T021, etc.) can run in parallel
- File splits for different modules (HUD, metrics, window, BVH) can run in parallel
- Build verification must be sequential after each module split

**Within US2 (Function Extraction)**:
- Extractions in different files can run in parallel (T086-T089, T090-T092, etc.)
- Each file's extraction must complete before norminette verification

**Within US3 (Formatting)**:
- All formatting fixes can run in parallel (T120-T124)
- Window formatting (T126-T128) can run parallel with header fixes

**Within US4 (Parameters)**:
- Structure creation (T133-T136) can run in parallel
- Function refactoring within different modules can run in parallel
- Call site updates must be sequential after structure creation

**Within US5 (Miscellaneous)**:
- Ternary replacements can run in parallel per file
- All miscellaneous fixes (T180-T184) can run in parallel

---

## Parallel Example: User Story 1 - File Splitting

```bash
# Launch all file creation tasks in parallel (first set):
Task T009: "Create src/ui/hud/hud_text.c and includes/ui/hud/hud_text.h"
Task T010: "Create src/ui/hud/hud_metrics.c and includes/ui/hud/hud_metrics.h"
Task T011: "Create src/ui/hud/hud_controls.c and includes/ui/hud/hud_controls.h"

# After hud_render split completes, launch metrics split in parallel:
Task T019: "Create src/core/metrics_tracking.c and includes/metrics_tracking.h"
Task T020: "Create src/core/metrics_display.c and includes/metrics_display.h"
Task T021: "Create src/core/metrics_fps.c and includes/metrics_fps.h"

# Window, timer, aabb, bvh splits can all run in parallel (different modules):
Task T029-T031: "Create window split files"
Task T039-T040: "Create timer split files"
Task T046-T047: "Create aabb split files"
Task T053-T054: "Create bvh_build split files"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only - File Splitting)

1. Complete Phase 1: Setup & Baseline
2. Complete Phase 2: User Story 1 (File Function Count Compliance)
3. **STOP and VALIDATE**: 
   - Verify TOO_MANY_FUNCS errors reduced from 53 to 0
   - Build succeeds, all tests pass
   - Progress: 77 → ~24 errors remaining (68% complete)

### Incremental Delivery

1. **Foundation** (Phase 1) → Baseline established
2. **US1** (Phase 2) → 53 errors eliminated (68% progress) → Validate independently
3. **US2** (Phase 3) → ~12 more errors eliminated (84% progress) → Validate independently
4. **US3** (Phase 4) → ~12 more errors eliminated (100% code errors) → Validate independently
5. **US4** (Phase 5) → Parameter compliance achieved → Validate independently
6. **US5** (Phase 6) → Final 100% compliance → Validate independently
7. **Final** (Phase 7) → Comprehensive validation and performance verification

### Parallel Team Strategy

With multiple developers:

1. **Setup Phase**: Team completes baseline together (Phase 1)
2. **US1 File Splitting**: 
   - Developer A: HUD files (hud_render, hud_format, hud_transparency, hud_navigation)
   - Developer B: Core files (metrics, timer, render_state)
   - Developer C: Window files (window split)
   - Developer D: BVH files (aabb, bvh_build)
3. **US2 Function Extraction**: Each developer extracts functions in their assigned modules
4. **US3-US5**: Distribute remaining tasks by file ownership from US1 split

---

## Success Metrics

| Metric | Baseline | Target | Status |
|--------|----------|--------|--------|
| Total Norminette Errors | 77 | 0 | To Be Measured |
| TOO_MANY_FUNCS Errors | 53 | 0 | After Phase 2 |
| TOO_MANY_LINES Errors | 12 | 0 | After Phase 3 |
| Formatting Errors | 12 | 0 | After Phase 4 |
| TOO_MANY_ARGS Errors | 7 | 0* | After Phase 5 |
| TOO_MANY_VARS Errors | 3 | 0 | After Phase 5 |
| Build Success | ✅ | ✅ | Continuous |
| Test Scenes Pass Rate | 100% | 100% | After Phase 7 |
| Performance Regression | 0% | ≤5% | After Phase 7 |

*Note: mlx library functions may still have TOO_MANY_ARGS as external dependencies

---

## Validation Checklist

- [ ] ✅ Zero norminette errors achieved (77 → 0)
- [ ] ✅ All files have ≤5 functions
- [ ] ✅ All functions have ≤25 lines
- [ ] ✅ All headers pass formatting checks
- [ ] ✅ All custom functions have ≤4 parameters
- [ ] ✅ All functions have ≤5 local variables
- [ ] ✅ All ternary operators replaced with if-else
- [ ] ✅ No miscellaneous violations remain
- [ ] ✅ Build succeeds: `make clean && make && make bonus`
- [ ] ✅ All test scenes render identically to baseline
- [ ] ✅ Performance within 5% of baseline
- [ ] ✅ Zero memory leaks reported by valgrind
- [ ] ✅ Code review confirms improved readability
- [ ] ✅ Documentation updated with file structure changes

---

## Notes

- **[P] tasks**: Can run in parallel (different files, no shared dependencies)
- **[Story] label**: Maps task to user story for traceability and independent testing
- **Critical Path**: Phase 1 → US1 → US2 → Final Validation
- **Fastest Path to Progress**: Complete US1 first (eliminates 68% of errors)
- **Each checkpoint**: Stop and validate independently before proceeding
- **Rollback strategy**: Git commit after each phase completion for easy revert
- **Build after every file split**: Catch compilation errors immediately
- **Performance monitoring**: Measure before/after each major phase

**Total Tasks**: 203 tasks organized across 7 phases
**Estimated Completion**: 68% progress after US1, 100% after all phases
**MVP Milestone**: After Phase 2 (US1) - 53 errors eliminated
