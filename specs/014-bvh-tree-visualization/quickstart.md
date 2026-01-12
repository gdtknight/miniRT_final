# Quickstart Guide: BVH Tree Visualization

**Feature**: BVH Tree Visualization  
**Version**: 1.0  
**Date**: 2026-01-12

## Overview

This guide provides a quick introduction to using the BVH tree visualization feature in miniRT. The visualization displays the hierarchical structure of the Bounding Volume Hierarchy (BVH) used for ray tracing acceleration, helping developers understand and debug spatial partitioning.

## Quick Start (30 seconds)

```bash
# Build miniRT with visualization support
make

# Run with visualization enabled
./miniRT scenes/simple.rt --bvh-vis

# You'll see output like this:
# BVH Tree Structure:
# ├── Internal [depth=0] min(-5.00, -3.00, -2.00) max(10.00, 8.00, 6.00)
# │   ├── Leaf [depth=1] min(-5.00, -3.00, -2.00) max(0.00, 2.00, 1.00) Objects: [0, 1]
# │   └── Leaf [depth=1] min(3.00, 1.00, 0.00) max(10.00, 8.00, 6.00) Objects: [2, 3]
# 
# BVH Statistics:
#   Total nodes: 3
#   Leaf nodes: 2
#   Internal nodes: 1
#   Max depth: 1
#   Average objects per leaf: 2.00
```

## Basic Usage

### Enable Visualization

The visualization is **disabled by default** to avoid performance overhead. Enable it with the `--bvh-vis` flag:

```bash
./miniRT path/to/scene.rt --bvh-vis
```

### Disable Visualization

Simply omit the flag for normal operation:

```bash
./miniRT path/to/scene.rt
```

## Understanding the Output

### Tree Structure Visualization

The tree uses ASCII box-drawing characters to show parent-child relationships:

```
BVH Tree Structure:
├── Internal [depth=0] min(-10.00, -5.00, -3.00) max(15.00, 10.00, 8.00)
│   ├── Internal [depth=1] min(-10.00, -5.00, -3.00) max(2.00, 4.00, 3.00)
│   │   ├── Leaf [depth=2] min(-10.00, -5.00, -3.00) max(-2.00, 1.00, 0.00) Objects: [0, 1]
│   │   └── Leaf [depth=2] min(-1.00, 0.00, -1.00) max(2.00, 4.00, 3.00) Objects: [2]
│   └── Leaf [depth=1] min(5.00, 2.00, 1.00) max(15.00, 10.00, 8.00) Objects: [3, 4, 5]
```

**Symbols**:
- `├──` : Branch connector (not the last child)
- `└──` : Last branch connector
- `│   ` : Vertical line continuing the branch
- `    ` : Empty space after last branch

### Node Information

Each node displays:

**Internal Nodes**:
```
├── Internal [depth=1] min(-5.00, -3.00, -2.00) max(10.00, 8.00, 6.00)
    ^^^^^^^^ ^^^^^^^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    Type     Depth      Bounding box coordinates (min and max)
```

**Leaf Nodes**:
```
└── Leaf [depth=2] min(0.50, 1.20, -1.00) max(3.00, 4.50, 2.00) Objects: [3, 7, 12]
    ^^^^ ^^^^^^^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  ^^^^^^^^^^^^^^^^
    Type Depth       Bounding box coordinates                         Object IDs
```

**Object IDs**: 0-based indices into the scene's object array. For example, `[3, 7, 12]` means this leaf contains the 4th, 8th, and 13th objects from the scene file.

### Statistics Summary

After the tree, summary statistics are displayed:

```
BVH Statistics:
  Total nodes: 5
  Leaf nodes: 3
  Internal nodes: 2
  Max depth: 2
  Average objects per leaf: 2.67
```

**Metrics**:
- **Total nodes**: All nodes in the tree (internal + leaf)
- **Leaf nodes**: Nodes containing actual objects
- **Internal nodes**: Nodes that partition space into children
- **Max depth**: Deepest level in the tree (0 = root)
- **Average objects per leaf**: Average number of objects per leaf node

## Common Scenarios

