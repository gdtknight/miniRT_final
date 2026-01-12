# Data Model: BVH Tree Visualization

**Feature**: BVH Tree Visualization  
**Date**: 2026-01-12  
**Status**: Design Complete

## Overview

This document defines the data structures, state management, and entity relationships for the BVH tree visualization system. All structures follow 42 School norminette standards.

## Entity Definitions

### 1. BVH Node (Modified)

**Purpose**: Represents a node in the BVH hierarchy with visualization support

**Structure**:
```c
typedef struct s_bvh_node
{
    t_aabb              bounds;          // Spatial bounding box
    struct s_bvh_node   *left;          // Left child (NULL for leaf)
    struct s_bvh_node   *right;         // Right child (NULL for leaf)
    t_object_ref        *objects;       // Object array (non-NULL for leaf)
    int                 object_count;   // Number of objects in leaf
    int                 depth;          // NEW: Depth level (0 = root)
}   t_bvh_node;
```

**Fields**:
- `bounds`: AABB defining node's spatial coverage (min/max 3D coordinates)
- `left`: Pointer to left child node (NULL indicates leaf node)
- `right`: Pointer to right child node (NULL indicates leaf node)
- `objects`: Array of object references (non-NULL only for leaf nodes)
- `object_count`: Number of objects in leaf (0 for internal nodes)
- `depth`: **NEW FIELD** - Integer depth level, 0 at root, increments for each level

**Invariants**:
- If `left == NULL && right == NULL`, node is a leaf and `objects != NULL`
- If `left != NULL || right != NULL`, node is internal and `objects == NULL`
- `depth >= 0` always
- `depth` of children equals `parent->depth + 1`

**Validation Rules**:
- Leaf nodes must have `object_count > 0`
- Internal nodes must have both left and right children (no single-child nodes in balanced BVH)
- `bounds` must fully contain all child bounds and object bounds

**State Transitions**: None (immutable after construction)

### 2. BVH Tree (Modified)

**Purpose**: Root structure managing the complete BVH tree with visualization settings

**Structure**:
```c
typedef struct s_bvh
{
    t_bvh_node  *root;              // Root node of tree
    int         enabled;            // BVH acceleration enabled flag
    int         total_nodes;        // Total node count
    int         max_depth;          // Maximum depth in tree
    int         visualize;          // NEW: Visualization enabled flag
}   t_bvh;
```

**Fields**:
- `root`: Pointer to root node of BVH tree
- `enabled`: Whether BVH acceleration is active for ray tracing
- `total_nodes`: Cached count of all nodes in tree
- `max_depth`: Cached maximum depth value
- `visualize`: **NEW FIELD** - Boolean flag to enable visualization (0=disabled, 1=enabled)

**Invariants**:
- If `enabled == 1`, then `root != NULL`
- `total_nodes > 0` when tree is built
- `max_depth >= 0` when tree is built
- `visualize` is independent of `enabled` (can visualize even if acceleration disabled)

### 3. Visualization Configuration

**Purpose**: Controls visualization output formatting and behavior

**Structure**:
```c
typedef struct s_vis_config
{
    int     max_depth_display;      // Maximum depth to display (-1 = unlimited)
    int     compact_mode;           // Compact format flag (0=detailed, 1=compact)
    int     terminal_width;         // Detected terminal width in characters
    int     show_warnings;          // Display warning messages flag
}   t_vis_config;
```

**Fields**:
- `max_depth_display`: Limits how many tree levels to display (-1 means show all)
- `compact_mode`: Toggle between detailed and abbreviated bounding box format
- `terminal_width`: Runtime-detected terminal width for layout adaptation
- `show_warnings`: Whether to display edge case warnings to stderr

**Default Values**:
```c
t_vis_config default_config = {
    .max_depth_display = -1,        // Show all levels by default
    .compact_mode = 0,              // Detailed view by default
    .terminal_width = 80,           // Fallback to standard width
    .show_warnings = 1              // Warnings enabled by default
};
```

### 4. Tree Prefix State

