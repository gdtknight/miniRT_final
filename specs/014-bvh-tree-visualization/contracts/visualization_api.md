# API Contract: BVH Visualization

**Feature**: BVH Tree Visualization  
**Version**: 1.0  
**Date**: 2026-01-12

## Overview

This document defines the public API contracts for the BVH visualization module. All functions follow 42 School norminette standards and use only allowed functions.

## Public API Functions

### 1. Main Visualization Entry Point

```c
/**
 * @brief Visualizes the complete BVH tree structure to stdout
 * 
 * Displays hierarchical tree structure with ASCII box-drawing characters,
 * node information (type, bounds, depth, objects), and summary statistics.
 * 
 * @param bvh Pointer to BVH tree structure (must not be NULL)
 * @param config Visualization configuration (NULL uses default config)
 * @return 0 on success, -1 on error (NULL bvh, invalid tree structure)
 * 
 * @note Output is written to stdout, warnings to stderr
 * @note Performance: O(n) where n is total number of nodes
 * @note Memory: Allocates temporary buffer for prefix strings (~4 bytes per depth level)
 */
int bvh_visualize(t_bvh *bvh, t_vis_config *config);
```

**Preconditions**:
- `bvh != NULL`
- `bvh->root != NULL` (tree must be built)
- All nodes have valid depth field
- Tree structure is valid (no cycles, correct parent-child relationships)

**Postconditions**:
- Tree structure printed to stdout with proper formatting
- Statistics summary printed to stdout after tree
- Warning messages (if any) printed to stderr before tree
- No memory leaks
- BVH structure unchanged (read-only operation)

**Error Handling**:
- Returns -1 if `bvh == NULL`
- Returns -1 if tree structure validation fails
- Prints error message to stderr on failure
- Does not modify program state on error

**Example Usage**:
```c
// Basic usage with default configuration
if (scene->bvh.visualize)
{
    if (bvh_visualize(&scene->bvh, NULL) != 0)
        fprintf(stderr, "Error: Failed to visualize BVH tree\n");
}

// Custom configuration
t_vis_config config = {
    .max_depth_display = 5,
    .compact_mode = 1,
    .terminal_width = 100,
    .show_warnings = 1
};
bvh_visualize(&scene->bvh, &config);
```

### 2. Configuration Initialization

```c
/**
 * @brief Creates default visualization configuration
 * 
 * @return Default configuration structure with sensible defaults
 * 
 * @note Always succeeds (no dynamic allocation)
 */
t_vis_config bvh_vis_default_config(void);
```

**Postconditions**:
- Returns initialized config with:
  - `max_depth_display = -1` (show all levels)
  - `compact_mode = 0` (detailed view)
  - `terminal_width = 80` (standard fallback)
  - `show_warnings = 1` (warnings enabled)

**Example Usage**:
```c
t_vis_config config = bvh_vis_default_config();
config.max_depth_display = 10;  // Limit to 10 levels
config.compact_mode = 1;         // Use compact format
bvh_visualize(&scene->bvh, &config);
```

### 3. Terminal Width Detection

```c
/**
 * @brief Detects current terminal width using ioctl
 * 
 * @return Terminal width in characters, or 80 if detection fails
 * 
 * @note Uses POSIX ioctl(STDOUT_FILENO, TIOCGWINSZ) for detection
 * @note Fallback to 80 characters when stdout is redirected or not a TTY
 */
int bvh_vis_get_terminal_width(void);
```

**Postconditions**:
- Returns actual terminal width if stdout is a TTY
- Returns 80 if stdout is not a TTY (redirected to file/pipe)
- Never returns value less than 1

**Example Usage**:
```c
t_vis_config config = bvh_vis_default_config();
config.terminal_width = bvh_vis_get_terminal_width();
if (config.terminal_width < 80)
    fprintf(stderr, "Warning: Terminal width is narrow (%d chars)\n", 
            config.terminal_width);
```

### 4. Statistics Collection

