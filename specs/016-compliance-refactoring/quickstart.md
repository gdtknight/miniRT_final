# Quickstart Guide: Compliance Refactoring

**Feature**: 016-compliance-refactoring  
**Date**: 2026-01-14  
**Audience**: Developers working on miniRT compliance refactoring

## Overview

This guide helps developers quickly get started with the miniRT compliance refactoring project. The refactoring ensures the codebase meets 42 School evaluation requirements: norminette compliance, forbidden function elimination, and code deduplication.

**Estimated Time**: 8-11 days  
**Complexity**: Medium (requires attention to detail)  
**Prerequisites**: Understanding of C programming, norminette standards, and git workflow

## Quick Start (5 Minutes)

### 1. Setup Your Environment

```bash
# Clone the repository (if not already done)
git clone <repo-url>
cd miniRT

# Verify tools are installed
norminette --version   # Should be 3.3.50+
make --version         # GNU Make 3.81+
valgrind --version     # Memory leak checker (Linux)
# or
leaks -h              # Memory leak checker (macOS)

# Check current compliance status
norminette src/ includes/ | grep -c "Error"  # Goal: 0
grep -rn "memset\|memcpy\|memmove" src/      # Goal: No matches
```

### 2. Create Feature Branch

```bash
# Create and switch to feature branch
git checkout -b 016-compliance-refactoring

# Verify branch
git branch
# Should show: * 016-compliance-refactoring
```

### 3. Run Baseline Validation

```bash
# Build current version
make clean && make

# Generate baseline renders for pixel-diff validation
mkdir -p tests/baselines
for scene in scenes/*.rt; do
    scene_name=$(basename "$scene" .rt)
    ./miniRT "$scene" > "tests/baselines/${scene_name}.ppm" 2>&1
done

# Create baseline metadata
echo "{\"generated\": \"$(date -I)\", \"commit\": \"$(git rev-parse HEAD)\"}" \
    > tests/baselines/metadata.json

echo "✅ Baseline captured - ready to start refactoring"
```

## Development Workflow

### Daily Workflow Loop

```bash
# 1. Pick a phase (see Phase Guide below)
# 2. Make changes to relevant files
# 3. Build and test
make clean && make

# 4. Run norminette on changed files
norminette <changed_file.c>

# 5. Test with key scenes
./miniRT scenes/mandatory/sphere.rt
./miniRT scenes/mandatory/plane.rt
./miniRT scenes/mandatory/cylinder.rt

# 6. Memory check (run on 1-2 scenes)
valgrind --leak-check=full ./miniRT scenes/mandatory/sphere.rt

# 7. Commit if validation passes
git add <changed_files>
git commit -m "refactor(module): description"

# 8. Repeat until phase complete
```

### Validation Checklist

Before marking any phase complete, verify:

- [ ] Code compiles without errors or warnings
- [ ] `norminette` passes on all changed files
- [ ] No forbidden functions in changed files
- [ ] Memory leaks = 0 bytes
- [ ] Test scenes render successfully
- [ ] Visual output matches baseline (manual check for key scenes)

## Phase Guide

### Phase 0: Baseline Capture (Day 1 - Morning)

**Goal**: Capture current state for regression detection

**Tasks**:
```bash
# Already done in Quick Start above
# Verify baseline exists
ls tests/baselines/*.ppm
# Should show 38 PPM files (one per scene)
```

**Completion Criteria**: 38 baseline images generated

---

### Phase 1: Duplicate Detection (Day 1 - Afternoon)

**Goal**: Identify files with 100% identical content

**Tasks**:
```bash
# Generate hashes
cd src
find . -name "*.c" -type f -exec sha256sum {} \; | sort > /tmp/hashes.txt

# Find duplicates
awk '{print $1}' /tmp/hashes.txt | uniq -d > /tmp/dup_hashes.txt

# List duplicate files
echo "=== Duplicate Files ==="
while read hash; do
    echo "Hash: $hash"
    grep "^$hash" /tmp/hashes.txt | awk '{print "  - " $2}'
    echo
done < /tmp/dup_hashes.txt
```

