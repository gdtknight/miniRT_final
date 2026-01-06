# Quick Start Guide: Norminette Compliance Refactoring

**Feature**: Complete Norminette Compliance  
**Branch**: `013-norminette-compliance`  
**Date**: 2026-01-03  
**Phase**: 1 (Design & Contracts)

## Overview

This guide provides a step-by-step workflow for eliminating all 77 norminette errors through systematic refactoring. Follow this guide to safely transform the codebase from 77 errors to 0 errors while preserving all functionality.

**Total Effort Estimate**: 10-12 days  
**Priority Order**: TOO_MANY_FUNCS (P1) → TOO_MANY_LINES (P2) → Formatting (P2) → Parameters/Variables (P3)

---

## Prerequisites

Before starting, ensure you have:

- ✅ norminette installed and in PATH (`norminette --version`)
- ✅ Make build system working (`make clean && make`)
- ✅ Test scenes available in `scenes/` directory
- ✅ Git repository with clean working tree
- ✅ Baseline performance data recorded
- ✅ Sufficient disk space for baseline renders (~100MB)

### Install Dependencies

```bash
# macOS
brew install norminette

# Linux (42 School VM)
python3 -m pip install --user norminette

# Verify installation
norminette --version
```

---

## Phase 0: Establish Baseline

**Duration**: 1-2 hours  
**Goal**: Record current state for comparison

### Step 1: Create Baseline Branch

```bash
# Ensure clean working tree
git status
git stash  # If needed

# Create and checkout feature branch
git checkout -b 013-norminette-compliance

# Tag baseline for reference
git tag baseline-norminette-77-errors
```

### Step 2: Record Norminette Errors

```bash
# Full norminette report
norminette src/ includes/ 2>&1 | tee norminette_baseline.txt

# Count errors by type
grep "Error:" norminette_baseline.txt | cut -d: -f2 | sort | uniq -c | sort -rn

# Expected output:
#  53 TOO_MANY_FUNCS
#   6 TOO_MANY_ARGS
#   5 TOO_MANY_LINES
#   5 TOO_FEW_TAB
#   5 SPC_BEFORE_NL
#   1 TOO_MANY_VARS_FUNC
#   1 TERNARY_FBIDDEN
#   1 PREPROC_CONSTANT
#   1 MISALIGNED_FUNC_DECL
# Total: 77 errors
```

### Step 3: Build Baseline

```bash
# Clean build
make clean
make

# Verify exit code
echo $?  # Should be 0

# Test with simple scene
./miniRT scenes/simple.rt > test.ppm
echo $?  # Should be 0
```

### Step 4: Generate Baseline Renders

```bash
# Create baseline directory
mkdir -p baseline_renders

# Render all test scenes
for scene in scenes/*.rt; do
    scene_name=$(basename "$scene" .rt)
    echo "Rendering baseline: $scene_name"
    ./miniRT "$scene" > "baseline_renders/${scene_name}.ppm" 2>&1
    echo "  Exit code: $?"
done

# Verify all renders completed
ls -lh baseline_renders/
```

### Step 5: Measure Performance Baseline

```bash
# Create performance baseline
echo "=== Performance Baseline ===" > performance_baseline.txt
echo "Scene: scenes/complex_scene.rt" >> performance_baseline.txt
echo "Iterations: 5" >> performance_baseline.txt
echo "" >> performance_baseline.txt

# Run 5 iterations
for i in {1..5}; do
    echo "Iteration $i:"
    { time ./miniRT scenes/complex_scene.rt > /dev/null; } 2>&1 | tee -a performance_baseline.txt
done

# Calculate average (manual or script)
cat performance_baseline.txt
```

---

## Phase 1: File Splitting (Priority 1)

**Duration**: 3-5 days  
**Goal**: Reduce all files to ≤5 functions

### Overview of Files to Split

| File | Current Functions | Target Files | Priority |
|------|-------------------|--------------|----------|
| hud_render.c | 13 | 4 (split to hud/ subdir) | P1-High |
| aabb.c | 9 | 2 files | P1-High |
| metrics.c | 8 | 3 files | P1-High |
| window.c | 7 | 3 files | P1-High |
| render_state.c | 7 | 2 files | P1-High |
| bvh_build.c | 7 | 2 files | P1-Med |
| timer.c | 6 | 2 files | P1-Med |
| render.c | 6 | 2 files | P1-Low |

### Template: Split a File

