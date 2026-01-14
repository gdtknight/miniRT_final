# Phase 0: Research & Analysis

**Feature**: 016-compliance-refactoring  
**Date**: 2026-01-14  
**Status**: Complete

## Executive Summary

This research document addresses all technical unknowns and provides the foundation for implementing a comprehensive compliance refactoring of the miniRT project. The refactoring targets 3 critical compliance areas: norminette style compliance, forbidden function replacement, and code deduplication.

**Key Findings**:
- **Codebase Scope**: 75 source files (.c) requiring norminette compliance
- **Forbidden Functions**: 3 confirmed violations (2x memcpy, 1x memset)
- **Test Coverage**: 38 scene files for regression validation
- **Norminette Version**: 3.3.50 confirmed available
- **Automation Strategy**: Pixel-diff validation with <0.1% threshold

## 1. Norminette Compliance Strategy

### Decision: Automated Incremental Refactoring Approach

**Rationale**:
The miniRT codebase contains 75 C source files that must pass norminette 3.3.50. Attempting manual simultaneous refactoring would introduce high risk of functional regressions. An automated, incremental approach with continuous validation provides the safest path.

**Implementation Strategy**:
1. **Baseline Assessment**: Run `norminette src/ includes/` to generate violation report
2. **Categorize Violations**: Group by type (function length, parameter count, variable declarations, formatting)
3. **Priority Order**: Fix structural issues first (function length > parameters > variables > formatting)
4. **Validation Loop**: After each module fix → build → test scenes → pixel-diff → memory check

**Alternatives Considered**:
- **Manual file-by-file refactoring**: Rejected due to high error rate and time cost
- **Automated code transformation tools**: Rejected as norminette violations require context-aware fixes
- **Big-bang refactoring**: Rejected due to regression risk

### Common Norminette Patterns & Solutions

#### Pattern 1: Functions Exceeding 25 Lines

**Solution**: Extract helper functions with meaningful names
```c
// Before: 40-line function
int complex_operation(t_data *data, int param)
{
    // ... 40 lines of code
}

// After: Split into helpers
static void helper_step_one(t_data *data)
{
    // First logical block (8 lines)
}

static void helper_step_two(t_data *data, int param)
{
    // Second logical block (10 lines)
}

int complex_operation(t_data *data, int param)
{
    helper_step_one(data);
    helper_step_two(data, param);
    return (final_result);
}
```

**Guideline**: Helper functions should be `static` by default (as per FR-037), promoted to non-static only when needed across multiple files.

#### Pattern 2: Functions with >4 Parameters

**Solution**: Use configuration structures or simplify interface
```c
// Before: 6 parameters
void render_pixel(t_scene *scene, t_camera *cam, int x, int y, 
                  t_color *output, t_config *cfg)

// After: Consolidate parameters
typedef struct s_render_context
{
    t_scene     *scene;
    t_camera    *cam;
    t_config    *cfg;
}   t_render_context;

void render_pixel(t_render_context *ctx, int x, int y, t_color *output)
```

#### Pattern 3: Variable Declarations

**Solution**: Move all declarations to function start
```c
// Before: Declarations scattered
int function(void)
{
    int result;
    result = calculate();
    int temp = result * 2;  // VIOLATION: declaration after statement
    return (temp);
}

// After: All declarations at start
int function(void)
{
    int result;
    int temp;
    
    result = calculate();
    temp = result * 2;
    return (temp);
}
```

## 2. Forbidden Function Replacement

### Decision: Custom Utility Functions in Dedicated Module

**Rationale**:
The miniRT.pdf allows a specific set of functions. Using memcpy, memset, memmove violates evaluation requirements and results in automatic failure. Custom implementations must provide byte-for-byte identical behavior while remaining simple enough to verify correctness.

**Current Violations**:
1. `src/bvh_vis/bvh_vis_prefix.c`: 2x memcpy (line-by-line buffer copying)
2. `src/main.c`: 1x memset (structure initialization)

