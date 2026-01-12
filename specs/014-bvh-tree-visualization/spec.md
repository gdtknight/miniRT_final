# Feature Specification: BVH Tree Visualization

**Feature Branch**: `014-bvh-tree-visualization`  
**Created**: 2026-01-12  
**Status**: Draft  
**Input**: User description: "Create a feature specification for visualizing the BVH (Bounding Volume Hierarchy) tree structure in the terminal after it's generated."

## Clarifications

### Session 2026-01-12

- Q: Should summary statistics (total nodes, max depth, leaf count, avg objects/leaf) be displayed? → A: Display statistics after tree visualization
- Q: What is the acceptable performance overhead for visualization? → A: < 1% of BVH construction time
- Q: How should the tree hierarchy be visually rendered? → A: Use tree characters: ├──, └──, │ in Unix tree command style
- Q: What precision should floating-point values (bounding box coordinates) display? → A: 2 decimal places
- Q: Should visualization be enabled by default or require explicit activation? → A: Disabled by default, enabled with flag (e.g., --bvh-vis)
- Q: How should the implementation be organized into source files for maximum maintainability? → A: Split into 5+ files for maximum granularity - add bvh_vis_prefix.c for prefix management, bvh_vis_print.c for output primitives
- Q: How should the visualization system obtain depth information for each node? → A: Store depth at construction time - adds one integer field per node, eliminates recursive depth calculation overhead during visualization
- Q: How should the system handle problematic edge cases (very deep trees, extremely narrow terminals, huge coordinate values)? → A: Print informative warning messages for each edge case scenario, then display visualization in adapted format
- Q: How should leaf nodes identify their contained objects in the visualization? → A: Each leaf node displays the list of contained object IDs (e.g., "Objects: [3, 7, 12]")
- Q: What identifier scheme should be used for object IDs in leaf node displays? → A: Use internal scene array index (0-based) for object identifiers

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Basic Tree Visualization (Priority: P1)

A developer runs the miniRT program with a scene file and sees the BVH tree structure displayed in the terminal after construction. The tree shows the hierarchical organization of bounding volumes using ASCII art, helping them understand how objects are spatially organized for ray tracing optimization.

**Why this priority**: This is the core functionality that delivers immediate value for debugging and understanding BVH structure. Without this, the feature provides no benefit.

**Independent Test**: Can be fully tested by running miniRT with any valid scene file and verifying that a tree structure appears in terminal output with ASCII tree formatting.

**Acceptance Scenarios**:

1. **Given** a scene file with 10 objects, **When** miniRT processes the scene and builds the BVH tree, **Then** a hierarchical ASCII tree representation is displayed in the terminal showing all nodes
2. **Given** the BVH tree has been constructed, **When** the visualization is displayed, **Then** internal nodes and leaf nodes are visually distinguishable in the output
3. **Given** a simple scene with 3 objects, **When** the tree is visualized, **Then** the tree hierarchy accurately reflects the BVH structure with correct parent-child relationships

---

### User Story 2 - Node Information Display (Priority: P2)

A developer examining the BVH visualization sees detailed information for each node, including bounding box coordinates, node type, object count for leaves, and depth level. After the tree display, summary statistics (total nodes, max depth, leaf count, average objects per leaf) provide a quick overview of the BVH structure quality. This allows them to verify that the BVH construction algorithm is working correctly and identify potential optimization opportunities.

**Why this priority**: Detailed node information is essential for meaningful debugging but depends on the basic visualization (P1) being in place first.

**Independent Test**: Can be tested by running miniRT with a known scene and verifying that each node in the tree display includes the specified information fields (type, bounds, object count, depth).

**Acceptance Scenarios**:

1. **Given** the BVH tree is displayed, **When** viewing an internal node, **Then** the node shows its bounding box min/max coordinates and depth level
2. **Given** the BVH tree is displayed, **When** viewing a leaf node, **Then** the node shows its bounding box coordinates, depth level, the number of objects it contains, and the list of contained object IDs using 0-based scene array indices (e.g., "Objects: [3, 7, 12]")
3. **Given** nodes at different depths, **When** examining the visualization, **Then** each node correctly displays its depth level starting from 0 at the root

---

### User Story 3 - Controlled Output Options (Priority: P3)

A developer working with large scenes can control the visualization output to avoid overwhelming terminal output. They can limit the maximum depth displayed, toggle between detailed and compact views, and enable/disable the visualization entirely for performance testing.

**Why this priority**: Output control improves usability for complex scenes but is not essential for the feature's core debugging value. Can be added after basic visualization works.

**Independent Test**: Can be tested by running miniRT with different command-line flags or configuration options and verifying that the visualization output changes accordingly (depth limiting, compact mode, disabled mode).

**Acceptance Scenarios**:

1. **Given** a command-line option to limit depth to 3 levels, **When** miniRT runs with this option, **Then** only the root and first 3 levels of the tree are displayed
2. **Given** a compact view option is enabled, **When** the tree is displayed, **Then** bounding box information is shown in abbreviated format instead of full coordinates
3. **Given** the --bvh-vis flag is not provided, **When** miniRT runs, **Then** no tree visualization appears and the program proceeds normally with zero performance overhead
4. **Given** the --bvh-vis flag is provided, **When** miniRT runs, **Then** the BVH tree visualization is displayed after construction completes

---

### Edge Cases

- What happens when the BVH tree is very deep (20+ levels)? Print warning message indicating deep tree detected (depth > configured threshold), then display visualization with adapted format (depth limiting applied if configured).
- What happens when a scene has only 1 or 2 objects? The tree visualization should handle minimal or degenerate tree structures gracefully.
- What happens when the terminal width is very narrow? Print warning message about narrow terminal width detected, then adapt visualization by truncating or abbreviating information to fit within constraints.
- What happens when bounding box coordinates have very large or very small values? Print warning message if coordinate magnitude exceeds display threshold, then format all coordinates to 2 decimal places regardless of magnitude (may result in scientific notation display).
- How does the system handle an empty scene with no objects? Should display appropriate message or show empty tree structure.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST display a visual representation of the BVH tree structure in the terminal after tree construction completes
- **FR-002**: System MUST render the tree hierarchy using tree box-drawing characters (├──, └──, │) in Unix tree command style
- **FR-003**: System MUST distinguish between internal nodes and leaf nodes in the visualization (e.g., different prefixes or labels)
- **FR-004**: System MUST display bounding box minimum and maximum coordinates for each node
- **FR-005**: System MUST display the list of contained object IDs for each leaf node using 0-based scene array indices (e.g., "Objects: [3, 7, 12]")
- **FR-006**: System MUST display the depth level for each node in the tree
- **FR-007**: System MUST support a maximum depth limit option to control how many levels of the tree are displayed
- **FR-008**: System MUST support a compact view option that abbreviates bounding box information
- **FR-009**: System MUST disable visualization by default and require explicit activation via command-line flag (e.g., --bvh-vis)
- **FR-010**: System MUST handle edge cases gracefully by printing informative warning messages for problematic scenarios (very deep trees > 20 levels, narrow terminals < 80 chars, extreme coordinate values), then displaying visualization in adapted format
- **FR-011**: System MUST format bounding box coordinates to 2 decimal places for consistent and readable output
- **FR-012**: System MUST display the visualization after BVH construction but before ray tracing begins
- **FR-013**: System MUST display summary statistics after the tree visualization, including total node count, maximum tree depth, leaf node count, and average objects per leaf

### Key Entities

- **BVH Node**: Represents a node in the hierarchy; contains bounding box information, depth level (stored as integer field at construction time), and references to children (for internal nodes) or object indices (for leaf nodes). Object indices are 0-based positions in the scene array.
- **Bounding Box**: Represents the spatial bounds of a node; defined by minimum and maximum 3D coordinates (x, y, z)
- **Visualization Configuration**: Contains settings that control the visualization output (max depth, compact mode, enabled/disabled status)

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Developer can view complete BVH tree structure for scenes with up to 100 objects in under 5 seconds after tree construction
- **SC-002**: Tree visualization accurately represents the hierarchical structure with 100% correctness (all parent-child relationships match actual BVH structure)
- **SC-003**: All node information (type, bounds, object IDs using 0-based scene array indices, depth) is displayed correctly for every node in the tree
- **SC-004**: Visualization output remains readable and fits within standard terminal width (80-120 characters) for trees up to 10 levels deep
- **SC-005**: Performance impact of visualization is < 1% of BVH construction time when enabled, and zero overhead when disabled
- **SC-006**: Developers can successfully debug BVH construction issues using the visualization within 2 minutes of examining the output
- **SC-007**: Compact view reduces output line count by at least 40% compared to detailed view while maintaining essential debugging information

## Assumptions

- The miniRT project already has a working BVH tree implementation that this feature will visualize
- Developers using this feature have access to a terminal that supports ASCII character output
- Standard terminal width is assumed to be at least 80 characters for proper visualization formatting
- The primary use case is development and debugging, not production rendering
- Command-line arguments or configuration files are acceptable methods for controlling visualization options; visualization is disabled by default and activated via --bvh-vis flag
- Bounding box coordinates are represented as floating-point numbers in 3D space
- The BVH tree construction completes before rendering begins, providing a natural point to insert the visualization
- Tree depth of 20+ levels is considered unusually deep and may indicate construction issues worth investigating

## Out of Scope

- Graphical (GUI) visualization of the BVH tree
- Interactive navigation of the tree structure (zooming, collapsing branches)
- Real-time visualization during tree construction
- Visualization of BVH traversal during ray tracing
- Export of visualization to file formats (SVG, PNG, etc.)
- Colored output or terminal formatting beyond basic ASCII
- Statistical analysis or profiling of BVH quality metrics
- Comparison visualization between different BVH construction methods
- Visualization of individual object geometry within bounding boxes