Use this template for each file split:

#### Step 1.1: Analyze Function Groupings

```bash
# View the file to understand function relationships
cat src/core/metrics.c

# Identify logical groups:
# Group 1: Tracking (init, update, reset, calculate, store)
# Group 2: Display (render, format, draw)
# Group 3: FPS (calculate_fps, smooth_fps)
```

#### Step 1.2: Create New Files

```bash
# Create new source files
touch src/core/metrics_tracking.c
touch src/core/metrics_display.c
touch src/core/metrics_fps.c

# Create corresponding headers
touch includes/metrics_tracking.h
touch includes/metrics_display.h
touch includes/metrics_fps.h
```

#### Step 1.3: Copy 42 Header

```bash
# Copy 42 header from original file to each new file
# Update the filename in the header comment
# Update creation date/time
```

#### Step 1.4: Move Functions

For each new file:

1. **Add includes**:
   ```c
   #include "metrics_tracking.h"
   #include "structures.h"
   #include <stdlib.h>
   ```

2. **Move function implementations** from original file

3. **Create header declarations**:
   ```c
   // includes/metrics_tracking.h
   #ifndef METRICS_TRACKING_H
   # define METRICS_TRACKING_H
   
   # include "structures.h"
   
   void init_metrics(t_metrics *metrics);
   void update_metrics(t_metrics *metrics, double delta);
   void reset_metrics(t_metrics *metrics);
   double calculate_average(t_metrics *metrics);
   void store_sample(t_metrics *metrics, double value);
   
   #endif
   ```

#### Step 1.5: Update Makefile

```makefile
# Find SRCS_CORE section
# Replace:
SRCS_CORE = \
    $(SRC_DIR)/core/metrics.c \
    ...

# With:
SRCS_CORE = \
    $(SRC_DIR)/core/metrics_tracking.c \
    $(SRC_DIR)/core/metrics_display.c \
    $(SRC_DIR)/core/metrics_fps.c \
    ...
```

#### Step 1.6: Update Include Statements

```bash
# Find all files that include metrics.h
grep -r "metrics.h" src/ includes/

# Update includes to use new headers:
# #include "metrics.h" → #include "metrics_tracking.h" (if only uses tracking)
# Or include multiple headers if needed
```

#### Step 1.7: Test Build

```bash
# Compile
make clean
make

# Check for errors
echo $?  # Must be 0

# If errors occur:
# - Check for missing includes
# - Verify function declarations in headers
# - Check for circular dependencies
```

#### Step 1.8: Test Functionality

```bash
# Run simple scene
./miniRT scenes/simple.rt > test_current.ppm
echo $?  # Must be 0

# Quick visual check
diff baseline_renders/simple.ppm test_current.ppm
# Should be identical (exit code 0)
```

#### Step 1.9: Verify Norminette

```bash
# Check new files only
norminette src/core/metrics_tracking.c
norminette src/core/metrics_display.c
norminette src/core/metrics_fps.c
norminette includes/metrics_tracking.h
norminette includes/metrics_display.h
norminette includes/metrics_fps.h

# Count remaining errors
norminette src/ includes/ 2>&1 | grep "Error:" | wc -l
# Should decrease from baseline
```

#### Step 1.10: Commit

```bash
# Stage changes
git add src/core/metrics_*.c
git add includes/metrics_*.h
git add Makefile

# Commit with clear message
git commit -m "refactor: split metrics.c into 3 files for norminette compliance

Split metrics.c (8 functions) into:
- metrics_tracking.c: init, update, reset, calculate, store (5 functions)
- metrics_display.c: render, format, draw (3 functions)  
- metrics_fps.c: merged into tracking (consolidated)

Norminette errors reduced: 77 → 69 (-8 TOO_MANY_FUNCS)
Build: ✓ Tests: ✓"
```

### Repeat for All Files

Apply the above template to each file in the priority order:

1. **hud_render.c** → 4 files in `src/ui/hud/` subdirectory
2. **aabb.c** → `aabb_basic.c`, `aabb_advanced.c`
3. **metrics.c** → (completed above)
4. **window.c** → `window_init.c`, `window_events.c`, `window_hooks.c`
5. **render_state.c** → `render_state.c`, `render_state_update.c`
6. **bvh_build.c** → `bvh_build_core.c`, `bvh_build_helpers.c`
7. **timer.c** → `timer_control.c`, `timer_metrics.c`
8. **render.c** → `render.c`, `render_init.c` or `render_loop.c`