**Purpose**: Manages ASCII prefix characters during recursive tree traversal

**Structure**:
```c
typedef struct s_prefix_state
{
    char    *buffer;        // Dynamic character buffer
    int     capacity;       // Allocated buffer size
    int     length;         // Current prefix string length
    int     level;          // Current depth level being rendered
}   t_prefix_state;
```

**Fields**:
- `buffer`: Dynamically allocated string buffer for prefix characters
- `capacity`: Total allocated size of buffer (grows as needed)
- `length`: Current valid length of prefix string
- `level`: Current tree depth level being processed

**Operations**:
- `prefix_push(state, is_last_child)`: Adds prefix for descending into child node
- `prefix_pop(state)`: Removes last prefix segment when ascending
- `prefix_print(state)`: Outputs current prefix to stdout
- `prefix_destroy(state)`: Frees buffer and resets state

**Lifecycle**:
1. Initialize: Allocate initial buffer (40 chars = 10 levels × 4 chars/level)
2. Traverse: Push/pop as recursion descends/ascends
3. Destroy: Free buffer when visualization complete

### 5. Tree Statistics

**Purpose**: Aggregates BVH quality metrics for summary display

**Structure**:
```c
typedef struct s_bvh_stats
{
    int     total_nodes;            // Count of all nodes (internal + leaf)
    int     leaf_count;             // Count of leaf nodes only
    int     internal_count;         // Count of internal nodes only
    int     max_depth;              // Maximum depth found
    int     total_objects;          // Sum of objects across all leaves
    double  avg_objects_per_leaf;   // Average objects per leaf node
}   t_bvh_stats;
```

**Fields**:
- `total_nodes`: Sum of internal and leaf nodes
- `leaf_count`: Number of leaf nodes
- `internal_count`: Number of internal nodes (total_nodes - leaf_count)
- `max_depth`: Deepest level in tree
- `total_objects`: Sum of object_count across all leaves
- `avg_objects_per_leaf`: `total_objects / leaf_count`

**Calculation**:
```c
void calculate_stats(t_bvh *bvh, t_bvh_stats *stats)
{
    collect_stats_recursive(bvh->root, stats, 0);
    stats->internal_count = stats->total_nodes - stats->leaf_count;
    if (stats->leaf_count > 0)
        stats->avg_objects_per_leaf = 
            (double)stats->total_objects / stats->leaf_count;
    else
        stats->avg_objects_per_leaf = 0.0;
}
```

**Validation Rules**:
- `total_nodes >= 1` (at least root node)
- `leaf_count >= 1` (at least one leaf)
- `internal_count >= 0`
- `total_nodes == internal_count + leaf_count`
- `max_depth >= 0`

### 6. Node Display Info

**Purpose**: Formatted string representation of node information

**Structure**:
```c
typedef struct s_node_info
{
    char    type[16];           // "Internal" or "Leaf"
    char    bounds[128];        // Formatted bounding box string
    char    objects[64];        // Formatted object list (leaves only)
    int     depth;              // Node depth level
}   t_node_info;
```

**Fields**:
- `type`: Node type string ("Internal" or "Leaf")
- `bounds`: Pre-formatted bounds string, e.g., "min(-1.50, 2.00, 0.00) max(3.20, 5.40, 2.10)"
- `objects`: Pre-formatted object list, e.g., "Objects: [0, 3, 7]" (empty for internal nodes)
- `depth`: Node depth for display

**Format Examples**:

**Internal Node**:
```
├── Internal [depth=1] min(-5.00, -3.00, -2.00) max(10.00, 8.00, 6.00)
```

**Leaf Node (detailed)**:
```
└── Leaf [depth=2] min(0.50, 1.20, -1.00) max(3.00, 4.50, 2.00) Objects: [3, 7, 12]
```

**Leaf Node (compact)**:
```
└── Leaf [d=2] Objects: [3, 7, 12]
```

## Entity Relationships

### BVH Tree Hierarchy

