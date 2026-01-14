# Data Model: Compliance Refactoring

**Feature**: 016-compliance-refactoring  
**Date**: 2026-01-14

## Overview

This document defines the data structures and entities involved in the compliance refactoring process. While this refactoring doesn't introduce new runtime data structures, it defines the organizational model for tracking compliance state, validation results, and refactoring metadata.

## Core Entities

### 1. Source File Metadata

**Purpose**: Track compliance state for each source file in the codebase

**Structure**:
```c
typedef struct s_file_compliance
{
    char            *path;              // Relative path from repo root
    bool            norminette_pass;    // True if passes norminette
    int             violation_count;    // Number of norminette violations
    char            **violations;       // Array of violation descriptions
    bool            has_forbidden_funcs; // True if uses memset/memcpy/memmove
    int             line_count;         // Total lines in file
    int             function_count;     // Number of functions in file
    char            *hash_sha256;       // SHA256 hash for duplicate detection
}   t_file_compliance;
```

**Validation Rules**:
- `path` must be valid file path within src/ or includes/
- `norminette_pass` is true only if `violation_count` == 0
- `hash_sha256` must be 64-character hexadecimal string
- `violations` array size must match `violation_count`

**State Transitions**:
```
INITIAL (unchecked) 
    → SCANNED (metadata collected)
    → VIOLATED (norminette failures)
    → FIXED (passes all checks)
```

### 2. Function Metadata

**Purpose**: Track individual function compliance for function-level refactoring

**Structure**:
```c
typedef struct s_function_compliance
{
    char            *name;              // Function name
    char            *file_path;         // Source file containing function
    int             start_line;         // Starting line number
    int             end_line;           // Ending line number
    int             line_count;         // Total lines (end - start)
    int             parameter_count;    // Number of parameters
    bool            is_static;          // True if static function
    bool            exceeds_line_limit; // True if >25 lines
    bool            exceeds_param_limit; // True if >4 params
    char            **param_names;      // Array of parameter names
}   t_function_compliance;
```

**Validation Rules**:
- `line_count` must equal `end_line - start_line + 1`
- `exceeds_line_limit` is true if `line_count > 25`
- `exceeds_param_limit` is true if `parameter_count > 4`
- `param_names` array size must match `parameter_count`
- If norminette compliant: `!exceeds_line_limit && !exceeds_param_limit`

### 3. Duplicate File Group

**Purpose**: Group files with identical content for consolidation

**Structure**:
```c
typedef struct s_duplicate_group
{
    char            *hash_sha256;       // Shared hash of duplicates
    char            **file_paths;       // Array of duplicate file paths
    int             file_count;         // Number of duplicates
    char            *canonical_path;    // Chosen canonical version
    bool            consolidated;       // True if duplicates removed
    char            *consolidation_date; // ISO date of consolidation
}   t_duplicate_group;
```

**Validation Rules**:
- `file_count` must be >= 2 (minimum 2 files to be duplicates)
- All files in `file_paths` must have identical `hash_sha256`
- `canonical_path` must be one of the paths in `file_paths`
- `consolidated` can only be true if `canonical_path` is set

**State Transitions**:
```
DETECTED (duplicates identified)
    → ANALYZED (canonical chosen)
    → CONSOLIDATED (duplicates removed)
    → VERIFIED (build passes)
```

### 4. Forbidden Function Usage

**Purpose**: Track usage of non-allowed functions for replacement

**Structure**:
```c
typedef struct s_forbidden_usage
{
    char            *function_name;     // e.g., "memcpy", "memset"
    char            *file_path;         // File containing usage
    int             line_number;        // Line number of usage
    char            *context;           // Code context (3 lines)
    char            *replacement_func;  // e.g., "ft_memcpy"
    bool            replaced;           // True if replaced
    bool            verified;           // True if replacement tested
}   t_forbidden_usage;
```

**Validation Rules**:
- `function_name` must be in forbidden list: {memcpy, memset, memmove, strcpy, strcat, strncpy, strncat}
- `replacement_func` must be implemented in `src/utils/memory.c`
- `verified` can only be true if `replaced` is true
- `context` should include 1 line before + line itself + 1 line after

### 5. Validation Result

**Purpose**: Store results of pixel-diff and memory validation

**Structure**:
```c
typedef struct s_validation_result
{
    char            *scene_file;        // Scene file path
    bool            render_success;     // True if render completed
    double          pixel_diff_pct;     // Percentage difference (0.0-100.0)
    bool            pixel_diff_pass;    // True if <0.1%
    int             memory_leaked_bytes; // Bytes leaked (valgrind)
    bool            memory_pass;        // True if 0 bytes leaked
    char            *error_message;     // Error if any check failed
    char            *timestamp;         // ISO timestamp of validation
}   t_validation_result;
```

**Validation Rules**:
- `pixel_diff_pass` is true only if `pixel_diff_pct < 0.1`
- `memory_pass` is true only if `memory_leaked_bytes == 0`
- Both `pixel_diff_pass` and `memory_pass` must be true for overall pass
- `error_message` is NULL if all checks pass

### 6. Refactoring Phase State

**Purpose**: Track progress through 10 implementation phases