After each file split:
- ✅ Build succeeds
- ✅ Norminette error count decreases
- ✅ Simple scene test passes
- ✅ Git commit created

---

## Phase 2: Function Extraction (Priority 2)

**Duration**: 2-3 days  
**Goal**: Reduce all functions to ≤25 lines

### Functions Exceeding 25 Lines

| Function | File | Current Lines | Strategy |
|----------|------|---------------|----------|
| main() | main.c | 79 | Extract init and cleanup |
| render_loop() | render.c | >25 | Extract rendering stages |
| render_scene() | render.c | >25 | Extract helpers |
| parse_sphere() | parse_elements.c | >25 | Extract validation |
| handle_key() | window.c | >25 | Extract per-key handlers |
| render_hud() | hud_render.c | >25 | Extract HUD sections |

### Template: Extract Helper Functions

#### Example: main() Function

**Before** (79 lines):
```c
int main(int argc, char **argv)
{
    t_scene scene;
    t_window win;
    
    // 15 lines of argument parsing
    // 20 lines of initialization
    // 30 lines of rendering loop setup
    // 10 lines of cleanup
    // 4 lines of return
}
```

**After** (20 lines):
```c
int main(int argc, char **argv)
{
    t_scene scene;
    t_window win;
    
    if (!parse_arguments(argc, argv, &scene))
        return (handle_error("Invalid arguments"));
    
    if (!initialize_scene(&scene, &win))
        return (handle_error("Initialization failed"));
    
    run_rendering_loop(&scene, &win);
    
    cleanup_scene(&scene, &win);
    return (EXIT_SUCCESS);
}

static int parse_arguments(int argc, char **argv, t_scene *scene)
{
    // 15 lines of argument parsing
    return (SUCCESS);
}

static int initialize_scene(t_scene *scene, t_window *win)
{
    // 20 lines of initialization
    return (SUCCESS);
}

static void run_rendering_loop(t_scene *scene, t_window *win)
{
    // 30 lines of rendering loop setup
}

static void cleanup_scene(t_scene *scene, t_window *win)
{
    // 10 lines of cleanup
}
```

### Extraction Steps

1. **Identify extraction boundaries**: Look for blocks that can be logically separated
2. **Create helper function**: Make it `static` unless needed elsewhere
3. **Move code**: Copy block to helper, replace with call
4. **Test**: Build and verify behavior unchanged
5. **Verify norminette**: Check line count with `norminette file.c`

### After Each Extraction

```bash
# Verify norminette
norminette src/path/to/file.c

# Test build
make

# Test functionality
./miniRT scenes/simple.rt > test.ppm
diff baseline_renders/simple.ppm test.ppm

# Commit
git commit -m "refactor: extract helpers from main() to meet 25-line limit"
```

---

## Phase 3: Formatting Fixes (Priority 2)

**Duration**: 1 day  
**Goal**: Fix all header formatting issues

### Files with Formatting Errors

| File | Error Type | Count | Fix |
|------|------------|-------|-----|
| miniRT.h | TOO_FEW_TAB | 2 | Replace spaces with tabs |
| structures.h | TOO_FEW_TAB | 3 | Replace spaces with tabs |
| structures.h | SPC_BEFORE_NL | 5 | Remove trailing whitespace |
| constants.h | PREPROC_CONSTANT | 1 | Fix constant name |
| (various) | MISALIGNED_FUNC_DECL | 1 | Align declaration |

### Fix TOO_FEW_TAB Errors

```bash
# View the file to see spaces vs tabs
cat -A includes/miniRT.h | grep "^ "

# Replace leading spaces with tabs
# In vim: :%s/^    /\t/g (4 spaces → 1 tab)
# In emacs: M-x tabify
# Or use sed: sed -i 's/^    /\t/g' includes/miniRT.h

# Verify fix
norminette includes/miniRT.h | grep TOO_FEW_TAB
# Should return nothing
```

### Fix SPC_BEFORE_NL Errors

```bash
# Remove trailing whitespace
sed -i 's/[[:space:]]*$//' includes/structures.h

# Or in vim: :%s/\s\+$//g

# Verify fix
norminette includes/structures.h | grep SPC_BEFORE_NL
# Should return nothing
```

### Fix PREPROC_CONSTANT Error