```
t_bvh
  └── root: t_bvh_node (depth=0)
      ├── left: t_bvh_node (depth=1)
      │   ├── left: t_bvh_node (depth=2, leaf)
      │   │   └── objects: t_object_ref[] (array of object references)
      │   └── right: t_bvh_node (depth=2, leaf)
      │       └── objects: t_object_ref[] (array of object references)
      └── right: t_bvh_node (depth=1, leaf)
          └── objects: t_object_ref[] (array of object references)
```

**Relationship Rules**:
- Parent-child: `parent->left` and `parent->right` point to children
- Sibling: Siblings share same parent, no direct pointer relationship
- Depth propagation: `child->depth = parent->depth + 1`
- Object ownership: Only leaf nodes have `objects` array (internal nodes have NULL)

### Visualization Data Flow

```
1. Input: t_bvh + t_vis_config
   ↓
2. Initialize: t_prefix_state, t_bvh_stats
   ↓
3. Traverse: Recursive tree walk
   - For each node: format t_node_info
   - Update: t_prefix_state (push/pop)
   - Accumulate: t_bvh_stats
   ↓
4. Output: Print tree visualization + statistics summary
   ↓
5. Cleanup: Free t_prefix_state
```

## State Management

### Initialization Sequence

```c
// 1. Parse command-line flag
if (has_flag("--bvh-vis"))
    scene->bvh.visualize = 1;

// 2. Build BVH with depth tracking
bvh_build(&scene->bvh, objects, count, scene);

// 3. Initialize visualization if enabled
if (scene->bvh.visualize)
{
    t_vis_config config = create_default_config();
    config.terminal_width = detect_terminal_width();
    
    t_prefix_state prefix = init_prefix_state();
    t_bvh_stats stats = {0};
    
    // 4. Visualize tree
    visualize_tree(scene->bvh.root, &prefix, &config, &stats, 1);
    
    // 5. Display statistics
    print_statistics(&stats);
    
    // 6. Cleanup
    destroy_prefix_state(&prefix);
}
```

### Traversal State Transitions

```
State: (node, prefix, depth)

Visit Internal Node:
  1. Print prefix + node info
  2. Push "│   " to prefix (for left child)
  3. Recurse left: (node->left, prefix + "│   ", depth + 1)
  4. Pop prefix
  5. Push "    " to prefix (for right child, last sibling)
  6. Recurse right: (node->right, prefix + "    ", depth + 1)
  7. Pop prefix

Visit Leaf Node:
  1. Print prefix + node info + objects
  2. Return (no recursion)
```

## Memory Management

### Allocation Responsibilities

**Static/Stack**:
- `t_vis_config`: Stack-allocated in visualization entry point
- `t_bvh_stats`: Stack-allocated in visualization entry point
- `t_node_info`: Stack-allocated per node during traversal

**Dynamic/Heap**:
- `t_prefix_state.buffer`: Heap-allocated, grows as needed
- `t_bvh_node`: Already allocated during BVH construction (no new allocations)
- `t_bvh_node.objects`: Already allocated during BVH construction (no new allocations)

### Deallocation Rules

```c
// Visualization cleanup (only frees visualization-specific allocations)
void cleanup_visualization(t_prefix_state *prefix)
{
    if (prefix && prefix->buffer)
        free(prefix->buffer);
    prefix->buffer = NULL;
    prefix->capacity = 0;
    prefix->length = 0;
}

// Note: BVH nodes and object arrays are NOT freed here
// They remain allocated for ray tracing use
```

### Memory Overhead Analysis

**Per-node overhead**:
- Depth field: 4 bytes
- For 100-node tree: 400 bytes total

**Temporary visualization overhead**:
- Prefix buffer: 4 bytes × max_depth (reused during traversal)
- Config structure: 16 bytes
- Stats structure: 24 bytes
- For depth-10 tree: ~80 bytes temporary allocation

**Total overhead**: ~480 bytes for 100-node tree (negligible)

## Validation and Invariants

### BVH Node Invariants

