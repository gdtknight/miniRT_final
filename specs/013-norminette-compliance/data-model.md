# Data Model: Norminette Compliance Refactoring

**Feature**: Complete Norminette Compliance  
**Branch**: `013-norminette-compliance`  
**Date**: 2026-01-03  
**Phase**: 1 (Design & Contracts)

## Overview

This document catalogs the existing data structures used in miniRT that are relevant to the norminette compliance refactoring. Since this is a refactoring task (not new feature development), no new domain entities are being created. Instead, this document serves as a reference for understanding how existing structures are used and will be affected by file splitting and function extraction.

The document also defines new **parameter structures** that will be introduced to reduce function parameter counts and achieve TOO_MANY_ARGS compliance.

---

## Existing Entities

### 1. Source File Module

**Definition**: A .c source file containing related functionality

**Attributes**:
- `file_path` (string): Relative path from repo root (e.g., `src/core/metrics.c`)
- `function_count` (int): Number of functions defined in the file
- `function_list` (array<string>): Names of all functions in the file
- `line_count` (int): Total lines in the file
- `dependencies` (array<string>): List of other source files this file depends on

**Constraints**:
- `function_count` ≤ 5 (norminette TOO_MANY_FUNCS rule)
- Functions in a file MUST be logically related
- File name MUST reflect the feature domain (e.g., metrics_tracking.c, window_events.c)

**Relationships**:
- Contains 1-5 Function Units
- Has exactly one corresponding Header File
- May depend on multiple other Source File Modules
- Compiled into one Object File

**Current Violations**:
| File | Function Count | Violations | Split Strategy |
|------|----------------|------------|----------------|
| metrics.c | 8 | TOO_MANY_FUNCS × 8 | Split into 3 files by concern |
| window.c | 7 | TOO_MANY_FUNCS × 7 | Split into 3 files by lifecycle |
| timer.c | 6 | TOO_MANY_FUNCS × 6 | Split into 2 files by purpose |
| render_state.c | 7 | TOO_MANY_FUNCS × 4 | Split into 2 files + fix ternary |
| aabb.c | 9 | TOO_MANY_FUNCS × 5 | Split into 2 files by complexity |
| bvh_build.c | 7 | TOO_MANY_FUNCS × 3 | Split into 2 files by role |
| render.c | 6 | TOO_MANY_FUNCS × 1 | Extract 1 function |
| hud_render.c | 13 | TOO_MANY_FUNCS × 9 | Split into 4 files by UI layer |

---

### 2. Function Unit

**Definition**: A single C function with defined behavior

**Attributes**:
- `name` (string): Function identifier (e.g., `calculate_fps`)
- `line_count` (int): Number of lines from opening { to closing }
- `parameter_count` (int): Number of formal parameters
- `local_variable_count` (int): Number of local variables declared
- `is_static` (bool): Whether function has static storage class
- `complexity_score` (int): Cyclomatic complexity (not enforced by norminette but useful)

**Constraints**:
- `line_count` ≤ 25 (norminette TOO_MANY_LINES rule)
- `parameter_count` ≤ 4 (norminette TOO_MANY_ARGS rule)
- `local_variable_count` ≤ 5 (norminette TOO_MANY_VARS_FUNC rule)
- Function name MUST follow snake_case convention
- Function MUST have exactly one responsibility (Single Responsibility Principle)

**Relationships**:
- Belongs to exactly one Source File Module
- Declared in exactly one Header File
- May call multiple other Function Units
- May be called by multiple other Function Units

**Current Violations**:
| Function | File | Violation | Line/Param/Var Count | Fix Strategy |
|----------|------|-----------|---------------------|--------------|
| main() | main.c | TOO_MANY_LINES | 79 lines | Extract initialization and cleanup |
| render_loop() | render.c | TOO_MANY_LINES | >25 | Extract rendering stages |
| render_scene() | render.c | TOO_MANY_VARS_FUNC | 6 vars | Group related vars, extract helpers |
| parse_sphere() | parse_elements.c | TOO_MANY_LINES | >25 | Extract validation logic |
| handle_key() | window.c | TOO_MANY_LINES | >25 | Extract per-key handlers |
| render_hud() | hud_render.c | TOO_MANY_LINES | >25 | Extract HUD sections |
| draw_keyguide() | keyguide_render.c | TOO_MANY_ARGS | 5 params | Create render context struct |
| traverse_bvh() | bvh_traverse.c | TOO_MANY_ARGS | 6 params | Create traversal context struct |
| build_bvh_node() | bvh_build.c | TOO_MANY_ARGS | 5 params | Create build context struct |

---

### 3. Header File

**Definition**: A .h header file declaring interfaces

