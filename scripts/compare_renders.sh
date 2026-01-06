#!/bin/bash
# Compare rendered images between baseline and current
# Usage: ./scripts/compare_renders.sh [baseline_dir] [current_dir]

set -e

BASELINE_DIR="${1:-baseline_renders}"
CURRENT_DIR="${2:-current_renders}"

echo "========================================="
echo "Render Comparison Script"
echo "========================================="
echo "Baseline: $BASELINE_DIR"
echo "Current:  $CURRENT_DIR"
echo ""

if [ ! -d "$BASELINE_DIR" ]; then
    echo "✗ ERROR: Baseline directory not found: $BASELINE_DIR"
    exit 1
fi

if [ ! -d "$CURRENT_DIR" ]; then
    echo "✗ ERROR: Current directory not found: $CURRENT_DIR"
    exit 1
fi

# Count files
baseline_count=$(find "$BASELINE_DIR" -name "*.ppm" -o -name "*.png" | wc -l | tr -d ' ')
current_count=$(find "$CURRENT_DIR" -name "*.ppm" -o -name "*.png" | wc -l | tr -d ' ')

echo "Baseline images: $baseline_count"
echo "Current images:  $current_count"
echo ""

if [ "$baseline_count" -eq 0 ]; then
    echo "✗ ERROR: No baseline images found"
    exit 1
fi

# Compare each image
pass_count=0
fail_count=0
missing_count=0

for baseline_img in "$BASELINE_DIR"/*.{ppm,png}; do
    [ -f "$baseline_img" ] || continue
    
    filename=$(basename "$baseline_img")
    current_img="$CURRENT_DIR/$filename"
    
    if [ ! -f "$current_img" ]; then
        echo "✗ MISSING: $filename"
        ((missing_count++))
        continue
    fi
    
    # Binary comparison
    if cmp -s "$baseline_img" "$current_img"; then
        echo "✓ IDENTICAL: $filename"
        ((pass_count++))
    else
        echo "✗ DIFFERS: $filename"
        ((fail_count++))
    fi
done

echo ""
echo "========================================="
echo "Results:"
echo "========================================="
echo "Identical:  $pass_count"
echo "Different:  $fail_count"
echo "Missing:    $missing_count"

if [ "$fail_count" -eq 0 ] && [ "$missing_count" -eq 0 ]; then
    echo "✓ PASS: All renders match baseline"
    exit 0
else
    echo "✗ FAIL: Some renders differ from baseline"
    exit 1
fi