```bash
# Find the constant
grep -n "#define" includes/constants.h

# Ensure constant names are UPPER_SNAKE_CASE
# Example: #define maxDepth 5 → #define MAX_DEPTH 5

# Verify fix
norminette includes/constants.h | grep PREPROC_CONSTANT
# Should return nothing
```

### Batch Formatting Script

```bash
#!/bin/bash
# fix_formatting.sh - Fix all header formatting issues

echo "Fixing TOO_FEW_TAB errors..."
for header in includes/*.h; do
    # Replace 4 spaces with 1 tab for indentation
    sed -i.bak 's/^    /\t/g' "$header"
done

echo "Fixing SPC_BEFORE_NL errors..."
for header in includes/*.h; do
    # Remove trailing whitespace
    sed -i.bak 's/[[:space:]]*$//' "$header"
done

echo "Cleaning up backup files..."
rm includes/*.bak

echo "Running norminette check..."
norminette includes/ | grep -E "(TOO_FEW_TAB|SPC_BEFORE_NL)"
echo "Done!"
```

### After Formatting Fixes

```bash
# Verify all formatting fixed
norminette includes/ | grep -E "(TOO_FEW_TAB|SPC_BEFORE_NL)" | wc -l
# Should be 0

# Commit
git add includes/*.h
git commit -m "style: fix header formatting for norminette compliance

Fixed:
- TOO_FEW_TAB: Replaced spaces with tabs in indentation
- SPC_BEFORE_NL: Removed trailing whitespace
- PREPROC_CONSTANT: Fixed constant naming

Norminette errors reduced: X → Y"
```

---

## Phase 4: Parameter/Variable Reduction (Priority 3)

**Duration**: 2-3 days  
**Goal**: Reduce parameters ≤4, variables ≤5

### Functions with Too Many Parameters

| Function | File | Params | Fix Strategy |
|----------|------|--------|--------------|
| traverse_bvh() | bvh_traverse.c | 6 | Create t_bvh_traversal struct |
| build_bvh_node() | bvh_build.c | 5 | Create t_bvh_build_ctx struct |
| draw_keyguide() | keyguide_render.c | 5 | Create t_hud_render_ctx struct |

### Functions with Too Many Variables

| Function | File | Vars | Fix Strategy |
|----------|------|------|--------------|
| render_scene() | render.c | 6 | Group related vars, extract helpers |

### Template: Create Parameter Structure

**Before**:
```c
void traverse_bvh(t_bvh_node *node, t_ray *ray, double t_min, 
                  double t_max, t_hit_record *rec, int depth)
{
    // traversal logic
}
```

**After**:
```c
// In structures.h - add new struct
typedef struct s_bvh_traversal
{
    t_ray           *ray;
    double          t_min;
    double          t_max;
    t_hit_record    *hit;
    int             depth;
}   t_bvh_traversal;

// In bvh_traverse.c - update function
void traverse_bvh(t_bvh_node *node, t_bvh_traversal *ctx)
{
    // traversal logic using ctx->ray, ctx->t_min, etc.
}

// At call site - create context
t_bvh_traversal ctx = {&ray, 0.001, 1000.0, &hit, 0};
traverse_bvh(root, &ctx);
```

### Steps to Add Parameter Structure

1. **Define structure** in `structures.h`
2. **Update function signature** to accept structure pointer
3. **Update function body** to access parameters via structure
4. **Update all call sites** to create and pass structure
5. **Test build** and functionality
6. **Verify norminette** reports no TOO_MANY_ARGS error

### After Each Reduction

```bash
# Verify norminette
norminette src/path/to/file.c | grep "TOO_MANY_ARGS\|TOO_MANY_VARS"

# Test build
make

# Test functionality
./miniRT scenes/complex_scene.rt > test.ppm

# Commit
git commit -m "refactor: reduce traverse_bvh parameters using context struct"
```

---

## Phase 5: Final Validation

**Duration**: 1 day  
**Goal**: Verify 0 errors and full compliance

### Step 5.1: Zero Errors Check

```bash
# Full norminette scan
norminette src/ includes/ 2>&1 | tee norminette_final.txt

# Count errors
grep "Error:" norminette_final.txt | wc -l
# MUST be 0

# If any errors remain, investigate and fix
grep "Error:" norminette_final.txt
```

### Step 5.2: Full Build Test

```bash
# Clean build all targets
make fclean
make
make bonus

# All must succeed (exit code 0)
echo "make: $?"
```

### Step 5.3: Comprehensive Scene Testing