**Implementation Plan**:

### Custom Memory Utilities Module

**File**: `src/utils/memory.c` + `includes/utils.h`

```c
// includes/utils.h
#ifndef UTILS_H
# define UTILS_H

# include <stddef.h>

void    *ft_memcpy(void *dst, const void *src, size_t n);
void    *ft_memset(void *b, int c, size_t len);

#endif

// src/utils/memory.c
#include "utils.h"

void    *ft_memcpy(void *dst, const void *src, size_t n)
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

void    *ft_memset(void *b, int c, size_t len)
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
```

**Validation Strategy**:
1. Unit tests comparing ft_memcpy/ft_memset output to libc versions
2. Replace violations one-by-one, test after each replacement
3. Run full test suite after all replacements
4. Verify pixel-diff <0.1% on all 38 scenes

**Alternatives Considered**:
- **Inline loops at call site**: Rejected due to code duplication
- **Use bzero instead of memset**: Rejected as bzero also needs verification against allowed list
- **Aggregate initialization for memset**: Considered but not applicable for dynamic buffers

## 3. Code Deduplication

### Decision: Hash-Based Exact Duplicate Detection (100% Match Only)

**Rationale**:
As clarified in spec.md, only files with 100% byte-for-byte identical content (verified by MD5/SHA256 hash) qualify as duplicates. This strict criterion prevents accidental consolidation of similar-but-different implementations.

**Detection Process**:
```bash
# Generate hashes for all source files
find src -name "*.c" -type f -exec sha256sum {} \; | sort > /tmp/hashes.txt

# Find duplicates
awk '{print $1}' /tmp/hashes.txt | uniq -d > /tmp/duplicate_hashes.txt

# Map duplicates to filenames
while read hash; do
    grep "^$hash" /tmp/hashes.txt | awk '{print $2}'
done < /tmp/duplicate_hashes.txt
```

**Consolidation Strategy**:
1. Identify duplicate sets (files with identical hashes)
2. Choose canonical version based on:
   - More descriptive filename
   - Better module location
   - More recent relevant commits (if functional differences exist)
3. Update all references (Makefile, includes)
4. Remove duplicate files
5. Document in commit message

**Verification**:
- Build must succeed after consolidation
- All 38 test scenes must pass
- No missing symbols errors
- Pixel-diff <0.1% validation

**Alternatives Considered**:
- **Similarity-based detection (>90%)**: Rejected per spec clarification
- **Manual code review**: Used as supplement but not primary method
- **AST-based comparison**: Rejected as overkill for exact duplicate detection

## 4. Automated Validation Framework

### Decision: Pixel-Diff Tool with <0.1% Threshold + Memory Checks

**Rationale**:
Visual regression testing ensures refactoring doesn't alter rendering behavior. A strict <0.1% difference threshold catches even subtle rendering bugs while allowing for floating-point rounding differences.

**Tool Implementation**:

```bash
# scripts/pixel_diff.sh
#!/bin/bash
# Generates baseline renders and compares against current output

BASELINE_DIR="tests/baselines"
CURRENT_DIR="tests/current"
SCENES_DIR="scenes"
THRESHOLD="0.1"  # 0.1% maximum pixel difference

mkdir -p "$BASELINE_DIR" "$CURRENT_DIR"

# Generate or compare renders
for scene in "$SCENES_DIR"/*.rt; do
    scene_name=$(basename "$scene" .rt)
    
    if [ "$1" = "baseline" ]; then
        ./miniRT "$scene" --save "$BASELINE_DIR/${scene_name}.ppm"
    else
        ./miniRT "$scene" --save "$CURRENT_DIR/${scene_name}.ppm"
        
        # Compare using ImageMagick or custom tool
        diff_pct=$(compare -metric RMSE \
            "$BASELINE_DIR/${scene_name}.ppm" \
            "$CURRENT_DIR/${scene_name}.ppm" \
            /dev/null 2>&1 | awk '{print $1 * 100}')
        
        if (( $(echo "$diff_pct > $THRESHOLD" | bc -l) )); then
            echo "FAIL: $scene_name - ${diff_pct}% difference"
            exit 1
        else
            echo "PASS: $scene_name - ${diff_pct}% difference"
        fi
    fi
done
```