**Attributes**:
- `file_path` (string): Relative path from repo root (e.g., `includes/metrics_tracking.h`)
- `declarations` (array<string>): List of function declarations
- `includes` (array<string>): List of other headers included
- `guards` (string): Include guard macro (e.g., `METRICS_TRACKING_H`)
- `has_42_header` (bool): Whether file contains 42 School header comment

**Constraints**:
- MUST use tabs for indentation (not spaces)
- MUST NOT have trailing whitespace (SPC_BEFORE_NL)
- MUST have include guards to prevent multiple inclusion
- Function declarations MUST be properly aligned
- One header file per source file (1:1 mapping)

**Relationships**:
- Declares functions from exactly one Source File Module
- May be included by multiple Source File Modules
- May include multiple other Header Files

**Current Violations**:
| Header | Violation Type | Count | Fix Strategy |
|--------|----------------|-------|--------------|
| miniRT.h | TOO_FEW_TAB | 2 | Replace spaces with tabs |
| structures.h | TOO_FEW_TAB | 3 | Replace spaces with tabs |
| structures.h | SPC_BEFORE_NL | 5 | Remove trailing whitespace |
| constants.h | PREPROC_CONSTANT | 1 | Fix constant naming |
| (various) | MISALIGNED_FUNC_DECL | 1 | Align function declaration |

**New Headers to Create**:
After file splitting, these headers will be created with proper formatting:
- `metrics_tracking.h`, `metrics_display.h`, `metrics_fps.h`
- `render_init.h`, `render_loop.h`, `render_state_update.h`
- `timer_control.h`, `timer_metrics.h`
- `window_init.h`, `window_events.h`, `window_hooks.h`
- `hud/hud_text.h`, `hud/hud_metrics.h`, `hud/hud_controls.h`
- `aabb_basic.h`, `aabb_advanced.h`
- `bvh_build_core.h`, `bvh_build_helpers.h`

---

### 4. Norminette Error

**Definition**: A specific rule violation detected by norminette

