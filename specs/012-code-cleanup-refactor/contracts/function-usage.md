# Function Usage Contract

**Feature**: 012-code-cleanup-refactor  
**Date**: 2026-01-03  
**Version**: 1.0

## Purpose

This contract defines the validation requirements for function usage, call graphs, and detection of unused code during the cleanup and refactoring process. It ensures that only necessary functions are retained and all removed code is truly unused.

## Scope

All function declarations in header files and implementations in source files across the miniRT project.

## Function States

### State Model

Every function exists in one of the following states:

```
┌────────────────┐
│   Declared     │  In header file, signature visible
│  (in header)   │
└────────┬───────┘
         │
         ↓
┌────────────────┐
│  Implemented   │  Has function body in source file
│  (in source)   │
└────────┬───────┘
         │
         ├─→ Used (called somewhere) → KEEP
         ├─→ Entry point (main) → KEEP
         ├─→ Callback (MLX hooks) → KEEP
         ├─→ Function pointer → VERIFY
         └─→ Unused (no calls) → REMOVE
```

### State Definitions

1. **Declared Only**: Function has header declaration but no implementation
   - **Action**: Remove declaration (dead code)
   - **Risk**: Low (linker catches if actually used)

2. **Implemented Only**: Function has implementation but no header declaration
   - **Action**: Make static (file-local) or add declaration
   - **Risk**: Low (currently not reachable from other files)

3. **Declared + Implemented + Used**: Normal active function
   - **Action**: Keep
   - **Risk**: None

4. **Declared + Implemented + Unused**: Function exists but never called
   - **Action**: Investigate, likely remove
   - **Risk**: Medium (may be callback or reserved for future)

---

## Function Categories

### Category 1: Entry Points (ALWAYS KEEP)

**Definition**: Functions that serve as program entry points.

**Examples**:
- `main()` - Program entry point

**Validation**: Not subject to usage analysis (required by system).

---

### Category 2: MLX Callbacks (ALWAYS KEEP)

**Definition**: Functions registered as callbacks with MiniLibX event system.

**Examples**:
- `int handle_keypress(int keycode, void *param)`
- `int handle_close(void *param)`
- `int handle_loop(void *param)`

**MLX Registration Functions**:
- `mlx_hook()`
- `mlx_loop_hook()`
- `mlx_key_hook()`
- `mlx_mouse_hook()`

**Validation**: Search for function name in MLX registration calls.

```bash
# Find MLX callbacks
grep -r "mlx_.*hook" src/ | grep -oE "[a-z_]+\(" | sort -u
```

**Acceptance**: All functions registered with MLX are marked as "callback" and excluded from removal.

---

### Category 3: Library API Functions (VERIFY BEFORE REMOVAL)

**Definition**: Functions that form the public API of a subsystem, even if not currently used.

**Examples**:
- Vector math operations (may be library-like)
- Parser utilities
- Spatial structure accessors

**Validation**: Manual review to determine if function is part of intentional API vs truly unused.

**Acceptance**: Document decision to keep/remove in commit message.

---

### Category 4: Function Pointer Targets (VERIFY BEFORE REMOVAL)

**Definition**: Functions called indirectly through function pointers.

**Examples**:
- Intersection functions stored in object vtables
- Generic operation handlers

**Validation**: Search for function name in struct initializations and assignments.

```bash
# Find function pointer assignments
grep -r "= &function_name" src/
grep -r "= function_name[,;]" src/
```

**Acceptance**: All function pointer targets verified and documented.

---

### Category 5: Direct Call Functions (USAGE-BASED)

**Definition**: Regular functions called directly by other code.

**Validation**: Search for function calls in entire codebase.

```bash
# Find calls to function_name
grep -r "function_name(" src/ includes/
```

**Acceptance**: If zero calls found and not in categories 1-4, function is unused and can be removed.

---

## Detection Methodology

### Step 1: Extract All Functions

**Header Functions** (declarations):
```bash
# Extract function declarations from headers
grep -h "^[a-z_].*(" includes/*.h | grep -v "^static" | sed 's/(.*//' > declared_functions.txt
```

**Source Functions** (implementations):
```bash
# Extract function implementations from sources
grep -h "^[a-z_].*(" src/**/*.c | grep -v "^static" | sed 's/(.*//' > implemented_functions.txt
```

**Static Functions** (file-local):
```bash
# Extract static functions (not in contract scope, but useful)
grep -h "^static.*(" src/**/*.c | sed 's/(.*//' > static_functions.txt
```

---

### Step 2: Build Call Graph

