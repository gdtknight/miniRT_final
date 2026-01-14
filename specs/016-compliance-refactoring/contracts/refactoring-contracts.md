# Refactoring Contracts & Interfaces

**Feature**: 016-compliance-refactoring  
**Date**: 2026-01-14

## Overview

This document defines the contracts and interfaces for the compliance refactoring process. These are not runtime APIs but rather the guaranteed behaviors, interfaces, and contracts that govern how refactoring is performed while maintaining backward compatibility.

## 1. Norminette Compliance Contract

### Interface: Source File Compliance

**Contract**: All source files MUST pass norminette 3.3.50 with zero errors and zero warnings.

**Input**:
- Source file path (`.c` or `.h`)

**Output**:
- Compliance status: `PASS` or `FAIL`
- List of violations (if any)

**Validation**:
```bash
norminette <file_path>
# Expected output for PASS: "OK!"
# Expected output for FAIL: List of errors with line numbers
```

**Guarantees**:
- Each function ≤ 25 lines
- Each function ≤ 4 parameters
- Each file ≤ 5 functions
- All variables declared at function start
- Proper formatting (indentation, spacing, line length)

**Breaking Conditions**:
- Any norminette error
- Any norminette warning
- File not found or unreadable

## 2. Forbidden Functions Contract

### Interface: Function Whitelist Compliance

**Contract**: Only functions from the miniRT.pdf allowed list may be used.

**Allowed Functions** (from miniRT.pdf):
```c
// Standard I/O
open, close, read, write, printf, perror, strerror, exit

// Memory management
malloc, free

// Math library (math.h)
All math.h functions (sin, cos, sqrt, pow, etc.)

// MinilibX (platform-specific)
mlx_init, mlx_new_window, mlx_new_image, mlx_get_data_addr
mlx_put_image_to_window, mlx_pixel_put, mlx_loop, mlx_hook
mlx_key_hook, mlx_mouse_hook, mlx_loop_hook, mlx_destroy_image
mlx_destroy_window, mlx_destroy_display

// Custom implementations
get_next_line (if implemented)
All libft functions (if implemented)
```

**Forbidden Functions** (partial list):
```c
// Memory manipulation
memset, memcpy, memmove, bzero, bcopy

// String manipulation  
strcpy, strncpy, strcat, strncat, strdup

// Concurrency
pthread_*, fork, pipe
```

**Replacement Contract**:
- Forbidden function usage MUST be replaced with:
  - Custom implementation (e.g., `ft_memcpy`)
  - Alternative allowed function
  - Refactored logic avoiding the function

**Validation**:
```bash
# Check for forbidden functions
grep -rn "memset\|memcpy\|memmove\|bzero" src/ includes/
# Expected: No matches (exit code 1)
```

**Guarantees**:
- Zero forbidden function calls in final codebase
- Custom replacements provide identical behavior
- No functional regressions from replacement

## 3. Custom Memory Utilities Contract

### Interface: ft_memcpy

**Signature**:
```c
void *ft_memcpy(void *dst, const void *src, size_t n);
```

**Contract**: Copy `n` bytes from `src` to `dst`, return `dst`.

**Behavior Specification**:
- Copies bytes in forward direction (src[0] → dst[0], then src[1] → dst[1], ...)
- Returns `dst` pointer
- Returns `NULL` if both `dst` and `src` are `NULL`
- Undefined behavior if memory regions overlap (same as standard memcpy)

**Preconditions**:
- `src` must point to valid readable memory of at least `n` bytes
- `dst` must point to valid writable memory of at least `n` bytes
- OR both `src` and `dst` are `NULL` and `n` is 0

**Postconditions**:
- For all `i` in [0, n): `dst[i] == src[i]`
- `dst` pointer is returned
- No memory beyond `n` bytes is modified

**Test Cases**:
```c
// Test 1: Normal copy
char src[10] = "hello";
char dst[10];
ft_memcpy(dst, src, 6);
assert(strcmp(dst, "hello") == 0);

// Test 2: Zero bytes
char buf[10];
void *result = ft_memcpy(buf, "test", 0);
assert(result == buf);

// Test 3: NULL handling
void *result = ft_memcpy(NULL, NULL, 0);
assert(result == NULL);

// Test 4: Binary data
unsigned char src[4] = {0xFF, 0x00, 0xAB, 0xCD};
unsigned char dst[4];
ft_memcpy(dst, src, 4);
assert(memcmp(dst, src, 4) == 0);
```

### Interface: ft_memset

**Signature**:
```c
void *ft_memset(void *b, int c, size_t len);
```

**Contract**: Fill `len` bytes at `b` with value `c`, return `b`.

**Behavior Specification**:
- Sets each byte to `(unsigned char)c`
- Returns `b` pointer
- Processes bytes in forward direction

**Preconditions**:
- `b` must point to valid writable memory of at least `len` bytes
- `c` is converted to `unsigned char` (uses only low 8 bits)

**Postconditions**:
- For all `i` in [0, len): `b[i] == (unsigned char)c`
- `b` pointer is returned
- No memory beyond `len` bytes is modified

