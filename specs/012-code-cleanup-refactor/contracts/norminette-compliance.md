# Norminette Compliance Contract

**Feature**: 012-code-cleanup-refactor  
**Date**: 2026-01-03  
**Version**: 1.0

## Purpose

This contract defines the validation requirements and acceptance criteria for 42 norminette compliance during and after the code cleanup and refactoring process.

## Scope

All C source files (`.c`) and header files (`.h`) in the miniRT project must comply with 42 School norminette standards without exceptions.

## Compliance Requirements

### 1. Header File Requirements

#### Include Guards
**Requirement**: Every header file MUST have include guards to prevent multiple inclusion.

**Format**:
```c
#ifndef FILENAME_H
# define FILENAME_H

/* Header content */

#endif
```

**Validation**: 
```bash
# Check all headers have guards
for file in includes/*.h; do
    if ! grep -q "^#ifndef.*_H$" "$file"; then
        echo "Missing include guard: $file"
    fi
done
```

**Acceptance**: Zero files reported missing include guards.

---

#### No Implementations
**Requirement**: Header files MUST NOT contain function implementations (except inline functions).

**Validation**:
```bash
# Check for function bodies in headers (simplified check)
grep -E "^\{" includes/*.h
```

**Acceptance**: No matches (or only inline functions explicitly marked).

---

#### Define Placement
**Requirement**: All `#define` macros MUST be in header files, not source files (FR-004, FR-005).

**Validation**:
```bash
# Check source files for #define
grep -r "^[[:space:]]*#define" src/ --include="*.c"
```

**Acceptance**: Zero matches (0 defines in source files).

**Current Status**: ✅ Already compliant (verified 2026-01-03).

---

#### Line Length
**Requirement**: No line may exceed 80 characters.

**Validation**: Included in norminette automated check.

**Acceptance**: Norminette reports zero line length violations.

---

### 2. Source File Requirements

#### No Defines
**Requirement**: Source files MUST NOT contain any `#define` statements (FR-004).

**Validation**:
```bash
grep -r "^[[:space:]]*#define" src/ --include="*.c" | wc -l
```

**Acceptance**: Output must be `0`.

**Current Status**: ✅ Already compliant (0 defines found).

---

#### Function Count per File
**Requirement**: Maximum 5 functions per file (excluding static/helper functions in some interpretations).

**Note**: This rule has varying interpretations. Conservative approach: limit public (non-static) functions to 5 per file.

**Validation**:
```bash
# Count non-static functions per file
for file in src/**/*.c; do
    count=$(grep -E "^[a-z_].*\(" "$file" | grep -v "^static" | wc -l)
    if [ "$count" -gt 5 ]; then
        echo "$file: $count functions"
    fi
done
```

**Acceptance**: All files report ≤5 functions.

---

#### Function Length
**Requirement**: No function may exceed 25 lines.

**Validation**: Included in norminette automated check.

**Acceptance**: Norminette reports zero function length violations.

---

#### Line Length
**Requirement**: No line may exceed 80 characters.

**Validation**: Included in norminette automated check.

**Acceptance**: Norminette reports zero line length violations.

---

#### 42 Header Comment
**Requirement**: All files MUST include the standard 42 header comment block.

**Format**:
```c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filename.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: author <email>                             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: YYYY/MM/DD HH:MM:SS by author           #+#    #+#             */
/*   Updated: YYYY/MM/DD HH:MM:SS by author          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
```

**Validation**: Included in norminette automated check.

**Acceptance**: Norminette reports zero header violations.

---

### 3. Naming Conventions

#### Function Names
**Requirement**: All functions MUST use snake_case naming (lowercase with underscores).

**Example**:
```c
✅ Good: calculate_shadow_ray()
❌ Bad: CalculateShadowRay(), calculateShadowRay()
```

**Validation**: Included in norminette automated check.

---

#### Variable Names
**Requirement**: All variables MUST use snake_case naming.

**Validation**: Included in norminette automated check.

---

### 4. Formatting

#### Indentation
**Requirement**: Use tabs (not spaces) for indentation, with tab width = 4.

**Validation**: Included in norminette automated check.

---

#### Braces
**Requirement**: Opening brace on same line for functions, proper alignment for control structures.

**Validation**: Included in norminette automated check.

---

## Automated Validation

### Norminette Tool

**Command**:
```bash
norminette src/ includes/
```

**Expected Output**:
```
src/main.c: OK!
includes/minirt.h: OK!
... (all files)
```

**Acceptance Criteria**: 
- Exit code: 0
- All files report "OK!"
- Zero errors
- Zero warnings