**For each function**, find all call sites:

```bash
# Example: Find calls to "calculate_shadow"
grep -rn "calculate_shadow(" src/ includes/
```

**Output format**:
```
src/lighting/shadow_calc.c:45:    result = calculate_shadow(ray, obj);
src/lighting/lighting.c:78:    if (calculate_shadow(shadow_ray, scene)) {
```

**Call count**: Number of lines in output = number of call sites.

---

### Step 3: Categorize Functions

For each function:

1. Check if entry point (e.g., `main`) → Category 1 (KEEP)
2. Check if MLX callback → Category 2 (KEEP)
3. Check if function pointer target → Category 4 (VERIFY)
4. Check call count:
   - If > 0 → Category 5 (KEEP)
   - If = 0 → Check if intentional API (Category 3) or REMOVE

---

### Step 4: Generate Removal Candidates

**Output**: List of functions with zero calls and not in categories 1-4.

```bash
# Pseudo-algorithm
for func in $(cat declared_functions.txt); do
    # Skip known categories
    if is_entry_point "$func" || is_mlx_callback "$func"; then
        continue
    fi
    
    # Count calls
    calls=$(grep -r "${func}(" src/ | grep -v "^.*:.*${func}(" | wc -l)
    
    if [ "$calls" -eq 0 ]; then
        echo "CANDIDATE: $func (0 calls)"
    fi
done
```

---

## 42 School Function Constraints

### Allowed Functions (from Constitution Principle VII)

**Standard C Library**:
- `open`, `close`, `read`, `write`
- `printf`, `malloc`, `free`
- `perror`, `strerror`, `exit`

**Math Library** (`-lm`):
- All functions from `math.h`: `sqrt`, `pow`, `sin`, `cos`, `tan`, `floor`, `ceil`, etc.

**MiniLibX Library**:
- All `mlx_*` functions

**Custom Libraries**:
- All functions from `libft` (student-written)
- `get_next_line` (if implemented)

---

### Prohibited Functions (from Constitution Principle VII)

**Threading** (NEVER ALLOWED):
- `pthread_create`, `pthread_join`
- `pthread_mutex_*`, `pthread_cond_*`
- Any pthread library function

**Multiprocessing** (NEVER ALLOWED):
- `fork`, `pipe`, `waitpid`
- Any process management function

**External Libraries** (NEVER ALLOWED):
- OpenMP, TBB, or any parallelization library
- SIMD intrinsics (SSE, AVX, NEON)

---

### Validation for Prohibited Functions

**Command**:
```bash
# Check for pthread usage
grep -r "pthread_" src/ includes/

# Check for fork/pipe
grep -r "\(fork\|pipe\|waitpid\)(" src/ includes/

# Check for OpenMP pragmas
grep -r "#pragma omp" src/ includes/
```

**Acceptance**: All commands return zero results (exit code 1 from grep = no matches = success).

**Current Status**: ✅ Zero prohibited functions found (verified 2026-01-03).

---

## Removal Workflow

### Process for Each Removal Candidate

```
1. Function identified as unused
   ↓
2. Manual verification:
   - Not an entry point? ✓
   - Not a callback? ✓
   - Not function pointer target? ✓
   - Zero direct calls? ✓
   ↓
3. Remove implementation from source file
   ↓
4. Remove declaration from header file
   ↓
5. Compile project
   ↓
6. Compilation success?
   ├─ Yes → Test with scene files
   └─ No → Investigate error, may be callback
   ↓
7. All tests pass?
   ├─ Yes → Commit removal
   └─ No → Revert, mark as false positive
   ↓
8. Document removal in commit message
```

---

### Batch Removal Strategy

**Phase 1**: Remove obvious unused functions (0 calls, not callbacks)
- Compile after each removal
- Test immediately
- Commit individually

**Phase 2**: Remove borderline cases after manual review
- More careful testing
- Document reasoning

**Phase 3**: Final validation
- Full norminette check
- All scene files render correctly
- No compilation warnings

---

## Call Graph Documentation

### Format

For each module, document function relationships:

**Example: Vector Math Module**

```
vec3.h:
├─ vec3_add(a, b)           [USED: 23 call sites]
├─ vec3_subtract(a, b)      [USED: 18 call sites]
├─ vec3_multiply(a, scalar) [USED: 15 call sites]
├─ vec3_dot(a, b)           [USED: 31 call sites]
├─ vec3_cross(a, b)         [USED: 12 call sites]
├─ vec3_length(v)           [USED: 27 call sites]
├─ vec3_normalize(v)        [USED: 45 call sites]
└─ vec3_reflect(v, n)       [USED: 8 call sites]

All functions: ACTIVE (KEEP)
```