### Debugging BVH Construction

**Problem**: Ray tracing is slow
**Solution**: Check if BVH is well-balanced

```bash
./miniRT scenes/complex.rt --bvh-vis | grep "Max depth"
# Max depth: 8  ← Good (log₂(n) for n objects)
# Max depth: 25 ← Bad (very deep, possibly degenerate tree)
```

A well-balanced BVH should have depth ≈ log₂(n) where n is the number of objects.

### Verifying Object Distribution

**Problem**: Some objects not rendering correctly
**Solution**: Verify objects are in the correct BVH leaves

```bash
./miniRT scenes/test.rt --bvh-vis | grep "Objects:"
# Objects: [0, 1]
# Objects: [2]
# Objects: [3, 4, 5]
```

Check that all expected object IDs appear in leaf nodes.

### Checking Spatial Partitioning

**Problem**: BVH doesn't seem to be accelerating rendering
**Solution**: Examine bounding box sizes and overlap

```bash
./miniRT scenes/scene.rt --bvh-vis
# Look for:
# - Very large bounding boxes (inefficient spatial queries)
# - Excessive overlap between sibling nodes
# - Leaves with many objects (>10 suggests splitting issues)
```

## Advanced Usage

### Interpreting Depth Levels

**Optimal depth**: log₂(number of objects)
- 10 objects → depth ~3-4
- 100 objects → depth ~6-7
- 1000 objects → depth ~9-10

**Warning signs**:
- Depth > 20: Very deep tree, possible degenerate structure
- Depth < log₂(n) - 2: Tree may be under-split (large leaves)
- Depth > log₂(n) + 5: Tree may be over-split (excessive internal nodes)

### Reading Bounding Boxes

Bounding boxes define the spatial extent of each node:

```
min(x_min, y_min, z_min) max(x_max, y_max, z_max)
```

**Example**:
```
min(-5.00, -3.00, -2.00) max(10.00, 8.00, 6.00)
```
- X range: -5.00 to 10.00 (width = 15.00)
- Y range: -3.00 to 8.00 (height = 11.00)
- Z range: -2.00 to 6.00 (depth = 8.00)

**Good partitioning**: Child bounding boxes are roughly equal in size
**Poor partitioning**: One child is much larger than the other

### Analyzing Statistics

**Healthy BVH indicators**:
- Average objects per leaf: 1-5 (good balance)
- Internal/leaf ratio: ~1:1 for binary tree
- Max depth: ≈ log₂(total objects)

**Problem indicators**:
- Average objects per leaf: >10 (under-split)
- Average objects per leaf: 1.0 exactly (possibly over-split)
- Max depth: >>log₂(n) (degenerate tree)

## Troubleshooting

### "BVH visualization is not displayed"

**Cause**: Forgot to enable visualization flag
**Solution**: Add `--bvh-vis` flag to command

```bash
./miniRT scenes/scene.rt --bvh-vis
```

### "Warning: BVH tree depth is X levels (very deep)"

**Cause**: Tree has depth > 20, possibly indicating construction issues
**Impact**: May affect rendering performance
**Solution**: Review BVH construction parameters, check for degenerate splits

### "Warning: Terminal width is X characters (narrow)"

**Cause**: Terminal is narrower than 80 characters
**Impact**: Output may be truncated or wrapped awkwardly
**Solution**: Widen terminal window or use compact mode (future feature)

### "Warning: Extreme coordinate values detected"

**Cause**: Bounding box coordinates are very large (>1e6) or very small (<1e-3)
**Impact**: Coordinates displayed in scientific notation
**Solution**: This is informational; check if scene coordinate scale is appropriate

### Tree output looks garbled

**Cause**: Terminal doesn't support box-drawing characters
**Solution**: Ensure terminal encoding is UTF-8:
```bash
export LANG=en_US.UTF-8
./miniRT scenes/scene.rt --bvh-vis
```

### Memory leak reported after visualization

**Cause**: Prefix buffer not freed properly (implementation bug)
**Solution**: Report issue; visualization should have zero memory leaks

## Performance Impact

### Overhead Measurement