---

### Continuous Validation Workflow

```
File Modified
     ↓
Run norminette on file
     ↓
   Pass? ──No──→ Fix violations
     ↓              ↓
    Yes         Re-run norminette
     ↓              ↓
  Commit  ←───────Pass
```

**Integration Points**:
1. **Pre-commit hook**: Run norminette on staged files
2. **CI pipeline**: Run norminette on all files in PR
3. **Manual check**: Developer runs before commit

---

## Violation Categories

### Critical Violations (Must Fix Immediately)
- Function exceeds 25 lines
- Line exceeds 80 characters
- `#define` in source file
- Missing include guards
- >5 functions per file

### Standard Violations (Must Fix Before Merge)
- Incorrect indentation
- Wrong brace placement
- Naming convention violations
- Missing 42 header

### Style Violations (Should Fix)
- Inconsistent spacing
- Suboptimal code organization within norminette rules

---

## Acceptance Testing

### Test Suite

**Test 1: Full Norminette Scan**
```bash
norminette src/ includes/ > norminette_report.txt
VIOLATIONS=$(grep -c "Error" norminette_report.txt)
if [ "$VIOLATIONS" -eq 0 ]; then
    echo "✅ PASS: Zero norminette violations"
    exit 0
else
    echo "❌ FAIL: $VIOLATIONS violations found"
    cat norminette_report.txt
    exit 1
fi
```

**Test 2: Define Statement Check**
```bash
DEFINES=$(grep -r "^[[:space:]]*#define" src/ --include="*.c" | wc -l)
if [ "$DEFINES" -eq 0 ]; then
    echo "✅ PASS: No defines in source files"
    exit 0
else
    echo "❌ FAIL: $DEFINES defines found in source files"
    grep -r "^[[:space:]]*#define" src/ --include="*.c"
    exit 1
fi
```

**Test 3: Include Guard Check**
```bash
MISSING=0
for file in includes/*.h; do
    basename=$(basename "$file" .h)
    guard=$(echo "$basename" | tr '[:lower:]' '[:upper:]')_H
    if ! grep -q "#ifndef $guard" "$file"; then
        echo "Missing guard in $file"
        MISSING=$((MISSING + 1))
    fi
done
if [ "$MISSING" -eq 0 ]; then
    echo "✅ PASS: All headers have include guards"
    exit 0
else
    echo "❌ FAIL: $MISSING headers missing guards"
    exit 1
fi
```

---

## Success Criteria (from Spec)

| Criterion | Requirement | Validation Method |
|-----------|-------------|-------------------|
| **SC-001** | All source files pass norminette with zero violations | Automated norminette scan |
| **SC-002** | Project compiles with zero errors/warnings | `make` exit code 0 |
| **SC-007** | All `#define` in headers only (zero in source) | grep script (Test 2) |
| **SC-011** | Zero prohibited functions | See function-usage.md |

---

## Contract Guarantees

Upon successful completion of this refactoring:

1. ✅ **All files will pass norminette** with zero errors and zero warnings
2. ✅ **No `#define` statements will exist in source files** (FR-004, FR-005)
3. ✅ **All headers will have proper include guards** (FR-005)
4. ✅ **All functions will be ≤25 lines** (norminette rule)
5. ✅ **All files will be ≤5 functions** (norminette rule, non-static)
6. ✅ **Codebase will be ready for 42 School evaluation** with zero compliance issues

---

## Maintenance Guidelines

### For Future Changes

When adding or modifying code:

1. **Before making changes**: Run norminette on file to establish baseline
2. **While coding**: Keep function length ≤25 lines, line width ≤80 chars
3. **After changes**: Run norminette immediately
4. **Before committing**: Verify norminette passes on all modified files
5. **If violations occur**: Fix immediately before moving to next task

### Common Pitfalls to Avoid

❌ Adding `#define` to source files  
❌ Forgetting include guards in new headers  
❌ Exceeding 25-line function limit  
❌ Using spaces instead of tabs  
❌ Functions exceeding 5 per file  

✅ Keep functions small and focused  
✅ Use helpers to break down complex logic  
✅ Run norminette frequently during development  
✅ Check CI pipeline before merging  

---

## References

- 42 School norminette documentation
- Feature spec: `specs/012-code-cleanup-refactor/spec.md` (FR-004, FR-005, FR-011)
- Success criteria: SC-001, SC-002, SC-007, SC-011
- Constitution Principle II: Code Quality Automation
- Constitution Principle VII: 42 School Function Constraints

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-03 | Initial contract based on spec requirements |