**Test Cases**:
```c
// Test 1: Zero initialization
char buf[10];
ft_memset(buf, 0, 10);
for (int i = 0; i < 10; i++)
    assert(buf[i] == 0);

// Test 2: Non-zero value
unsigned char buf[5];
ft_memset(buf, 0xFF, 5);
for (int i = 0; i < 5; i++)
    assert(buf[i] == 0xFF);

// Test 3: Structure initialization
typedef struct { int a; int b; } t_test;
t_test s;
ft_memset(&s, 0, sizeof(t_test));
assert(s.a == 0 && s.b == 0);

// Test 4: Truncation of int to char
char buf[3];
ft_memset(buf, 0x1234, 3);  // Only 0x34 used
for (int i = 0; i < 3; i++)
    assert(buf[i] == 0x34);
```

## 4. Duplicate Consolidation Contract

### Interface: Duplicate File Detection

**Contract**: Identify files with 100% identical content using cryptographic hash.

**Algorithm**:
```bash
# Generate SHA-256 hash for each file
sha256sum <file>

# Files are duplicates if and only if:
hash(file1) == hash(file2) AND file1 != file2
```

**Guarantees**:
- 100% byte-for-byte match required (not similarity)
- Hash collision probability negligible (SHA-256)
- Whitespace, comments, all content must match exactly

**Consolidation Contract**:
- ONE canonical file chosen from duplicate set
- ALL other duplicates removed from filesystem and build system
- ALL references updated to point to canonical file
- Build succeeds after consolidation
- Functionality preserved (validated by test suite)

**Selection Criteria for Canonical File**:
1. More descriptive filename
2. Better module location (logical organization)
3. Most recent relevant commit (if tied)

**Validation**:
```bash
# After consolidation
make clean && make
# Expected: Successful build

./run_tests.sh
# Expected: All tests pass
```

## 5. Validation & Testing Contract

### Interface: Pixel-Diff Validation

**Contract**: Rendered images before and after refactoring MUST be visually identical within 0.1% tolerance.

**Input**:
- Baseline render (PPM format)
- Current render (PPM format)

**Output**:
- Pixel difference percentage (0.0 - 100.0)
- Pass/Fail status

**Algorithm**:
```
For each pixel (x, y):
    baseline_color = baseline_image[x][y]
    current_color = current_image[x][y]
    diff += abs(baseline_color.r - current_color.r)
    diff += abs(baseline_color.g - current_color.g)  
    diff += abs(baseline_color.b - current_color.b)

total_pixels = width * height * 3
diff_percentage = (diff / (total_pixels * 255)) * 100

PASS if diff_percentage < 0.1
FAIL otherwise
```

**Guarantees**:
- Catches visual regressions
- Tolerates minor floating-point rounding differences
- Deterministic (same input → same output)

**Test Scenes Contract**:
- All 38 scene files MUST be tested
- Each scene MUST render successfully (no crashes)
- Each scene MUST pass pixel-diff validation

### Interface: Memory Leak Validation

**Contract**: Program MUST not leak any memory across all test scenarios.

**Tool**: valgrind (Linux) or leaks (macOS)

**Command**:
```bash
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --error-exitcode=1 \
         ./miniRT <scene_file>
```

**Expected Output**:
```
HEAP SUMMARY:
    definitely lost: 0 bytes in 0 blocks
    indirectly lost: 0 bytes in 0 blocks
    possibly lost: 0 bytes in 0 blocks
    still reachable: 0 bytes in 0 blocks
```

**Guarantees**:
- Zero bytes definitely lost
- Zero bytes indirectly lost
- Zero bytes possibly lost
- All allocated memory freed before exit

**Test Coverage**:
- Normal execution (valid scenes)
- Error cases (invalid scenes)
- Early termination (ESC key)
- Window close event

## 6. Build System Contract

### Interface: Makefile Targets

**Contract**: Makefile MUST provide standard targets with specified behavior.

#### Target: `all`
**Behavior**: Compile all source files and link into `miniRT` executable

**Guarantees**:
- Uses flags: `-Wall -Wextra -Werror`
- Links required libraries: `-lmlx -lX11 -lXext -lm` (Linux) or `-lmlx -framework OpenGL -framework AppKit` (macOS)
- Creates executable at repository root
- Incremental compilation (only rebuild changed files)

#### Target: `clean`
**Behavior**: Remove all object files

**Guarantees**:
- Removes all `.o` files from build directory
- Preserves executable
- Idempotent (can run multiple times)

#### Target: `fclean`
**Behavior**: Remove all object files and executable

**Guarantees**:
- Performs `clean`
- Removes `miniRT` executable
- Idempotent

#### Target: `re`
**Behavior**: Full rebuild

**Guarantees**:
- Equivalent to `make fclean && make all`
- Fresh compilation of all files

**No-Relink Contract**:
```bash
make all          # Build project
make all          # Run again
# Expected: "Nothing to be done for 'all'" or "miniRT is up to date"
# Guarantees: No recompilation, no relinking
```

## 7. Function Refactoring Contract