**Structure**:
```c
typedef enum e_phase_status
{
    PHASE_NOT_STARTED,
    PHASE_IN_PROGRESS,
    PHASE_COMPLETED,
    PHASE_FAILED,
    PHASE_SKIPPED
}   t_phase_status;

typedef struct s_phase_state
{
    int             phase_number;       // 0-10
    char            *phase_name;        // e.g., "Baseline Capture"
    t_phase_status  status;             // Current status
    char            *start_time;        // ISO timestamp
    char            *end_time;          // ISO timestamp
    int             files_processed;    // Files affected in this phase
    int             files_modified;     // Files actually changed
    char            **artifacts;        // Generated files/reports
    int             artifact_count;     // Number of artifacts
    bool            validation_passed;  // True if phase validation passed
}   t_phase_state;
```

**Validation Rules**:
- `phase_number` must be 0-10 inclusive
- `status` can only be `COMPLETED` if `validation_passed` is true
- `end_time` must be after `start_time`
- `files_modified` must be <= `files_processed`

**Phase Sequence**:
```
Phase 0:  Baseline Capture
Phase 1:  Duplicate Detection
Phase 2:  Duplicate Consolidation
Phase 3:  Forbidden Functions Audit
Phase 4:  Function Replacement
Phase 5:  Norminette First Pass
Phase 6:  Function Splitting
Phase 7:  Parameter Reduction
Phase 8:  Style Fixes
Phase 9:  Verification
Phase 10: Documentation Update
```

## Relationships

### File → Function (One-to-Many)
- One source file contains multiple functions
- Each function belongs to exactly one file
- File compliance depends on all contained functions being compliant

### Duplicate Group → Files (One-to-Many)
- One duplicate group contains multiple identical files
- Each file can be in at most one duplicate group
- Consolidation removes all but canonical file

### Validation Result → Scene File (One-to-One)
- Each scene file has one validation result per validation run
- Multiple validation runs tracked over time
- Latest validation result determines current compliance state

### Phase → Files (Many-to-Many)
- Each phase processes multiple files
- Each file may be processed in multiple phases
- Phase completion depends on all processed files passing validation

## Data Storage

### Tracking Files

**Location**: `specs/016-compliance-refactoring/tracking/`

**Files**:
- `file_compliance.json`: Array of t_file_compliance records
- `function_compliance.json`: Array of t_function_compliance records
- `duplicates.json`: Array of t_duplicate_group records
- `forbidden_usage.json`: Array of t_forbidden_usage records
- `validation_results.json`: Array of t_validation_result records
- `phase_state.json`: Array of t_phase_state records

**Format**: JSON for human readability and easy scripting

### Baselines

**Location**: `tests/baselines/`

**Files**:
- `*.ppm`: Rendered images for pixel-diff comparison
- `baseline_metadata.json`: Metadata about baseline generation

**Format**: PPM (Portable Pixel Map) for renders, JSON for metadata

## Query Patterns

### Find all non-compliant files
```json
SELECT * FROM file_compliance 
WHERE norminette_pass = false 
ORDER BY violation_count DESC
```

### Find functions needing splitting
```json
SELECT * FROM function_compliance 
WHERE exceeds_line_limit = true 
ORDER BY line_count DESC
```

### Find duplicate groups not yet consolidated
```json
SELECT * FROM duplicate_group 
WHERE consolidated = false
```

### Find unreplaced forbidden functions
```json
SELECT * FROM forbidden_usage 
WHERE replaced = false
```

### Get latest validation results
```json
SELECT * FROM validation_result 
ORDER BY timestamp DESC 
LIMIT 38  -- One per scene file
```

### Get phase progress
```json
SELECT phase_number, phase_name, status, validation_passed 
FROM phase_state 
ORDER BY phase_number ASC
```

## Data Integrity Constraints

### Referential Integrity
- `t_function_compliance.file_path` must reference valid `t_file_compliance.path`
- `t_forbidden_usage.file_path` must reference valid `t_file_compliance.path`
- `t_duplicate_group.canonical_path` must be in `t_duplicate_group.file_paths`

### Business Rules
- A file cannot be marked `norminette_pass = true` if any function in it has `exceeds_line_limit = true` or `exceeds_param_limit = true`
- A phase cannot have `status = COMPLETED` unless `validation_passed = true`
- A duplicate group cannot have `consolidated = true` unless exactly one file remains in the repository

### Immutability
- Baseline renders once generated should not be modified
- Historical validation results should be preserved for audit trail
- Phase state history should be append-only

## Migration Strategy

This data model is metadata-only and doesn't affect runtime code. No data migration is required. Tracking files are generated fresh during refactoring and discarded after completion.

## Extensibility

Future enhancements could add:
- `t_performance_metric`: Track compilation time, render time per phase
- `t_code_metric`: Track cyclomatic complexity, maintainability index
- `t_review_feedback`: Store peer review comments per file
- `t_test_coverage`: Track which test scenes exercise which functions

## Summary

This data model provides:
- **Comprehensive tracking** of compliance state for 75 source files
- **Granular metadata** for 300+ functions requiring validation
- **Duplicate detection** with hash-based exact matching
- **Validation results** with pixel-diff and memory leak tracking
- **Phase progress** monitoring across 11 implementation phases
- **Audit trail** through JSON-based tracking files

The model supports the entire refactoring workflow from initial audit through final validation, enabling automated reporting and progress tracking throughout the compliance refactoring process.