```c
/**
 * @brief Collects comprehensive statistics about BVH tree structure
 * 
 * @param bvh Pointer to BVH tree (must not be NULL)
 * @param stats Pointer to statistics structure to fill (must not be NULL)
 * @return 0 on success, -1 on error
 * 
 * @note Performance: O(n) single-pass traversal
 */
int bvh_collect_statistics(t_bvh *bvh, t_bvh_stats *stats);
```

**Preconditions**:
- `bvh != NULL`
- `stats != NULL`
- `bvh->root != NULL`

**Postconditions**:
- `stats` structure filled with accurate metrics
- All invariants satisfied (total = internal + leaf, etc.)
- BVH structure unchanged (read-only)

**Example Usage**:
```c
t_bvh_stats stats;
if (bvh_collect_statistics(&scene->bvh, &stats) == 0)
{
    printf("Total nodes: %d\n", stats.total_nodes);
    printf("Leaf nodes: %d\n", stats.leaf_count);
    printf("Max depth: %d\n", stats.max_depth);
    printf("Avg objects/leaf: %.2f\n", stats.avg_objects_per_leaf);
}
```

## Internal Module Functions

These functions are part of the visualization module but are not exposed in the public API. They are documented here for completeness.

### Prefix Management (bvh_vis_prefix.c)

```c
/**
 * @brief Initializes prefix state with initial buffer
 * @return Initialized prefix state structure
 */
t_prefix_state prefix_init(void);

/**
 * @brief Pushes prefix segment for child node
 * @param state Prefix state to modify
 * @param is_last_child Whether this is the last child of parent
 */
void prefix_push(t_prefix_state *state, int is_last_child);

/**
 * @brief Pops prefix segment after processing child
 * @param state Prefix state to modify
 */
void prefix_pop(t_prefix_state *state);

/**
 * @brief Prints current prefix to stdout
 * @param state Prefix state to print
 */
void prefix_print(t_prefix_state *state);

/**
 * @brief Destroys prefix state and frees buffer
 * @param state Prefix state to destroy
 */
void prefix_destroy(t_prefix_state *state);
```

### Node Formatting (bvh_vis_node.c)

```c
/**
 * @brief Formats node information for display
 * @param node BVH node to format
 * @param compact Use compact format if non-zero
 * @return Formatted node info structure (stack-allocated)
 */
t_node_info format_node_info(t_bvh_node *node, int compact);

/**
 * @brief Formats bounding box coordinates to string
 * @param bounds AABB to format
 * @param buffer Output buffer (must be at least 128 bytes)
 * @param compact Use compact format if non-zero
 */
void format_bounds(t_aabb bounds, char *buffer, int compact);

/**
 * @brief Formats object list to string
 * @param objects Array of object references
 * @param count Number of objects
 * @param buffer Output buffer (must be at least 64 bytes)
 */
void format_objects(t_object_ref *objects, int count, char *buffer);
```

### Tree Traversal (bvh_vis_tree.c)

```c
/**
 * @brief Recursively visualizes BVH subtree
 * @param node Current node to visualize
 * @param prefix Prefix state for tree drawing
 * @param config Visualization configuration
 * @param stats Statistics accumulator
 * @param is_last_child Whether this is the last child of parent
 */
void visualize_node_recursive(t_bvh_node *node, t_prefix_state *prefix,
                               t_vis_config *config, t_bvh_stats *stats,
                               int is_last_child);
```

### Statistics Display (bvh_vis_stats.c)

```c
/**
 * @brief Prints formatted statistics summary
 * @param stats Statistics structure to display
 */
void print_statistics_summary(t_bvh_stats *stats);
```

### Initialization and Validation (bvh_vis_init.c)

```c
/**
 * @brief Validates BVH tree structure before visualization
 * @param bvh BVH tree to validate
 * @return 1 if valid, 0 if invalid
 */
int validate_bvh_structure(t_bvh *bvh);

/**
 * @brief Checks for edge cases and prints warnings
 * @param bvh BVH tree to check
 * @param config Configuration to check
 */
void check_and_warn_edge_cases(t_bvh *bvh, t_vis_config *config);
```