### Interface: Function Splitting

**Contract**: Functions exceeding 25 lines MUST be split into helper functions while preserving behavior.

**Transformation Pattern**:
```c
// Before: 40 lines
int original_function(t_data *data, int param)
{
    // ... 40 lines of code
    return result;
}

// After: Main function + helpers
static int helper_function_1(t_data *data)
{
    // First logical block (< 25 lines)
}

static int helper_function_2(t_data *data, int param)
{
    // Second logical block (< 25 lines)
}

int original_function(t_data *data, int param)
{
    int temp_result;
    
    temp_result = helper_function_1(data);
    return helper_function_2(data, param);
}
```

**Guarantees**:
- Original function behavior preserved (same inputs → same outputs)
- Helper functions default to `static` visibility
- Each function (including helpers) ≤ 25 lines
- Logical coherence (helpers perform complete sub-tasks)

### Interface: Parameter Reduction

**Contract**: Functions with >4 parameters MUST be refactored to use structures or simplified interfaces.

**Transformation Pattern**:
```c
// Before: 6 parameters
void render_pixel(t_scene *scene, t_camera *cam, 
                  int x, int y, t_color *output, t_config *cfg)

// After: Context structure + 4 parameters
typedef struct s_render_context
{
    t_scene     *scene;
    t_camera    *cam;
    t_config    *cfg;
}   t_render_context;

void render_pixel(t_render_context *ctx, int x, int y, t_color *output)
```

**Guarantees**:
- Parameter count ≤ 4
- Logical grouping of related parameters
- No performance degradation (struct passed by pointer)
- Clear ownership semantics

## 8. Rollback Contract

### Interface: Safe Rollback Procedure

**Contract**: At any point, refactoring MUST be revertible to last known good state.

**Rollback Triggers**:
- Pixel-diff validation fails (>0.1% difference)
- Memory leak detected (valgrind reports leaks)
- Build failure
- Test suite failure
- Norminette regression

**Rollback Procedure**:
```bash
# Identify last good commit
git log --oneline --decorate

# Revert to last validation point
git reset --hard <last-good-commit>

# Or revert specific files
git checkout <last-good-commit> -- <file-path>

# Re-run validation
make clean && make
./run_tests.sh
```

**Guarantees**:
- All changes tracked in version control
- Frequent commits at validation points
- Tag validation milestones
- No data loss during rollback

## 9. Documentation Contract

### Interface: Code Documentation

**Contract**: All modified functions MUST have updated documentation.

**Format**: Doxygen-style comments
```c
/**
 * @brief Brief description of function purpose
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter
 * @return Description of return value
 * @note Additional notes (e.g., "Helper function for X")
 */
```

**Guarantees**:
- Every public function has documentation
- Helper functions document their role
- Complex algorithms reference external documentation
- Documentation matches implementation

### Interface: Refactoring Log

**Contract**: All refactoring actions MUST be logged for audit trail.

**Format**: Structured commit messages
```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types**: `fix`, `refactor`, `style`, `docs`, `test`

**Example**:
```
refactor(parser): split parse_scene to meet 25-line limit

Extracted helper functions:
- parse_scene_ambient(): Parse ambient lighting
- parse_scene_camera(): Parse camera configuration  
- parse_scene_lights(): Parse light sources

All helpers are static. Behavior preserved, validated with
test scenes mandatory/*.rt.

Closes #16
```

## 10. Success Criteria Contract

### Interface: Definition of Done

**Contract**: Refactoring is complete when ALL criteria are met.

**Criteria**:
- ✅ 100% files pass norminette (75 source files)
- ✅ 0 forbidden function usages
- ✅ 0 build warnings with strict flags
- ✅ 0 memory leaks (valgrind)
- ✅ 100% test scenes pass (38 scenes)
- ✅ Pixel-diff <0.1% on all scenes
- ✅ All documentation updated
- ✅ Peer review approved
- ✅ CI/CD pipeline passes

**Verification**:
```bash
# Norminette
norminette src/ includes/ | grep -i error
# Expected: No output

# Forbidden functions
grep -rn "memset\|memcpy\|memmove" src/ includes/
# Expected: No matches

# Build
make clean && make
# Expected: Success, 0 warnings

# Tests
./run_tests.sh
# Expected: All pass

# Memory
valgrind --leak-check=full ./miniRT scenes/mandatory/sphere.rt
# Expected: 0 bytes lost

# Pixel-diff
./scripts/pixel_diff.sh
# Expected: All <0.1%
```

## Summary

These contracts define:

1. **Compliance Contracts**: Norminette and function whitelist requirements
2. **Implementation Contracts**: Custom utility function behavior
3. **Quality Contracts**: Validation, testing, and memory safety
4. **Process Contracts**: Refactoring patterns and rollback procedures
5. **Documentation Contracts**: Code comments and commit message format
6. **Success Contracts**: Definition of done with measurable criteria

All contracts are verifiable, testable, and enforceable through automated tooling. Adherence to these contracts ensures the refactoring maintains functionality while achieving full 42 School compliance.