**Memory Validation**:
```bash
# scripts/memory_check.sh
#!/bin/bash
# Runs all scenes through valgrind

for scene in scenes/*.rt; do
    echo "Testing: $scene"
    valgrind --leak-check=full --error-exitcode=1 \
        ./miniRT "$scene" 2>&1 | grep -E "ERROR SUMMARY|definitely lost"
    if [ $? -ne 0 ]; then
        echo "FAIL: Memory leak in $scene"
        exit 1
    fi
done
echo "All memory checks passed"
```

**Workflow Integration**:
1. **Before refactoring**: Generate baselines with `pixel_diff.sh baseline`
2. **After each phase**: Run `pixel_diff.sh` to compare
3. **After each module**: Run `memory_check.sh`
4. **Final validation**: Full test suite + pixel-diff + memory check

**Alternatives Considered**:
- **Manual visual inspection**: Rejected due to time cost and error rate
- **Checksum-based comparison**: Rejected as too strict (floating-point differences)
- **Perceptual hash comparison**: Rejected as too lenient for technical requirements

## 5. Build System Best Practices

### Decision: Incremental Compilation with Dependency Tracking

**Current Makefile Status**: Review required to ensure compliance with FR-009 through FR-014.

**Required Targets**:
- `all`: Default target, builds miniRT executable
- `clean`: Removes object files only
- `fclean`: Removes object files and executable
- `re`: Equivalent to `fclean` + `all`
- `bonus`: Builds with bonus features (if applicable)

**Compilation Flags** (FR-010):
```makefile
CFLAGS = -Wall -Wextra -Werror
```

**No-Relink Test** (FR-011):
```bash
make
touch miniRT  # Update timestamp
make
# Expected: "make: 'miniRT' is up to date."
```

**Best Practices**:
- Use automatic dependency generation for header changes
- Separate object files in build/ directory
- Pattern rules for consistent compilation
- Color-coded output for better visibility

## 6. Testing Methodology

### Decision: Multi-Layer Validation Approach

**Layer 1: Static Analysis**
- norminette 3.3.50 (zero errors, zero warnings)
- Forbidden function grep scan
- Compilation with strict flags

**Layer 2: Functional Validation**
- All 38 scene files render successfully
- Pixel-diff comparison <0.1% threshold
- Memory leak detection (valgrind/leaks)

**Layer 3: Manual Verification**
- Window management (ESC, close button)
- Error handling (invalid scenes)
- Evaluation checklist (miniRT_eval.pdf)

**Test Scene Organization**:
```
scenes/
├── mandatory/          # Required features
│   ├── sphere.rt
│   ├── plane.rt
│   ├── cylinder.rt
│   └── lighting.rt
├── edge_cases/         # Boundary conditions
│   ├── fov_0.rt
│   ├── fov_180.rt
│   └── large_coords.rt
├── error_cases/        # Invalid inputs
│   ├── missing_camera.rt
│   ├── invalid_rgb.rt
│   └── malformed.rt
└── bonus/              # Additional features
    ├── specular.rt
    ├── shadows.rt
    └── textures.rt
```

**Regression Prevention**:
- Baseline renders stored in version control
- Automated CI checks on every commit
- Pre-commit hooks for norminette

## 7. Risk Analysis & Mitigation

### Risk 1: Breaking Existing Functionality (HIGH)

**Likelihood**: High  
**Impact**: Critical (project fails evaluation)