Visualization adds minimal overhead:
- **With flag**: <1% of BVH construction time
- **Without flag**: 0% overhead (single boolean check)

**Example timing** (100-object scene):
```
BVH construction: 1.2 seconds
Visualization: 0.008 seconds (0.67% overhead)
```

### When to Enable

**Enable during**:
- Development and debugging
- BVH algorithm testing
- Scene setup verification
- Performance optimization

**Disable for**:
- Production rendering
- Benchmark tests
- Batch processing
- Automated CI/CD

## Examples

### Small Scene (3 objects)

```bash
./miniRT scenes/small.rt --bvh-vis
```

**Expected output**:
```
BVH Tree Structure:
├── Internal [depth=0] min(-2.00, -2.00, -2.00) max(5.00, 5.00, 5.00)
│   ├── Leaf [depth=1] min(-2.00, -2.00, -2.00) max(0.00, 0.00, 0.00) Objects: [0]
│   └── Leaf [depth=1] min(2.00, 2.00, 2.00) max(5.00, 5.00, 5.00) Objects: [1, 2]

BVH Statistics:
  Total nodes: 3
  Leaf nodes: 2
  Internal nodes: 1
  Max depth: 1
  Average objects per leaf: 1.50
```

### Medium Scene (20 objects)

```bash
./miniRT scenes/medium.rt --bvh-vis
```

**Expected output**: Tree with depth 4-5, approximately 40 nodes

### Large Scene (100 objects)

```bash
./miniRT scenes/large.rt --bvh-vis | head -20
```

**Expected output**: Tree with depth 6-7, may want to limit output with `head`

## Integration with Workflow

### Development Workflow

1. **Create/modify scene**
   ```bash
   vim scenes/test.rt
   ```

2. **Build with visualization**
   ```bash
   make
   ./miniRT scenes/test.rt --bvh-vis
   ```

3. **Analyze output**
   - Check depth is reasonable
   - Verify objects are correctly distributed
   - Look for spatial partitioning quality

4. **Optimize if needed**
   - Adjust scene object distribution
   - Modify BVH construction parameters (if exposed)

### CI/CD Integration

```bash
# Run test suite with visualization enabled
./test_miniRT.sh --bvh-vis

# Verify BVH structure in automated tests
./miniRT scenes/test.rt --bvh-vis 2>&1 | grep "Max depth" | awk '{print $3}' | ./check_depth_reasonable.sh
```

## FAQ

**Q: Does visualization affect rendering quality?**  
A: No, visualization only displays the BVH structure. It doesn't change ray tracing results.

**Q: Can I save visualization output to a file?**  
A: Yes, redirect stdout:
```bash
./miniRT scenes/scene.rt --bvh-vis > bvh_output.txt
```

**Q: Why are object IDs 0-based?**  
A: They match the internal scene array indices, making debugging easier.

**Q: Can I visualize only part of the tree?**  
A: Not in the initial version. Future extensions may add depth limiting.

**Q: Does this work on macOS?**  
A: Yes, the feature uses POSIX-compliant functions that work on both Linux and macOS.

**Q: What if my tree has 1000+ nodes?**  
A: Output will be very long. Consider piping to `less` or `head`:
```bash
./miniRT scenes/huge.rt --bvh-vis | less
```

## Next Steps

- **Read the API documentation**: [contracts/visualization_api.md](contracts/visualization_api.md)
- **Understand the data model**: [data-model.md](data-model.md)
- **Review implementation details**: [plan.md](plan.md)
- **Check research decisions**: [research.md](research.md)

## Support

For issues or questions:
1. Check this guide and the troubleshooting section
2. Review the feature specification: [spec.md](spec.md)
3. Consult the API contract: [contracts/visualization_api.md](contracts/visualization_api.md)
4. Open an issue in the repository with:
   - Scene file used
   - Command executed
   - Expected vs. actual output
   - Terminal environment details

## Version History

- **1.0** (2026-01-12): Initial release
  - Basic tree visualization with ASCII art
  - Statistics summary display
  - Command-line flag integration
  - Edge case handling and warnings