## Data Type Contracts

### t_vis_config

```c
typedef struct s_vis_config
{
    int max_depth_display;   // -1 = unlimited, >= 0 = depth limit
    int compact_mode;        // 0 = detailed, 1 = compact
    int terminal_width;      // Must be > 0, typically 80-200
    int show_warnings;       // 0 = silent, 1 = show warnings
}   t_vis_config;
```

**Invariants**:
- `max_depth_display >= -1`
- `terminal_width > 0`
- `compact_mode` is 0 or 1
- `show_warnings` is 0 or 1

### t_bvh_stats

```c
typedef struct s_bvh_stats
{
    int     total_nodes;
    int     leaf_count;
    int     internal_count;
    int     max_depth;
    int     total_objects;
    double  avg_objects_per_leaf;
}   t_bvh_stats;
```

**Invariants**:
- `total_nodes >= 1`
- `leaf_count >= 1`
- `internal_count >= 0`
- `total_nodes == internal_count + leaf_count`
- `max_depth >= 0`
- `total_objects >= leaf_count`
- `avg_objects_per_leaf >= 1.0`

### t_prefix_state

```c
typedef struct s_prefix_state
{
    char    *buffer;
    int     capacity;
    int     length;
    int     level;
}   t_prefix_state;
```

**Invariants**:
- `buffer != NULL` after initialization
- `capacity > 0` after initialization
- `0 <= length < capacity`
- `level >= 0`
- `length == level * 4` (4 characters per depth level)

## Output Format Specification

### Tree Visualization Format

```
BVH Tree Structure:
├── Internal [depth=0] min(-10.00, -5.00, -3.00) max(15.00, 10.00, 8.00)
│   ├── Internal [depth=1] min(-10.00, -5.00, -3.00) max(2.00, 4.00, 3.00)
│   │   ├── Leaf [depth=2] min(-10.00, -5.00, -3.00) max(-2.00, 1.00, 0.00) Objects: [0, 1]
│   │   └── Leaf [depth=2] min(-1.00, 0.00, -1.00) max(2.00, 4.00, 3.00) Objects: [2]
│   └── Leaf [depth=1] min(5.00, 2.00, 1.00) max(15.00, 10.00, 8.00) Objects: [3, 4, 5]

BVH Statistics:
  Total nodes: 5
  Leaf nodes: 3
  Internal nodes: 2
  Max depth: 2
  Average objects per leaf: 2.67
```

### Compact Format

```
BVH Tree Structure:
├── Internal [d=0]
│   ├── Internal [d=1]
│   │   ├── Leaf [d=2] Objects: [0, 1]
│   │   └── Leaf [d=2] Objects: [2]
│   └── Leaf [d=1] Objects: [3, 4, 5]

BVH Statistics:
  Total: 5 nodes (3 leaves, 2 internal)
  Max depth: 2
  Avg objects/leaf: 2.67
```

### Tree Box-Drawing Characters

- `├──` : Internal branch connector
- `└──` : Last branch connector
- `│   ` : Vertical continuation line
- `    ` : Empty space after last branch

## Error Codes and Messages

### Return Codes

- `0` : Success
- `-1` : Error (NULL pointer, invalid structure, allocation failure)

### Error Messages (stderr)

```c
"Error: BVH pointer is NULL\n"
"Error: BVH root is NULL (tree not built)\n"
"Error: Invalid BVH tree structure detected\n"
"Error: Failed to allocate prefix buffer\n"
"Error: Statistics pointer is NULL\n"
```

### Warning Messages (stderr)

```c
"Warning: BVH tree depth is %d levels (very deep). Consider reviewing construction parameters.\n"
"Warning: Terminal width is %d characters (narrow). Output may be truncated.\n"
"Warning: Extreme coordinate values detected. Using scientific notation.\n"
```