**Completion Criteria**: 
- List of duplicate file groups generated
- Document findings in `specs/016-compliance-refactoring/tracking/duplicates.json`

---

### Phase 2: Duplicate Consolidation (Day 1 - Afternoon)

**Goal**: Remove duplicate files, keep canonical versions

**Tasks**:
```bash
# For each duplicate group:
# 1. Choose canonical file (better name/location)
# 2. Update Makefile to remove duplicate from compilation
# 3. Remove duplicate file
# 4. Update any includes if necessary
# 5. Test build

# Example:
# If src/utils/helper.c and src/helpers/util.c are identical:
# Keep: src/utils/helper.c (better module organization)
# Remove: src/helpers/util.c

git rm src/helpers/util.c
# Edit Makefile to remove reference
make clean && make
./miniRT scenes/mandatory/sphere.rt  # Verify still works

git commit -m "refactor: consolidate duplicate files

Removed src/helpers/util.c (duplicate of src/utils/helper.c)
Updated Makefile to reference canonical version only."
```

**Completion Criteria**:
- 0 duplicate files (verified by re-running hash check)
- Build succeeds
- Basic functionality verified

---

### Phase 3: Forbidden Functions Audit (Day 2 - Morning)

**Goal**: Locate all uses of forbidden functions

**Tasks**:
```bash
# Search for forbidden functions
grep -rn "memset\|memcpy\|memmove\|strcpy\|strcat" src/ includes/

# Current known violations:
# src/bvh_vis/bvh_vis_prefix.c: 2x memcpy
# src/main.c: 1x memset

# Document each violation:
# - File path
# - Line number
# - Function being called
# - Context (what it's used for)
# - Proposed replacement

# Save to tracking file
cat > specs/016-compliance-refactoring/tracking/forbidden_usage.json <<EOF
[
  {
    "function": "memcpy",
    "file": "src/bvh_vis/bvh_vis_prefix.c",
    "line": 45,
    "context": "Buffer reallocation",
    "replacement": "ft_memcpy"
  },
  {
    "function": "memcpy", 
    "file": "src/bvh_vis/bvh_vis_prefix.c",
    "line": 47,
    "context": "Append prefix to buffer",
    "replacement": "ft_memcpy"
  },
  {
    "function": "memset",
    "file": "src/main.c",
    "line": 123,
    "context": "Initialize scene structure",
    "replacement": "ft_memset"
  }
]
EOF
```

**Completion Criteria**: 
- All forbidden function usages documented
- Replacement strategy defined for each

---

### Phase 4: Function Replacement (Day 2 - Afternoon)

**Goal**: Implement custom utilities and replace forbidden functions

**Tasks**:
```bash
# 1. Create custom utilities module
mkdir -p src/utils
touch src/utils/memory.c

# 2. Implement ft_memcpy and ft_memset
# (See contracts/refactoring-contracts.md for implementation)

cat > src/utils/memory.c <<'EOF'
#include "utils.h"

void *ft_memcpy(void *dst, const void *src, size_t n)
{
    size_t          i;
    unsigned char   *d;
    unsigned char   *s;

    if (!dst && !src)
        return (NULL);
    d = (unsigned char *)dst;
    s = (unsigned char *)src;
    i = 0;
    while (i < n)
    {
        d[i] = s[i];
        i++;
    }
    return (dst);
}

void *ft_memset(void *b, int c, size_t len)
{
    size_t          i;
    unsigned char   *ptr;

    ptr = (unsigned char *)b;
    i = 0;
    while (i < len)
    {
        ptr[i] = (unsigned char)c;
        i++;
    }
    return (b);
}
EOF

# 3. Add header declarations
cat >> includes/utils.h <<'EOF'
#ifndef UTILS_H
# define UTILS_H

# include <stddef.h>

void *ft_memcpy(void *dst, const void *src, size_t n);
void *ft_memset(void *b, int c, size_t len);

#endif
EOF

# 4. Replace usages one by one
# In each file with violations:
# - Add #include "utils.h"
# - Replace memcpy → ft_memcpy
# - Replace memset → ft_memset

# 5. Update Makefile to compile memory.c

# 6. Test each replacement
make clean && make
valgrind --leak-check=full ./miniRT scenes/mandatory/sphere.rt

# 7. Commit after each successful replacement
git add src/utils/memory.c includes/utils.h
git commit -m "feat(utils): add custom memory utilities

Implement ft_memcpy and ft_memset to replace forbidden functions.
Compliant with 42 School allowed functions list."

git add src/bvh_vis/bvh_vis_prefix.c
git commit -m "refactor(bvh_vis): replace memcpy with ft_memcpy

Replaced 2 memcpy calls with ft_memcpy for compliance.
Behavior preserved, validated with BVH test scenes."
```