```bash
# Create current renders directory
mkdir -p current_renders

# Render all scenes
for scene in scenes/*.rt; do
    scene_name=$(basename "$scene" .rt)
    echo "Testing: $scene_name"
    
    ./miniRT "$scene" > "current_renders/${scene_name}.ppm" 2>&1
    exit_code=$?
    
    if [ $exit_code -ne 0 ]; then
        echo "  ✗ FAILED with exit code $exit_code"
    else
        echo "  ✓ Rendered successfully"
    fi
done

# Compare all renders
echo ""
echo "=== Image Comparison ==="
for scene in scenes/*.rt; do
    scene_name=$(basename "$scene" .rt)
    
    if diff -q "baseline_renders/${scene_name}.ppm" "current_renders/${scene_name}.ppm" > /dev/null; then
        echo "✓ $scene_name: IDENTICAL"
    else
        echo "✗ $scene_name: DIFFERS - Manual verification required"
    fi
done
```

### Step 5.4: Performance Benchmark

```bash
# Measure current performance
echo "=== Performance Final ===" > performance_final.txt
echo "Scene: scenes/complex_scene.rt" >> performance_final.txt
echo "Iterations: 5" >> performance_final.txt
echo "" >> performance_final.txt

for i in {1..5}; do
    echo "Iteration $i:"
    { time ./miniRT scenes/complex_scene.rt > /dev/null; } 2>&1 | tee -a performance_final.txt
done

# Compare with baseline
echo ""
echo "=== Performance Comparison ==="
echo "Baseline (from performance_baseline.txt):"
grep "real" performance_baseline.txt
echo ""
echo "Final (from performance_final.txt):"
grep "real" performance_final.txt
echo ""
echo "Calculate regression percentage manually or with script"
```

### Step 5.5: Memory Leak Check (Optional)

```bash
# If valgrind is available
valgrind --leak-check=full --error-exitcode=1 ./miniRT scenes/simple.rt > /dev/null

if [ $? -eq 0 ]; then
    echo "✓ No memory leaks detected"
else
    echo "✗ Memory leaks found - review valgrind output"
fi
```

### Step 5.6: Code Review

Manually review:
- ✅ Function count per file (≤5)
- ✅ Function line count (≤25)
- ✅ Parameter count (≤4)
- ✅ Variable count (≤5)
- ✅ Code readability improved
- ✅ Comments and documentation present
- ✅ No logic changes or behavior modifications

---

## Phase 6: Documentation & Merge

**Duration**: 0.5 days  
**Goal**: Document changes and merge to main

### Step 6.1: Generate Change Summary

```bash
# Count commits
git log --oneline baseline-norminette-77-errors..HEAD | wc -l

# Generate summary
git log --oneline baseline-norminette-77-errors..HEAD > refactoring_commits.txt

# Create summary document
cat > NORMINETTE_COMPLIANCE_SUMMARY.md << 'EOF'
# Norminette Compliance - Implementation Summary

## Objective
Eliminate all 77 norminette errors to achieve 100% compliance with 42 School coding standards.

## Results
- **Baseline Errors**: 77
- **Final Errors**: 0
- **Files Modified**: [count]
- **Files Created**: [count]
- **Commits**: [count]

## Changes by Category

### File Splits (TOO_MANY_FUNCS)
- metrics.c → 3 files
- window.c → 3 files
- [list all splits]

### Function Extractions (TOO_MANY_LINES)
- main(): 79 → 20 lines
- [list all extractions]

### Formatting Fixes
- Fixed TOO_FEW_TAB in headers
- Removed trailing whitespace
- [list all formatting fixes]

### Parameter/Variable Reductions
- Created t_bvh_traversal struct
- [list all reductions]

## Validation

### Build: ✅ PASSED
All build targets succeed without errors or warnings.

### Functionality: ✅ PASSED
All test scenes render identically to baseline (verified via image diff).

### Performance: ✅ PASSED
Rendering performance within X% of baseline (threshold: ≤5%).

### Norminette: ✅ PASSED
Zero errors reported across all source and header files.

## Files Created
[List all new source and header files]

## Makefile Changes
Updated SRCS variables to include all new source files.

## Compliance Achievement Date
[Date]

## Next Steps
- Merge feature branch to main
- Update project documentation
- Close related issues
EOF
```

### Step 6.2: Final Commit