```c
// Check node invariants
int validate_node(t_bvh_node *node, int expected_depth)
{
    if (!node) return (1);  // NULL is valid
    
    // Depth matches expectation
    if (node->depth != expected_depth) return (0);
    
    // Leaf node checks
    if (!node->left && !node->right)
    {
        if (!node->objects) return (0);  // Leaf must have objects
        if (node->object_count <= 0) return (0);  // Must have at least 1 object
    }
    
    // Internal node checks
    if (node->left || node->right)
    {
        if (node->objects) return (0);  // Internal must not have objects
        if (!node->left || !node->right) return (0);  // Must have both children
    }
    
    // Recursively validate children
    return (validate_node(node->left, expected_depth + 1) &&
            validate_node(node->right, expected_depth + 1));
}
```

### Statistics Invariants

```c
// Check statistics consistency
int validate_stats(t_bvh_stats *stats)
{
    if (stats->total_nodes < 1) return (0);
    if (stats->leaf_count < 1) return (0);
    if (stats->internal_count < 0) return (0);
    if (stats->total_nodes != stats->internal_count + stats->leaf_count)
        return (0);
    if (stats->max_depth < 0) return (0);
    if (stats->total_objects < stats->leaf_count) return (0);  // At least 1 per leaf
    if (stats->avg_objects_per_leaf < 1.0) return (0);
    
    return (1);
}
```

## Performance Considerations

### Data Structure Size

```
sizeof(t_bvh_node) = ~48 bytes (with new depth field)
  - t_aabb bounds: 24 bytes (6 doubles)
  - left pointer: 8 bytes
  - right pointer: 8 bytes
  - objects pointer: 8 bytes
  - object_count: 4 bytes
  - depth: 4 bytes
  - padding: ~0-8 bytes (depends on alignment)

For 100-node tree: ~4.8 KB total
```

### Access Patterns

**Cache-friendly**:
- Sequential depth propagation during construction
- Depth-first traversal during visualization (good spatial locality)
- Statistics collection reuses traversal path

**Not cache-friendly**:
- Random access to object array indices for formatting

### Optimization Opportunities

1. **Prefix Buffer Growth**: Use exponential growth (2x) instead of fixed increments
2. **String Formatting**: Pre-compute format strings for common coordinate ranges
3. **Statistics Caching**: Store stats in t_bvh structure if visualized multiple times

## Implementation Notes

### 42 School Compliance

**Allowed Operations**:
- Struct field access (all fields are standard C types)
- Pointer navigation (parent-child relationships)
- Integer arithmetic (depth calculation, statistics)
- Dynamic memory (malloc/free for prefix buffer)
- String formatting (printf family functions)

**No Prohibited Operations**:
- No threading or synchronization needed (single-threaded traversal)
- No external libraries (pure C standard library)
- No system calls beyond ioctl for terminal width

### Norminette Compliance

All structures and functions follow norminette rules:
- Line length ≤ 80 characters
- Function length ≤ 25 lines
- Maximum 4 functions per file
- Variable names follow conventions
- No global variables (state passed as parameters)

## Testing Strategy

### Unit Tests

1. **Node validation**: Test depth field correctness at each level
2. **Prefix management**: Test push/pop operations maintain correct state
3. **Statistics calculation**: Verify counts and averages for known trees
4. **Format functions**: Test coordinate formatting with edge cases

### Integration Tests

1. **Small tree** (3-5 objects): Verify complete tree visualization
2. **Medium tree** (10-20 objects): Check depth tracking accuracy
3. **Deep tree** (>20 levels): Test warning messages and depth limits
4. **Edge cases**: Empty tree, single node, extreme coordinates

### Validation Checklist

- [ ] All leaf nodes have object_count > 0
- [ ] All internal nodes have both children
- [ ] Depth increases by 1 at each level
- [ ] Statistics sum correctly (total = internal + leaf)
- [ ] Prefix buffer never overflows
- [ ] Memory is freed properly (no leaks)
- [ ] Output fits within terminal width
- [ ] Coordinate precision is exactly 2 decimal places