**Completion Criteria**:
- 0 forbidden function usages (grep check passes)
- Custom utilities implemented and tested
- All replacements validated with memory checks

---

### Phase 5: Norminette First Pass (Day 3 - Morning)

**Goal**: Audit all files and categorize violations

**Tasks**:
```bash
# Run full norminette audit
norminette src/ includes/ > /tmp/norminette_report.txt

# Analyze violations by type
grep "TOO_MANY_LINES" /tmp/norminette_report.txt | wc -l
grep "TOO_MANY_ARGS" /tmp/norminette_report.txt | wc -l
grep "VAR_DECL_START_FUNC" /tmp/norminette_report.txt | wc -l

# Categorize files by complexity
# Easy: < 5 violations
# Medium: 5-15 violations
# Hard: > 15 violations

# Prioritize: Start with Easy files for quick wins
```

**Completion Criteria**:
- Full violation report generated
- Files categorized by complexity
- Work plan for Phases 6-8 created

---

### Phase 6: Function Splitting (Days 3-5)

**Goal**: Split functions exceeding 25 lines into helpers

**Tasks** (per function):
```bash
# 1. Identify long function (from norminette report)
# 2. Analyze logical blocks within function
# 3. Extract helper functions (static by default)
# 4. Update original function to call helpers
# 5. Test and validate

# Example refactoring:
# Before: render_scene() - 45 lines
# After:
#   - render_scene() - 12 lines (main orchestration)
#   - static init_render_context() - 15 lines
#   - static process_pixels() - 18 lines

# Pattern:
# - Helpers should perform complete sub-tasks
# - Maintain clear data flow
# - Keep helpers cohesive (one purpose)

# 6. Run norminette on modified file
norminette src/render/scene.c

# 7. Test functionality
./miniRT scenes/mandatory/sphere.rt

# 8. Commit
git add src/render/scene.c
git commit -m "refactor(render): split render_scene to meet 25-line limit"
```

**Completion Criteria**:
- All functions ≤ 25 lines
- `TOO_MANY_LINES` violations = 0
- Functionality preserved

---

### Phase 7: Parameter Reduction (Days 5-6)

**Goal**: Reduce functions with >4 parameters

**Tasks** (per function):
```bash
# 1. Identify function with >4 params
# 2. Group related parameters into context structure
# 3. Define structure in appropriate header
# 4. Update function signature
# 5. Update all call sites
# 6. Test and validate

# Example:
# Before: render_pixel(scene, cam, x, y, output, cfg)  # 6 params
# After:  render_pixel(ctx, x, y, output)              # 4 params
#   with: typedef struct { scene, cam, cfg } t_render_ctx

# 7. Run norminette
norminette src/render/pixel.c includes/render.h

# 8. Commit
git add src/render/pixel.c includes/render.h
git commit -m "refactor(render): reduce render_pixel parameters with context struct"
```

**Completion Criteria**:
- All functions ≤ 4 parameters
- `TOO_MANY_ARGS` violations = 0
- Functionality preserved

---

### Phase 8: Style Fixes (Day 7)