**Example: Parser Module**

```
parser.h:
├─ parse_scene(filename)    [USED: 1 call site - main.c]
├─ parse_camera(line)       [USED: 1 call site - parse_scene]
├─ parse_light(line)        [USED: 1 call site - parse_scene]
├─ parse_sphere(line)       [USED: 1 call site - parse_scene]
├─ parse_plane(line)        [USED: 1 call site - parse_scene]
├─ parse_cylinder(line)     [USED: 1 call site - parse_scene]
└─ validate_scene(scene)    [USED: 0 call sites] → CANDIDATE FOR REMOVAL

Removal candidate: validate_scene (may be reserved for future)
Decision: REMOVE (not implemented, not referenced)
```

---

## Testing Validation

### Test 1: Prohibited Function Check

```bash
#!/bin/bash
echo "Checking for prohibited functions..."

PTHREAD=$(grep -r "pthread_" src/ includes/ | wc -l)
FORK=$(grep -r "\(fork\|pipe\|waitpid\)(" src/ includes/ | wc -l)
OPENMP=$(grep -r "#pragma omp" src/ includes/ | wc -l)

TOTAL=$((PTHREAD + FORK + OPENMP))

if [ "$TOTAL" -eq 0 ]; then
    echo "✅ PASS: No prohibited functions found"
    exit 0
else
    echo "❌ FAIL: Found $TOTAL prohibited function usages"
    echo "  pthread: $PTHREAD"
    echo "  fork/pipe: $FORK"
    echo "  OpenMP: $OPENMP"
    exit 1
fi
```

---

### Test 2: Function Usage Validation

```bash
#!/bin/bash
echo "Checking for unused functions..."

# Generate list of all public functions
grep -h "^[a-z_].*(" includes/*.h | grep -v "^static" | sed 's/(.*//' | sort -u > /tmp/declared.txt

UNUSED=0
while read func; do
    # Skip main
    if [ "$func" = "main" ]; then
        continue
    fi
    
    # Count calls
    calls=$(grep -r "${func}(" src/ | wc -l)
    
    if [ "$calls" -eq 0 ]; then
        echo "Unused: $func"
        UNUSED=$((UNUSED + 1))
    fi
done < /tmp/declared.txt

if [ "$UNUSED" -eq 0 ]; then
    echo "✅ PASS: No unused functions"
    exit 0
else
    echo "⚠️  WARNING: $UNUSED potentially unused functions (verify callbacks)"
    exit 0  # Not a failure, requires manual review
fi
```

---

## Success Criteria (from Spec)

| Criterion | Requirement | Validation Method |
|-----------|-------------|-------------------|
| **SC-011** | Zero prohibited functions | Test 1 (prohibited function check) |
| **FR-001** | Remove all unused header declarations | Test 2 + manual verification |
| **FR-002** | Remove all unused implementations | Call graph analysis |
| **FR-016** | Only allowed functions used | Test 1 (positive validation) |
| **FR-017** | No prohibited functions | Test 1 (negative validation) |

---

## Contract Guarantees

Upon successful completion:

1. ✅ **All remaining functions are used** (called, callback, or entry point)
2. ✅ **No unused declarations exist** in headers (FR-001)
3. ✅ **No unused implementations exist** in sources (FR-002)
4. ✅ **Only allowed functions are used** (libc, math, mlx, libft, GNL) (FR-016)
5. ✅ **Zero prohibited functions** (no pthread, fork, external libs) (FR-017)
6. ✅ **Call graph is documented** for each module
7. ✅ **All removals are justified** and tested

---

## Maintenance Guidelines

### For Future Development

When adding new functions:

1. ✅ Declare in appropriate header (domain-based)
2. ✅ Implement in corresponding source file
3. ✅ Verify function is actually used (no dead code)
4. ✅ Check function is in allowed list (Constitution Principle VII)
5. ✅ Document purpose and usage in comments

When removing functions:

1. ✅ Verify zero call sites
2. ✅ Check not a callback or entry point
3. ✅ Test compilation after removal
4. ✅ Test with scene files
5. ✅ Document removal reason in commit

---

## References

- Feature spec: `specs/012-code-cleanup-refactor/spec.md` (FR-001, FR-002, FR-016, FR-017)
- Success criteria: SC-011
- Constitution Principle VII: 42 School Function Constraints
- Data model: `specs/012-code-cleanup-refactor/data-model.md`

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-03 | Initial contract based on spec requirements |