**Attributes**:
- `error_type` (enum): Type of violation (TOO_MANY_FUNCS, TOO_MANY_LINES, etc.)
- `file_path` (string): File where error occurs
- `line_number` (int): Line number of violation
- `column_number` (int): Column number of violation
- `severity` (enum): Always ERROR (norminette doesn't have warnings)

**Constraints**:
- Each error MUST be resolved (no acceptable violations)
- Errors MUST be resolved in priority order (high-impact first)

**Current Error Distribution**:
| Error Type | Count | Percentage | Priority |
|------------|-------|------------|----------|
| TOO_MANY_FUNCS | 53 | 69% | P1 (High) |
| TOO_FEW_TAB | 5 | 6% | P2 (Medium) |
| SPC_BEFORE_NL | 5 | 6% | P2 (Medium) |
| TOO_MANY_LINES | 5 | 6% | P2 (Medium) |
| TOO_MANY_ARGS | 6 | 8% | P3 (Low) |
| TOO_MANY_VARS_FUNC | 1 | 1% | P3 (Low) |
| TERNARY_FBIDDEN | 1 | 1% | P3 (Low) |
| PREPROC_CONSTANT | 1 | 1% | P3 (Low) |
| MISALIGNED_FUNC_DECL | 1 | 1% | P3 (Low) |
| **TOTAL** | **77** | **100%** | - |

**Target State**: 0 errors across all categories

---

## New Parameter Structures

These structures will be introduced to reduce function parameter counts and achieve norminette compliance.

### 5. t_render_config (NEW)

**Purpose**: Group rendering configuration parameters

**Definition**:
```c
typedef struct s_render_config
{
    int     width;              // Window width in pixels
    int     height;             // Window height in pixels
    int     samples_per_pixel;  // Anti-aliasing samples
    int     max_ray_depth;      // Maximum ray bounce depth
    double  gamma_correction;   // Gamma correction factor
}   t_render_config;
```

**Usage**: Passed to rendering functions that currently take multiple config parameters

**Replaces**: Multiple individual parameters in functions like `render_scene()`, `init_rendering()`

**Benefits**:
- Reduces parameter count from 5-6 to 1
- Groups logically related configuration
- Easier to extend with new config options

---

### 6. t_state_update (NEW)

**Purpose**: Group state update parameters for event handling

**Definition**:
```c
typedef struct s_state_update
{
    double  delta_time;     // Time since last update (seconds)
    int     key_code;       // Key pressed/released code
    int     mouse_x;        // Mouse X coordinate
    int     mouse_y;        // Mouse Y coordinate
    int     mouse_button;   // Mouse button state
}   t_state_update;
```

**Usage**: Passed to state update functions that handle user input

**Replaces**: Multiple parameters in `update_state()`, `handle_input()`, etc.

**Benefits**:
- Reduces parameter count from 5 to 2 (state + update)
- Encapsulates all input information
- Supports adding new input types without signature changes

---

### 7. t_bvh_traversal (NEW)

**Purpose**: Group BVH traversal context parameters

**Definition**:
```c
typedef struct s_bvh_traversal
{
    t_ray           *ray;       // Ray being traced
    double          t_min;      // Minimum ray parameter
    double          t_max;      // Maximum ray parameter
    t_hit_record    *hit;       // Output: closest hit found
    int             depth;      // Current traversal depth
}   t_bvh_traversal;
```

**Usage**: Passed to `traverse_bvh()` and related functions

**Replaces**: 6 parameters in `traverse_bvh()` (bvh_traverse.c line 58)

**Benefits**:
- Reduces parameter count from 6 to 2 (node + context)
- Groups all traversal state
- Easier to add traversal statistics or debugging info

---

### 8. t_bvh_build_ctx (NEW)

**Purpose**: Group BVH construction parameters

**Definition**:
```c
typedef struct s_bvh_build_ctx
{
    t_object    *objects;       // Array of objects to build from
    int         obj_count;      // Number of objects
    int         max_depth;      // Maximum tree depth
    int         min_objects;    // Minimum objects per leaf
}   t_bvh_build_ctx;
```

**Usage**: Passed to `build_bvh()` and helper functions

**Replaces**: 5 parameters in `build_bvh_node()` (bvh_build.c line 74)

**Benefits**:
- Reduces parameter count from 5 to 2 (indices + context)
- Encapsulates build configuration
- Supports advanced build strategies

---

### 9. t_hud_render_ctx (NEW)

**Purpose**: Group HUD rendering parameters

**Definition**:
```c
typedef struct s_hud_render_ctx
{
    void        *mlx;           // MLX connection
    void        *window;        // MLX window
    int         width;          // Window width
    int         height;         // Window height
    t_metrics   *metrics;       // Performance metrics to display
}   t_hud_render_ctx;
```

**Usage**: Passed to HUD rendering functions

**Replaces**: 5+ parameters in `draw_keyguide()` (keyguide_render.c line 18) and related functions

**Benefits**:
- Reduces parameter count from 5+ to 2 (hud_state + context)
- Groups all rendering resources
- Easier to add new HUD elements

---

## Entity Relationships

```
Source File Module (1) ─────┬───→ (1) Header File
                            │
                            └───→ (1-5) Function Units
                            
Function Unit (1) ──────────→ (0-4) Parameters
                            │
                            └───→ (0-5) Local Variables
                            
Header File (1) ────────────→ (1-N) Function Declarations
                            │
                            └───→ (0-N) Includes
                            
Norminette Error (*) ───────→ (1) Source File Module or Header File
                            │
                            └───→ (0-1) Function Unit (if function-specific)
                            
Parameter Structure (1) ────→ (2-6) Parameters (groups them)
                            │
                            └───→ (1-N) Function Units (used by)
```

---

## Data Model Validation

### Completeness Check
- ✅ All existing entity types documented
- ✅ New parameter structures defined
- ✅ Current violations cataloged
- ✅ Relationships mapped
- ✅ Constraints documented

### Consistency Check
- ✅ All structures use miniRT naming conventions (t_ prefix)
- ✅ All constraints align with norminette rules
- ✅ All relationships are bidirectional where appropriate
- ✅ All new structures serve clear compliance purpose

### Traceability
- ✅ Each existing violation mapped to fix strategy
- ✅ Each new structure mapped to specific usage
- ✅ Each entity mapped to affected files
- ✅ Each constraint mapped to norminette rule

---

## Data Model Impact Analysis

### Files Affected by Refactoring

**Core Module**: 6 new files, 12 new headers
- Splitting: metrics.c (→3), timer.c (→2), render.c (+2), render_state.c (+1)

**UI Module**: 6 new files, 7 new headers  
- Splitting: window.c (→3), hud_render.c (→4 in hud/ subdirectory)

**BVH Module**: 4 new files, 4 new headers
- Splitting: aabb.c (→2), bvh_build.c (→2)

**Total Impact**:
- **Before**: ~20 source files, ~15 header files
- **After**: ~36 source files, ~32 header files
- **New Structures**: 5 parameter structures
- **Lines of Code**: No significant change (code moved, not rewritten)

### Build System Impact
- Makefile: Update SRCS variables with ~16 new files
- Header dependencies: Add ~17 new include relationships
- Compilation time: Minimal increase (more smaller translation units)

### Testing Impact
- Functionality: No change expected (behavior-preserving refactoring)
- Performance: ≤5% regression acceptable (function call overhead)
- Validation: Image diff, exit codes, benchmark comparison

---

**Phase 1 Data Model Complete**: All entities documented, ready for contract definition.