**Goal**: Fix remaining style violations (variable declarations, formatting)

**Tasks**:
```bash
# Common fixes:
# - Move variable declarations to function start
# - Fix indentation/spacing
# - Add missing empty lines
# - Fix line length (80 columns)
# - Fix function declarations

# Run norminette frequently
norminette <file>

# Fix violations systematically
# Commit after each file or small group of related files
```

**Completion Criteria**:
- norminette passes on 100% of files
- `norminette src/ includes/ | grep Error` returns nothing

---

### Phase 9: Full Validation (Day 8)

**Goal**: Comprehensive testing and validation

**Tasks**:
```bash
# 1. Clean build
make fclean && make

# 2. Norminette validation
norminette src/ includes/
# Expected: All files "OK!"

# 3. Forbidden functions check
grep -rn "memset\|memcpy\|memmove" src/ includes/
# Expected: No matches

# 4. Build verification
make clean
make  # Should succeed
make  # Should not relink (up to date)
make clean  # Should remove *.o only
ls miniRT  # Should still exist
make fclean  # Should remove *.o and miniRT
ls miniRT  # Should not exist

# 5. Memory leak validation (all scenes)
for scene in scenes/*.rt; do
    echo "Testing: $scene"
    valgrind --leak-check=full --error-exitcode=1 \
        ./miniRT "$scene" 2>&1 | tail -5
done

# 6. Pixel-diff validation (if automation ready)
./scripts/pixel_diff.sh
# Expected: All <0.1% difference

# 7. Manual testing
# - Test window management (ESC, close button)
# - Test error handling (invalid scenes)
# - Verify visual quality unchanged
```

**Completion Criteria**:
- All automated checks pass
- Manual verification complete
- CI/CD pipeline green (if available)

---

### Phase 10: Documentation Update (Day 9)

**Goal**: Update all documentation to reflect changes

**Tasks**:
```bash
# 1. Update README.md if needed
# 2. Document new utility functions
# 3. Update code comments for refactored functions
# 4. Create refactoring summary document
# 5. Update change log

# Example documentation:
cat >> docs/CHANGELOG.md <<EOF
## Compliance Refactoring (2026-01-14)

### Changes
- Achieved 100% norminette compliance (75 files)
- Eliminated all forbidden function usages (3 violations fixed)
- Consolidated duplicate source files
- Improved code organization with helper functions
- Added custom memory utilities (ft_memcpy, ft_memset)

### Validation
- All 38 test scenes pass
- Zero memory leaks confirmed
- Visual output preserved (<0.1% pixel difference)
- Build system verified (all targets functional)

### Files Modified
- See commit history in branch 016-compliance-refactoring
EOF

git add docs/CHANGELOG.md
git commit -m "docs: document compliance refactoring changes"
```

**Completion Criteria**:
- All documentation updated
- Change log complete
- Code comments reviewed

## Common Issues & Solutions

### Issue: Norminette Error - Function Too Long

**Solution**: Extract logical blocks into static helper functions

```c
// Before
int complex_function(t_data *data)
{
    // 40 lines of code
}

// After
static int helper_1(t_data *data) { /* ... */ }
static int helper_2(t_data *data) { /* ... */ }

int complex_function(t_data *data)
{
    if (helper_1(data) < 0)
        return (-1);
    return (helper_2(data));
}
```

### Issue: Norminette Error - Too Many Parameters

**Solution**: Use context structure

```c
// Before
void func(int a, int b, int c, int d, int e)

// After
typedef struct s_context { int a, b, c, d, e; } t_context;
void func(t_context *ctx)
```

### Issue: Memory Leak After Refactoring

**Solution**: Check all malloc/free pairs, especially in error paths

```bash
# Debug with valgrind
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes \
         ./miniRT scenes/test.rt
```

### Issue: Pixel-Diff Fails After Refactoring

**Solution**: Check for logic changes in refactored functions

```bash
# Compare renders visually
./miniRT scenes/sphere.rt > new.ppm
diff tests/baselines/sphere.ppm new.ppm
# If significant differences, review refactored code carefully
```