```bash
# Stage all final changes
git add .

# Create comprehensive final commit
git commit -m "docs: complete norminette compliance refactoring

Summary:
- Eliminated all 77 norminette errors (77 → 0)
- Split 8 files with >5 functions into 20+ organized files
- Extracted long functions (>25 lines) into focused helpers
- Fixed all header formatting issues
- Reduced function parameters/variables using structures

Changes:
- Files split: metrics.c, window.c, timer.c, aabb.c, bvh_build.c, 
  render_state.c, render.c, hud_render.c
- Helper functions extracted in: main.c, render.c, parse_elements.c
- Formatting fixed in: miniRT.h, structures.h, constants.h
- Parameter structures added: t_bvh_traversal, t_bvh_build_ctx, 
  t_hud_render_ctx, t_render_config, t_state_update

Validation:
- Build: ✅ All targets compile successfully
- Functionality: ✅ All test scenes identical to baseline
- Performance: ✅ Within 5% of baseline
- Norminette: ✅ Zero errors

Branch: 013-norminette-compliance
Closes: #[issue number if applicable]"
```

### Step 6.3: Push and Create PR

```bash
# Push feature branch
git push origin 013-norminette-compliance

# Create pull request via GitHub CLI or web interface
gh pr create --title "Complete norminette compliance (77 → 0 errors)" \
             --body "$(cat NORMINETTE_COMPLIANCE_SUMMARY.md)"
```

### Step 6.4: PR Checklist

Verify PR includes:
- ✅ Clear description of changes
- ✅ Before/after norminette error counts
- ✅ Validation results (build, tests, performance)
- ✅ List of new files created
- ✅ Makefile updates documented
- ✅ CI/CD checks pass (if configured)

---

## Troubleshooting

### Build Failures

**Problem**: Compiler errors after splitting files
```
undefined reference to `function_name'
```

**Solution**:
1. Check function is declared in header file
2. Check header is included in calling file
3. Check function is added to Makefile SRCS
4. Verify no circular dependencies

### Image Differences

**Problem**: Rendered images differ from baseline

**Solution**:
1. Review git diff to identify logic changes
2. Verify order of operations hasn't changed
3. Check for unintentional variable reuse
4. Test with simpler scene to isolate issue

### Performance Regression

**Problem**: Rendering >5% slower than baseline

**Solution**:
1. Profile with `time` or profiling tools
2. Identify hot spots (likely in ray tracing loop)
3. Add `static inline` to frequently called helpers
4. Consider consolidating some extracted functions

### Norminette Errors Persist

**Problem**: Still getting errors after fixes

**Solution**:
1. Re-run norminette on specific file: `norminette file.c`
2. Check line count includes only { to } range
3. Verify tabs vs spaces (use `cat -A file.c`)
4. Ensure parameter structures count as 1 parameter

---

## Success Criteria Checklist

Before declaring completion:

- [ ] norminette reports exactly 0 errors
- [ ] All source files have ≤5 functions
- [ ] All functions have ≤25 lines
- [ ] All functions have ≤4 parameters
- [ ] All functions have ≤5 local variables
- [ ] All headers properly formatted (tabs, no trailing spaces)
- [ ] Build succeeds: `make clean && make && make bonus`
- [ ] All test scenes render correctly
- [ ] Image diffs show no changes from baseline
- [ ] Performance within ±5% of baseline
- [ ] All new files have 42 headers
- [ ] All functions have documentation comments
- [ ] Makefile updated with all new sources
- [ ] Git history is clean and well-documented
- [ ] Code review completed
- [ ] PR approved and ready to merge

---

## Quick Reference Commands

### Check Norminette
```bash
norminette src/ includes/ 2>&1 | grep "Error:" | wc -l
```

### Build and Test
```bash
make clean && make && ./miniRT scenes/simple.rt > test.ppm && diff baseline_renders/simple.ppm test.ppm
```

### Count Functions in File
```bash
grep -c "^[a-z_].*(" src/core/metrics.c
```

### Count Lines in Function
```bash
# Manually count from opening { to closing }
sed -n '/^function_name(/,/^}/p' file.c | wc -l
```

### Find Files with >5 Functions
```bash
for file in src/**/*.c; do
    count=$(grep -c "^[a-z_].*(" "$file")
    if [ $count -gt 5 ]; then
        echo "$file: $count functions"
    fi
done
```

---

**Quick Start Guide Complete**: Follow this workflow to achieve 0 norminette errors safely and systematically.
