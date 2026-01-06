# Implementation Plan: Complete Norminette Compliance

**Branch**: `013-norminette-compliance` | **Date**: 2026-01-03 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/013-norminette-compliance/spec.md`

**Note**: This plan eliminates all 77 norminette errors through systematic file splitting, function refactoring, and formatting corrections.

## Summary

Achieve 100% norminette compliance (0 errors) by systematically addressing violations across 5 categories: (1) TOO_MANY_FUNCS (53 errors) through logical file splitting by feature concerns, (2) TOO_MANY_LINES (5 errors) through function extraction, (3) formatting issues (10 errors) through tab/space correction, (4) parameter/variable count violations (7 errors) through structure refactoring, and (5) miscellaneous issues (2 errors) through naming/alignment fixes. All refactoring must preserve existing functionality and maintain rendering performance within 5% of baseline.

## Technical Context

**Language/Version**: C (C99 standard per 42 School requirements)  
**Primary Dependencies**: MiniLibX (graphics), libft (custom C library), math library (-lm)  
**Storage**: N/A (no persistent storage, scene files loaded at startup)  
**Testing**: Manual test scenes in `scenes/` directory, automated image diff validation, norminette compliance checker  
**Target Platform**: Linux and macOS (42 School evaluation environments)  
**Project Type**: Single executable (graphics application)  
**Performance Goals**: Real-time ray tracing at interactive frame rates (target: maintain current performance ±5%)  
**Constraints**: 
  - 42 School norminette rules: ≤5 functions/file, ≤25 lines/function, ≤4 params/function, ≤5 vars/function
  - No pthread, fork, SIMD, or external parallelization libraries allowed
  - Must use only allowed functions: libc standard, math.h, mlx_*, libft, get_next_line
  - Zero norminette errors required for project certification
**Scale/Scope**: 
  - Current: 77 norminette errors across ~20 source files
  - Target: 0 norminette errors, estimated 30-35 source files after splitting
  - 8 files exceed function limit (need splitting)
  - 5 functions exceed line limit (need extraction)
  - 10 formatting violations (headers)
  - 7 parameter/variable count violations

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Code Quality Automation** (Principle II):
- ✅ All changes MUST pass norminette with zero errors before merge
- ✅ Build MUST succeed with `make` and `make bonus` without errors
- ✅ Memory leak check (valgrind) MUST report zero leaks
- ✅ CI pipeline MUST enforce all quality gates automatically

**42 School Constraints Check** (Principle VII):
- ✅ All functions used MUST be from allowed list (libc, math, mlx_*, libft, get_next_line)
- ✅ pthread functions MUST NOT be used (not applicable - refactoring only)
- ✅ fork/pipe/multiprocessing MUST NOT be used (not applicable - refactoring only)
- ✅ External parallelization libraries (SIMD, OpenMP, etc.) MUST NOT be used (not applicable - refactoring only)
- ✅ Optimizations MUST use only allowed techniques (not applicable - no new optimizations, only code restructuring)
- ✅ Refactoring MUST NOT introduce any prohibited functions or libraries

**Project Structure Standards** (Principle I):
- ✅ New source files MUST follow existing organization in `src/` with subdirectories by feature
- ✅ New header files MUST be organized in `includes/` directory
- ✅ One header file per source file with matching names (e.g., `src/ui/hud_text.c` → `includes/ui/hud_text.h`)
- ✅ Build artifacts MUST remain in `.gitignore`

**Documentation Standards**:
- ✅ All C functions MUST maintain Doxygen-style comments in English
- ✅ Commit messages MUST clearly document file splits and function movements
- ✅ 42 Header format MUST be preserved in all new files

**Quality Assurance Workflow**:
- ✅ Each file split MUST be followed by immediate build verification
- ✅ Each refactoring batch MUST be followed by test scene validation
- ✅ Performance baseline MUST be measured before refactoring begins
- ✅ Performance MUST be re-measured after all changes (≤5% regression allowed)

**GATE STATUS**: ✅ PASSED - All principles satisfied. Refactoring work does not introduce prohibited functions, maintains quality gates, follows structure standards, and preserves documentation requirements.

## Project Structure

### Documentation (this feature)

```text
specs/[###-feature]/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
Current Structure:
src/
├── core/              # Main entry, trace, metrics, render
├── geometry/          # Camera, vector operations
├── ui/               # Window, HUD rendering, keyguide
├── utils/            # Parser, error handling, utilities
├── lighting/         # Shadow calculations, lighting
└── bvh/              # BVH acceleration structure

Post-Refactoring Structure (estimated):
src/
├── core/
│   ├── main.c                    # Entry point (currently 1 TOO_MANY_LINES error)
│   ├── trace.c                   # Ray tracing (OK)
│   ├── metrics.c → SPLIT INTO:
│   │   ├── metrics_tracking.c    # Metrics collection (≤5 functions)
│   │   ├── metrics_display.c     # Metrics display (≤5 functions)
│   │   └── metrics_fps.c         # FPS calculation (≤5 functions)
│   ├── metrics_calc.c            # (OK)
│   ├── render.c                  # Main rendering (1 TOO_MANY_VARS, 1 TOO_MANY_LINES, 1 TOO_MANY_FUNCS)
│   ├── render_init.c             # Render initialization (extracted)
│   ├── render_loop.c             # Render loop (extracted)
│   ├── render_state.c            # State management (1 TERNARY, 2 TOO_MANY_FUNCS, 1 TOO_MANY_ARGS)
│   ├── render_state_update.c     # State updates (extracted)
│   └── timer.c → SPLIT INTO:
│       ├── timer_control.c       # Timer control (≤5 functions)
│       └── timer_metrics.c       # Timer metrics (≤5 functions)
│
├── geometry/
│   ├── camera.c                  # (OK)
│   ├── vector.c                  # (OK)
│   └── vector_ops.c              # (OK)
│
├── ui/
│   ├── window.c → SPLIT INTO:
│   │   ├── window_init.c         # Window initialization (≤5 functions)
│   │   ├── window_events.c       # Event handling (≤5 functions)
│   │   └── window_hooks.c        # MLX hooks setup (≤5 functions)
│   ├── hud/ (NEW SUBDIRECTORY for better organization)
│   │   ├── hud_render.c          # Main HUD coordination (≤5 functions)
│   │   ├── hud_text.c            # Text rendering (extracted)
│   │   ├── hud_metrics.c         # Metrics display (extracted)
│   │   └── hud_controls.c        # Control hints (extracted)
│   ├── keyguide_cleanup.c        # (OK)
│   ├── keyguide_init.c           # (OK)
│   └── keyguide_render.c         # (2 TOO_MANY_ARGS, 1 TOO_MANY_LINES)
│
├── utils/
│   ├── parser.c                  # (OK)
│   ├── parse_elements.c          # (1 TOO_MANY_LINES)
│   ├── parse_objects.c           # (OK)
│   ├── parse_validation.c        # (OK)
│   ├── error.c                   # (OK)
│   ├── cleanup.c                 # (OK)
│   ├── ft_atof.c                 # (OK)
│   └── ft_atoi.c                 # (OK)
│
├── lighting/
│   ├── lighting.c                # (OK)
│   ├── shadow_test.c             # (OK)
│   ├── shadow_config.c           # (OK)
│   ├── shadow_calc.c             # (OK)
│   └── shadow_attenuation.c      # (OK)
│
└── bvh/
    ├── bvh_init.c                # (OK)
    ├── bounds.c                  # (OK)
    ├── aabb.c → SPLIT INTO:
    │   ├── aabb_basic.c          # Basic operations (≤5 functions)
    │   └── aabb_advanced.c       # Advanced operations (≤5 functions)
    ├── bvh_traverse.c            # (2 TOO_MANY_ARGS, 1 TOO_MANY_VARS)
    └── bvh_build.c → SPLIT INTO:
        ├── bvh_build_core.c      # Core building (≤5 functions)
        └── bvh_build_helpers.c   # Helper functions (≤5 functions)

includes/
├── miniRT.h                      # Main header (formatting issues)
├── structures.h                  # Type definitions (formatting issues)
├── constants.h                   # Constants (1 PREPROC_CONSTANT error)
└── [new headers matching each .c file]

New Headers to Create (one per new source file):
├── metrics_tracking.h
├── metrics_display.h
├── metrics_fps.h
├── render_init.h
├── render_loop.h
├── render_state_update.h
├── timer_control.h
├── timer_metrics.h
├── window_init.h
├── window_events.h
├── window_hooks.h
├── hud/
│   ├── hud_text.h
│   ├── hud_metrics.h
│   └── hud_controls.h
├── aabb_basic.h
├── aabb_advanced.h
├── bvh_build_core.h
└── bvh_build_helpers.h
```

**Structure Decision**: Single project (graphics application) with hierarchical source organization. New files will be created alongside existing files, organized by feature domain (core/, ui/, bvh/, etc.). The hud/ subdirectory will be created under ui/ to better organize the HUD-related files after splitting. All new source files will have corresponding header files in includes/ with matching names. This structure maintains consistency with existing organization while accommodating the increased file count from splitting.

## Complexity Tracking

> **No violations to justify** - All constitution checks passed. This is a refactoring task that does not introduce new complexity, only reorganizes existing code to meet norminette constraints.

---

## Phase 0: Outline & Research

**Output**: `research.md` with refactoring strategies and validation approach

### Research Tasks

1. **File Splitting Strategy Research**
   - **Task**: Identify best practices for splitting large C files while maintaining logical cohesion
   - **Questions**:
     - How to determine natural split boundaries in C code?
     - What patterns minimize header file complexity?
     - How to maintain call graph efficiency after splitting?
   - **Expected Output**: Document decision criteria for grouping functions into files

2. **Function Extraction Techniques**
   - **Task**: Research approaches for extracting long functions (>25 lines) into helper functions
   - **Questions**:
     - How to extract calculation logic without introducing performance overhead?
     - What naming conventions make helper function purpose clear?
     - How to avoid creating too many micro-functions?
   - **Expected Output**: Guidelines for function extraction with examples

3. **Parameter Structure Design**
   - **Task**: Research patterns for reducing function parameters through structures
   - **Questions**:
     - When should parameters be grouped vs. kept separate?
     - How to design structures that are reusable across functions?
     - What impact do parameter structures have on stack usage and performance?
   - **Expected Output**: Design patterns for parameter structures in performance-critical code

4. **Validation Strategy**
   - **Task**: Define comprehensive validation approach to ensure functionality preservation
   - **Questions**:
     - What automated checks can verify behavior equivalence?
     - How to establish performance baseline and acceptable regression threshold?
     - What manual verification steps are necessary?
   - **Expected Output**: Step-by-step validation workflow for each refactoring stage

5. **Makefile Update Strategy**
   - **Task**: Research approaches for maintaining Makefile with increasing file count
   - **Questions**:
     - Should sources be listed explicitly or discovered with wildcards?
     - How to organize source file groups for better maintainability?
     - What pattern supports incremental builds most efficiently?
   - **Expected Output**: Makefile organization recommendation

### Research Deliverable Format

```markdown
# Norminette Compliance Refactoring Research

## File Splitting Strategy

**Decision**: [Split by feature concern with max 5 functions per file]
**Rationale**: [Why chosen - maintains logical cohesion, clear ownership]
**Alternatives Considered**: [Alphabetical splitting - rejected because loses semantic grouping]

## Function Extraction Techniques

**Decision**: [Extract calculation blocks, conditional branches, initialization sequences]
**Rationale**: [Improves testability, maintains performance with inline hints]
**Alternatives Considered**: [Macro-based approaches - rejected due to debugging complexity]

## Parameter Structure Design

**Decision**: [Group related parameters by domain (rendering params, state params, config params)]
**Rationale**: [Reduces signature complexity, enables future extensibility]
**Alternatives Considered**: [Global state - rejected due to testability concerns]

## Validation Strategy

**Decision**: [Automated image diff + exit code validation + manual smoke test + performance benchmark]
**Rationale**: [Comprehensive coverage, fast feedback, objective metrics]
**Workflow**:
1. Establish baseline: Save reference renders + performance metrics before refactoring
2. After each file split: Build, run test scenes, compare images, verify exit codes
3. After all changes: Full performance comparison with ≤5% regression threshold

## Makefile Update Strategy

**Decision**: [Explicit source file listing organized by directory]
**Rationale**: [Clear visibility, supports partial builds, avoids accidental inclusion]
**Pattern**: Group SRCS by subdirectory with comments for each feature area
```

---

## Phase 1: Design & Contracts

**Output**: `data-model.md`, `contracts/`, `quickstart.md`, updated agent context

### Data Model

**Note**: This refactoring task does not introduce new data structures. The data model document will catalog existing structures that are referenced during refactoring to ensure correct parameter passing and state management.

#### Existing Entities to Document

**Source File Module**:
- **Attributes**: file_path, function_count, function_list, line_count, dependencies
- **Constraints**: function_count ≤ 5, functions must be logically related
- **Relationships**: Contains 1-5 functions, included by header files

**Function Unit**:
- **Attributes**: name, line_count, parameter_count, local_variable_count, complexity_score
- **Constraints**: line_count ≤ 25, parameter_count ≤ 4, local_variable_count ≤ 5
- **Relationships**: Belongs to one source file, may call other functions

**Parameter Structure** (new):
- **Attributes**: struct_name, field_list, usage_functions
- **Purpose**: Group related parameters to reduce function signature complexity
- **Examples**: 
  - `t_render_params`: Contains rendering configuration (width, height, samples, etc.)
  - `t_state_update_params`: Contains state modification parameters

**Header File**:
- **Attributes**: file_path, declarations, includes, guards
- **Constraints**: Must follow norminette tab/space rules, one header per source file
- **Relationships**: Declares functions from one source file

### Contracts (Refactoring Rules)

Since this is a refactoring task, contracts define the rules that must be maintained rather than APIs.

#### Contract 1: Function Signature Preservation

```c
// RULE: Public function signatures MUST NOT change
// Rationale: External callers depend on existing signatures

// ALLOWED: Extract private helper function
static void helper_calculate_subset(t_data *data, int start, int end);

// NOT ALLOWED: Change public function signature
// Before: void render_scene(t_scene *scene, t_camera *camera);
// After: void render_scene(t_render_params *params);  // BREAKS CONTRACT
```

#### Contract 2: Behavior Equivalence

```c
// RULE: Refactored code MUST produce identical output for identical input
// Validation: Automated image diff must show 0 pixel differences

// ALLOWED: Reorder independent statements
calc_a();
calc_b();  // If calc_a and calc_b don't depend on each other

// NOT ALLOWED: Change calculation order if side effects exist
update_state();
read_state();  // Order matters - cannot reorder
```

#### Contract 3: Performance Preservation

```c
// RULE: Performance regression MUST be ≤5% of baseline
// Validation: Benchmark rendering of complex scene before/after

// ALLOWED: Introduce function calls if inlining opportunity exists
static inline void helper_func(void);

// MONITOR: Function call overhead in tight loops
// May need inline hints or loop restructuring
```

#### Contract 4: Build Compatibility

```c
// RULE: All build targets MUST succeed after refactoring
// Validation: make clean && make && make bonus

// REQUIRED: Update Makefile with all new source files
SRCS += src/core/metrics_tracking.c \
        src/core/metrics_display.c \
        src/core/metrics_fps.c

// REQUIRED: Create headers for new source files
includes/metrics_tracking.h
includes/metrics_display.h
includes/metrics_fps.h
```

### Quickstart Guide

```markdown
# Norminette Compliance Refactoring - Quick Start

## Prerequisites

- norminette installed and in PATH
- Make build system
- Test scenes in `scenes/` directory
- Baseline performance metrics established

## Refactoring Workflow

### Step 1: Establish Baseline

```bash
# Run norminette and count errors
norminette src/ includes/ | tee norminette_baseline.txt
grep "Error:" norminette_baseline.txt | wc -l  # Should be 77

# Build and test current state
make clean && make
./miniRT scenes/complex_scene.rt

# Measure performance
./scripts/benchmark.sh scenes/complex_scene.rt > performance_baseline.txt
```

### Step 2: Split Files (Priority 1: TOO_MANY_FUNCS)

For each file with >5 functions:

```bash
# Example: Split metrics.c (8 functions) into 3 files

# 1. Create new source files
touch src/core/metrics_tracking.c
touch src/core/metrics_display.c  
touch src/core/metrics_fps.c

# 2. Create corresponding headers
touch includes/metrics_tracking.h
touch includes/metrics_display.h
touch includes/metrics_fps.h

# 3. Move functions maintaining 42 header format
# - metrics_tracking.c: metrics initialization + update functions
# - metrics_display.c: display rendering functions
# - metrics_fps.c: FPS calculation functions

# 4. Update Makefile SRCS variable
# Add new files to appropriate section

# 5. Update includes in calling files

# 6. Build and verify
make clean && make
./miniRT scenes/complex_scene.rt

# 7. Verify norminette improvement
norminette src/core/metrics*.c | grep "Error:"
```

### Step 3: Extract Long Functions (Priority 2: TOO_MANY_LINES)

For each function >25 lines:

```bash
# Example: Extract logic from 35-line function

# 1. Identify extraction boundaries
#    - Calculation blocks
#    - Conditional branches
#    - Initialization sequences

# 2. Create static helper functions
static void helper_calculate_lighting(t_scene *scene, t_ray *ray);

# 3. Verify line count
norminette src/lighting/lighting.c | grep "TOO_MANY_LINES"

# 4. Test
make clean && make
./miniRT scenes/lighting_test.rt
```

### Step 4: Fix Formatting (Priority 2: Headers)

```bash
# Fix tab/space issues in headers
norminette includes/ | grep "TOO_FEW_TAB\|SPC_BEFORE_NL"

# Apply fixes systematically:
# - Replace spaces with tabs for indentation
# - Remove trailing whitespace
# - Verify with norminette after each file
```

### Step 5: Reduce Parameters/Variables (Priority 3)

```bash
# For functions with >4 parameters:
# 1. Create parameter structure
# 2. Update function signature
# 3. Update all call sites
# 4. Verify build and behavior

# For functions with >5 local variables:
# 1. Group related variables
# 2. Extract complex calculations to helper functions
```

### Step 6: Final Verification

```bash
# Zero errors check
norminette src/ includes/ | grep "Error:" | wc -l  # Should be 0

# Full build
make clean && make && make bonus

# Regression testing
./scripts/run_all_test_scenes.sh

# Performance comparison
./scripts/benchmark.sh scenes/complex_scene.rt > performance_final.txt
./scripts/compare_performance.sh performance_baseline.txt performance_final.txt
```

## Rollback Procedure

If any step breaks functionality:

```bash
# Revert to previous commit
git reset --hard HEAD~1

# Or revert specific file
git checkout HEAD -- src/path/to/file.c

# Rebuild and retest
make clean && make
```

## Success Criteria Checklist

- [ ] norminette reports 0 errors
- [ ] All test scenes render correctly
- [ ] Performance within 5% of baseline
- [ ] All build targets succeed (make, make bonus)
- [ ] Code review confirms improved readability
```

### Agent Context Update

After completing Phase 1 design:

```bash
# Update agent context with refactoring patterns
cd /Users/yoshin/dev/working/miniRT
.specify/scripts/bash/update-agent-context.sh copilot
```

**New patterns to add**:
- File splitting by feature concern (5 functions max)
- Function extraction for >25 line functions
- Parameter structure patterns for >4 parameter functions
- Header file organization (one per source file)

---

## Phase 2: Task Breakdown (NOT CREATED BY THIS COMMAND)

**Phase 2 is NOT executed by `/speckit.plan` command.**

The `/speckit.tasks` command creates `tasks.md` using the generated design artifacts from Phase 0 and Phase 1.

To proceed to Phase 2:
```bash
# After Phase 1 is complete, run:
/speckit.tasks
```

This will generate:
- `specs/013-norminette-compliance/tasks.md` with implementation tasks organized by priority and dependency

---

## Validation & Testing

### Pre-Implementation Validation

1. **Norminette Baseline**: Record current error count (77 errors)
2. **Build Baseline**: Verify clean build with `make clean && make`
3. **Functional Baseline**: Verify all test scenes render correctly
4. **Performance Baseline**: Measure rendering time for standard complex scene

### During Implementation Validation

After each file split or major refactoring:

1. **Incremental Norminette Check**: `norminette [modified files] | grep Error | wc -l`
2. **Incremental Build**: `make` (should succeed without errors)
3. **Smoke Test**: `./miniRT scenes/simple.rt` (should render correctly)
4. **Exit Code Verification**: Check program exits cleanly

### Post-Implementation Validation

1. **Zero Errors**: `norminette src/ includes/` returns 0 errors
2. **Full Build**: `make clean && make && make bonus` all succeed
3. **Comprehensive Testing**: All test scenes in `scenes/` render identically to baseline
4. **Performance Test**: Rendering performance within ±5% of baseline
5. **Code Review**: Verify improved readability and maintainability

### Validation Tools

```bash
# Norminette error counter
norminette src/ includes/ 2>&1 | grep "Error:" | wc -l

# Image comparison (after implementing image diff tool)
./scripts/compare_renders.sh baseline/ current/

# Performance comparison
./scripts/benchmark.sh scenes/complex_scene.rt

# Memory leak check (if applicable)
valgrind --leak-check=full ./miniRT scenes/test.rt
```

---

## Risk Mitigation

### High-Risk Items

**Risk**: File splitting introduces compilation errors
- **Mitigation**: Test build immediately after each file split
- **Recovery**: Git revert to last working state

**Risk**: Function extraction changes behavior
- **Mitigation**: Automated image diff validation after each change
- **Recovery**: Compare git diff, manually verify logic equivalence

**Risk**: Performance regression >5%
- **Mitigation**: Profile before/after, add inline hints if needed
- **Recovery**: Restructure function calls, consider alternative extraction approach

### Medium-Risk Items

**Risk**: Increased file count complicates navigation
- **Mitigation**: Clear naming conventions, logical directory structure
- **Documentation**: Update README with file organization guide

**Risk**: Parameter structures add memory overhead
- **Mitigation**: Use stack allocation, measure actual overhead
- **Optimization**: Only use structures where necessary (>4 parameters)

### Low-Risk Items

**Risk**: Merge conflicts from formatting changes
- **Mitigation**: Complete formatting changes in dedicated commit
- **Recovery**: Standard git merge resolution

---

## Rollout Plan

### Stage 1: Foundation (Days 1-2)
- Establish all baselines (norminette, build, performance, functionality)
- Create research.md with refactoring strategies
- Set up validation automation scripts

### Stage 2: Major File Splits (Days 3-5)
- Split files with 7-13 functions (window.c, metrics.c, timer.c, aabb.c, bvh_build.c)
- Create corresponding headers
- Update Makefile incrementally
- Validate after each split

### Stage 3: Function Extraction (Days 6-7)
- Extract long functions (>25 lines) in main.c, render.c, parse_elements.c
- Verify norminette line count compliance
- Validate behavior preservation

### Stage 4: Formatting Fixes (Day 8)
- Fix all header formatting issues (tab/space, trailing whitespace)
- Verify norminette formatting compliance

### Stage 5: Parameter/Variable Reduction (Days 9-10)
- Create parameter structures for functions with >4 parameters
- Refactor functions with >5 local variables
- Update call sites
- Validate behavior and performance

### Stage 6: Final Validation (Day 11)
- Full norminette check (target: 0 errors)
- Comprehensive testing (all scenes)
- Performance benchmark comparison
- Code review and documentation update

### Stage 7: Integration (Day 12)
- Final testing on clean branch
- Merge to main branch
- CI/CD validation
- Document completion in project records

---

## Success Metrics

| Metric | Baseline | Target | Measurement |
|--------|----------|--------|-------------|
| Norminette Errors | 77 | 0 | `norminette src/ includes/ \| grep Error \| wc -l` |
| Files >5 functions | 8 | 0 | Manual count from norminette output |
| Functions >25 lines | 5 | 0 | Manual count from norminette output |
| Build Success | ✅ | ✅ | `make clean && make` exit code |
| Test Scene Pass Rate | 100% | 100% | All scenes render correctly |
| Performance | Baseline | ≤105% | Rendering time comparison |
| Code Readability | N/A | Improved | Code review assessment |

---

## Completion Checklist

- [ ] Phase 0: research.md created with refactoring strategies
- [ ] Phase 1: data-model.md documents existing entities
- [ ] Phase 1: contracts/ defines refactoring rules
- [ ] Phase 1: quickstart.md provides step-by-step workflow
- [ ] Phase 1: Agent context updated with new patterns
- [ ] Constitution check re-validated (all gates passed)
- [ ] All success metrics met
- [ ] Documentation updated (commit messages, code comments)
- [ ] Ready for Phase 2 task breakdown via `/speckit.tasks`

---

**Command Completion**: `/speckit.plan` command stops here.
**Next Step**: Run `/speckit.tasks` to generate implementation task breakdown.