### Issue: Build Fails After Removing Duplicate

**Solution**: Update Makefile and check all includes

```bash
# Find references to removed file
grep -r "removed_file.h" src/ includes/
# Update to reference canonical file instead
```

## Testing Commands Reference

```bash
# Quick validation (run frequently)
make clean && make && norminette src/changed_file.c && ./miniRT scenes/mandatory/sphere.rt

# Full validation (run before committing)
make fclean && make && norminette src/ includes/ && valgrind --leak-check=full ./miniRT scenes/mandatory/sphere.rt

# Memory check all scenes (run after major changes)
for scene in scenes/*.rt; do valgrind --leak-check=full --error-exitcode=1 ./miniRT "$scene" 2>&1 | grep "ERROR SUMMARY"; done

# Forbidden function check (run before final commit)
grep -rn "memset\|memcpy\|memmove\|strcpy\|strcat" src/ includes/
```

## Git Workflow Reference

```bash
# Create feature branch (done once)
git checkout -b 016-compliance-refactoring

# Regular commits (atomic, descriptive)
git add <files>
git commit -m "refactor(module): brief description"

# Tag validation milestones
git tag -a v0.1-duplicates-removed -m "Duplicate files consolidated"
git tag -a v0.2-forbidden-funcs-fixed -m "Forbidden functions replaced"
git tag -a v0.3-norminette-pass -m "Full norminette compliance"

# Push to remote
git push origin 016-compliance-refactoring
git push origin --tags

# Create pull request (when complete)
# Use GitHub/GitLab interface to create PR from branch
```

## Success Verification

Before marking the refactoring complete, verify:

```bash
# 1. Norminette (100% pass)
norminette src/ includes/ | grep -c "OK!" 
# Expected: Count matches number of files

norminette src/ includes/ | grep -c "Error"
# Expected: 0

# 2. Forbidden functions (0 usages)
grep -rn "memset\|memcpy\|memmove" src/ includes/ | wc -l
# Expected: 0

# 3. Build (clean, no warnings)
make fclean && make 2>&1 | grep -i warning | wc -l
# Expected: 0

# 4. No-relink test
make && make
# Expected: "Nothing to be done" or "up to date"

# 5. Memory (0 leaks on all scenes)
for scene in scenes/*.rt; do 
    valgrind --leak-check=full --error-exitcode=1 ./miniRT "$scene" 2>&1 | grep "definitely lost" | grep -v "0 bytes"
done
# Expected: No output

# 6. Functionality (all scenes render)
failed=0
for scene in scenes/*.rt; do
    ./miniRT "$scene" || failed=$((failed + 1))
done
echo "Failed scenes: $failed"
# Expected: 0
```

If all checks pass: **Refactoring complete! 🎉**

## Next Steps

After completing the refactoring:

1. Create pull request for code review
2. Address any feedback from reviewers
3. Merge to main branch
4. Tag release (e.g., `v1.0.0-compliant`)
5. Update project documentation
6. Prepare for 42 School evaluation

## Resources

- **Norminette**: https://github.com/42School/norminette
- **miniRT Subject**: `miniRT.pdf` in repository root
- **Evaluation Checklist**: `miniRT_eval.pdf` in repository root
- **Feature Spec**: `specs/016-compliance-refactoring/spec.md`
- **Implementation Plan**: `specs/016-compliance-refactoring/plan.md`
- **Research Document**: `specs/016-compliance-refactoring/research.md`
- **Contracts**: `specs/016-compliance-refactoring/contracts/`

## Getting Help

If you encounter issues:

1. Check "Common Issues & Solutions" section above
2. Review research.md for detailed strategies
3. Consult contracts/refactoring-contracts.md for behavior specifications
4. Review git history for similar refactorings: `git log --grep="refactor"`
5. Ask for peer review if uncertain about refactoring approach

---

**Happy refactoring! May your norminette always return "OK!" 🚀**