**Mitigation**:
- Incremental changes with validation after each step
- Automated pixel-diff <0.1% validation
- Full test suite run after every module
- Git branching allows instant rollback
- Never modify multiple modules simultaneously

### Risk 2: Introducing Memory Leaks (MEDIUM)

**Likelihood**: Medium  
**Impact**: High (evaluation penalty)

**Mitigation**:
- Run valgrind after each refactoring phase
- Focus on custom memory utilities correctness
- Automated memory checks in CI
- Manual review of malloc/free pairs

### Risk 3: Missing Subtle Norminette Violations (MEDIUM)

**Likelihood**: Medium  
**Impact**: Critical (automatic evaluation failure)

**Mitigation**:
- Run norminette frequently during development
- Automated pre-commit norminette hooks
- Peer review checklist includes norminette verification
- CI pipeline blocks merge on norminette failure

### Risk 4: Performance Degradation (LOW)

**Likelihood**: Low  
**Impact**: Medium (user experience)

**Mitigation**:
- Custom ft_memcpy/ft_memset are simple loops (minimal overhead)
- Function splitting doesn't impact inlining in optimized builds
- Benchmark key rendering operations before/after
- Profile with gprof if performance issues detected

### Risk 5: Incomplete Duplicate Detection (LOW)

**Likelihood**: Low  
**Impact**: Low (maintenance burden)

**Mitigation**:
- Hash-based detection is deterministic
- Manual code review supplements automated detection
- Focus on exact duplicates only (100% match)
- Document rationale for keeping similar files

## 8. Rollback Procedures

### Scenario 1: Functional Regression Detected

**Detection**: Pixel-diff >0.1% or visual artifacts
**Procedure**:
1. Stop refactoring immediately
2. `git diff` to identify changed files since last validation
3. `git checkout <file>` to restore specific files
4. Re-run validation to confirm fix
5. Investigate root cause before proceeding

### Scenario 2: Memory Leak Introduced

**Detection**: Valgrind reports leaks
**Procedure**:
1. Identify module causing leak (binary search approach)
2. Revert specific module with `git checkout`
3. Review custom memory utilities (ft_memcpy/ft_memset)
4. Fix leak in isolated environment
5. Re-integrate with full validation

### Scenario 3: Build Failure

**Detection**: Compilation errors
**Procedure**:
1. `git log --oneline` to identify recent changes
2. `git revert <commit>` for problematic commit
3. Fix compilation errors in clean environment
4. Re-apply changes incrementally

### Scenario 4: Norminette Regression

**Detection**: Previously passing file now fails norminette
**Procedure**:
1. `norminette <file>` to see specific violation
2. Compare with last known good version
3. Fix violation following patterns in Section 1
4. Re-validate entire module

## 9. Implementation Phase Dependencies

### Dependency Graph
```
Phase 0: Baseline Capture (PREREQUISITE for all)
    ↓
Phase 1: Duplicate Detection (Independent)
Phase 3: Forbidden Functions (Independent)
Phase 5: Norminette Audit (Independent)
    ↓
Phase 2: Duplicate Consolidation (Depends on Phase 1)
Phase 4: Function Replacement (Depends on Phase 3)
    ↓
Phase 6: Function Splitting (Depends on Phase 5)
Phase 7: Parameter Reduction (Depends on Phase 5)
Phase 8: Style Fixes (Depends on Phase 5)
    ↓
Phase 9: Full Validation
    ↓
Phase 10: Documentation Update
```

**Critical Path**: Phase 0 → Phase 5 → Phase 6/7/8 → Phase 9

**Parallelization Opportunities**:
- Phase 1 (Duplicates) can run parallel to Phase 3 (Forbidden Functions)
- Phase 6, 7, 8 can be partially parallelized (different modules)

## 10. Technology Decisions

### Version Control Strategy

**Decision**: Feature branch with frequent commits

**Branch Name**: `016-compliance-refactoring`