## Performance Guarantees

### Time Complexity

- `bvh_visualize()`: O(n) where n = total nodes
- `bvh_collect_statistics()`: O(n) where n = total nodes
- `bvh_vis_get_terminal_width()`: O(1)
- `bvh_vis_default_config()`: O(1)

### Space Complexity

- Prefix buffer: O(d) where d = max depth (4 bytes per level)
- Statistics: O(1) constant space
- Total temporary allocation: O(d) ≈ 40-80 bytes typical

### Performance Target

- Visualization overhead: <1% of BVH construction time
- For 100-node tree: <10ms visualization time
- Memory overhead: <1KB temporary allocation

## Integration Points

### Command-Line Parsing

```c
// In parser module (src/parser/)
if (strcmp(argv[i], "--bvh-vis") == 0)
    scene->bvh.visualize = 1;
```

### Main Program Flow

```c
// In main.c, after BVH construction
scene_build_bvh(scene);

if (scene->bvh.visualize)
{
    t_vis_config config = bvh_vis_default_config();
    config.terminal_width = bvh_vis_get_terminal_width();
    
    if (bvh_visualize(&scene->bvh, &config) != 0)
        fprintf(stderr, "Error: BVH visualization failed\n");
}

// Continue with rendering...
```

### BVH Construction Modifications

```c
// In bvh_build_recursive() - add depth parameter
t_bvh_node *bvh_build_recursive(t_object_ref *objects, int count,
                                void *scene, int depth)
{
    t_bvh_node *node = create_node();
    node->depth = depth;  // Store depth during construction
    
    // ... existing construction logic ...
    
    if (should_split)
    {
        node->left = bvh_build_recursive(left_objects, left_count, 
                                         scene, depth + 1);
        node->right = bvh_build_recursive(right_objects, right_count, 
                                          scene, depth + 1);
    }
    
    return node;
}
```

## Testing Contracts

### Unit Test Expectations

```c
// Test: Empty BVH should return error
assert(bvh_visualize(NULL, NULL) == -1);

// Test: Valid BVH should succeed
t_bvh bvh = create_test_bvh();
assert(bvh_visualize(&bvh, NULL) == 0);

// Test: Statistics should be accurate
t_bvh_stats stats;
bvh_collect_statistics(&bvh, &stats);
assert(stats.total_nodes == expected_count);
assert(stats.leaf_count + stats.internal_count == stats.total_nodes);
```

### Integration Test Expectations

```bash
# Test: Visualization enabled by flag
./miniRT scenes/test.rt --bvh-vis 2>&1 | grep "BVH Tree Structure"

# Test: No visualization without flag
./miniRT scenes/test.rt 2>&1 | grep -v "BVH Tree Structure"

# Test: Statistics displayed after tree
./miniRT scenes/test.rt --bvh-vis 2>&1 | grep "BVH Statistics"
```

## Versioning and Compatibility

**API Version**: 1.0  
**Stability**: Stable  
**Backward Compatibility**: N/A (new feature)

**Future Extensions** (not in current scope):
- Additional output formats (JSON, XML)
- Interactive navigation
- Color terminal support
- File export capability

## 42 School Compliance

**Allowed Functions Used**:
- `printf`, `fprintf` (stdio.h) - output formatting
- `malloc`, `free` (stdlib.h) - prefix buffer allocation
- `ioctl` (sys/ioctl.h) - terminal width detection
- `strcmp` (libft) - command-line parsing
- `fabs` (math.h) - coordinate formatting

**No Prohibited Functions**:
- No pthread usage
- No fork/pipe usage
- No external libraries beyond standard C and 42 allowed list

## Documentation References

- [Feature Specification](../spec.md)
- [Implementation Plan](../plan.md)
- [Research Documentation](../research.md)
- [Data Model](../data-model.md)
- [Quickstart Guide](../quickstart.md)