**Commit Strategy**:
- Small, atomic commits per module
- Descriptive messages: `fix(parser): split parse_scene to meet 25-line limit`
- Tag validation points: `v0.1-duplicates-removed`, `v0.2-norminette-pass`

### CI/CD Integration

**Decision**: GitHub Actions with quality gates

**Pipeline Stages**:
1. Checkout code
2. Install dependencies (norminette, valgrind)
3. Build with strict flags
4. Run norminette checks
5. Execute test suite
6. Memory validation
7. Pixel-diff validation (if baselines exist)

**Blocking Conditions**:
- Norminette errors → FAIL
- Build errors → FAIL
- Memory leaks → FAIL
- Pixel-diff >0.1% → FAIL

### Development Environment

**Required Tools**:
- gcc/clang with C99 support
- norminette 3.3.50+
- valgrind (Linux) or leaks (macOS)
- ImageMagick (for pixel-diff) or custom comparison tool
- make (GNU Make 3.81+)

**Optional Tools**:
- gdb/lldb for debugging
- clang-format for consistent formatting (if compatible with norminette)
- git hooks for pre-commit norminette checks

## 11. Success Metrics

### Quantitative Metrics

| Metric | Target | Measurement |
|--------|--------|-------------|
| Norminette Pass Rate | 100% | `norminette src/ includes/ \| grep Error \| wc -l` = 0 |
| Forbidden Functions | 0 | `grep -r "memset\|memcpy\|memmove" src/ \| wc -l` = 0 |
| Memory Leaks | 0 bytes | `valgrind --leak-check=full` → "0 bytes lost" |
| Test Pass Rate | 100% | All 38 scenes render successfully |
| Pixel Difference | <0.1% | Automated pixel-diff tool |
| Build Warnings | 0 | `-Wall -Wextra -Werror` produces executable |
| Compilation Time | <120% baseline | Time `make clean && make` |

### Qualitative Metrics

- Code readability improved through better function organization
- Maintainability enhanced by removing duplicates
- Evaluation readiness verified through checklist
- Peer review approval from 42 School standards

## 12. Documentation Requirements

### Code Documentation

**Function Comments** (Doxygen-style):
```c
/**
 * @brief Renders a single pixel in the scene
 * @param ctx Render context containing scene and camera
 * @param x X coordinate of pixel
 * @param y Y coordinate of pixel
 * @param output Pointer to store resulting color
 * @return 0 on success, -1 on error
 */
int render_pixel(t_render_context *ctx, int x, int y, t_color *output);
```

**Module Documentation**:
- Each module should have a comment block explaining purpose
- Complex algorithms should reference external documentation
- Helper functions should document their role in parent function

### Project Documentation

**Files to Update**:
- `README.md`: Reflect any new build requirements
- `docs/`: Korean documentation for implementation decisions
- `specs/016-compliance-refactoring/`: All design documents
- Commit messages: Clear history of refactoring steps

## Conclusion

This research establishes a comprehensive, risk-mitigated approach to achieving miniRT compliance. All technical unknowns have been resolved with concrete implementation strategies:

1. **Norminette**: Incremental refactoring with automated validation
2. **Forbidden Functions**: Custom utilities module (ft_memcpy, ft_memset)
3. **Duplicates**: Hash-based exact match detection
4. **Validation**: Pixel-diff <0.1% + memory checks
5. **Risks**: Identified with specific mitigation strategies
6. **Rollback**: Clear procedures for each failure scenario

**Next Steps**: Proceed to Phase 1 (Design & Contracts) to generate:
- data-model.md (entity definitions)
- contracts/ (API contracts if applicable)
- quickstart.md (developer onboarding)

**Estimated Timeline**: 
- Phase 0-2 (Preparation): 2 days
- Phase 3-8 (Refactoring): 5-7 days
- Phase 9-10 (Validation): 1-2 days
- **Total**: 8-11 days with buffer for unexpected issues
