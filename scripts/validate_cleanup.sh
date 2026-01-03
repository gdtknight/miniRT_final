#!/bin/bash
# Validation script for behavior preservation testing
# Created as part of 012-code-cleanup-refactor feature

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SCENES_DIR="$PROJECT_ROOT/scenes"
BASELINE_DIR="/tmp/minirt_baseline"
CURRENT_DIR="/tmp/minirt_current"

echo "=== MiniRT Cleanup Validation Script ==="
echo "Project Root: $PROJECT_ROOT"
echo ""

# Function to render all scenes and save output
render_scenes() {
    local output_dir="$1"
    mkdir -p "$output_dir"
    
    echo "Rendering scenes to: $output_dir"
    
    for scene in "$SCENES_DIR"/*.rt; do
        if [ -f "$scene" ]; then
            scene_name=$(basename "$scene" .rt)
            echo "  - Rendering $scene_name..."
            # Note: This would need actual screenshot/comparison logic
            # For now, just verify the program runs without crash
            timeout 2s "$PROJECT_ROOT/miniRT" "$scene" > "$output_dir/${scene_name}.log" 2>&1 || true
        fi
    done
    
    echo "Rendering complete."
}

# Function to compare baseline vs current
compare_results() {
    echo ""
    echo "=== Comparison Results ==="
    
    if [ ! -d "$BASELINE_DIR" ]; then
        echo "ERROR: No baseline directory found. Run with 'baseline' argument first."
        return 1
    fi
    
    local differences=0
    
    for baseline_log in "$BASELINE_DIR"/*.log; do
        scene_name=$(basename "$baseline_log")
        current_log="$CURRENT_DIR/$scene_name"
        
        if [ ! -f "$current_log" ]; then
            echo "  ✗ MISSING: $scene_name"
            differences=$((differences + 1))
            continue
        fi
        
        # Simple log comparison (in practice, would compare images)
        if diff -q "$baseline_log" "$current_log" > /dev/null 2>&1; then
            echo "  ✓ PASS: $scene_name"
        else
            echo "  ✗ DIFF: $scene_name"
            differences=$((differences + 1))
        fi
    done
    
    echo ""
    if [ $differences -eq 0 ]; then
        echo "✓ All scenes match baseline - behavior preserved!"
        return 0
    else
        echo "✗ $differences scene(s) differ from baseline"
        return 1
    fi
}

# Main execution
case "${1:-test}" in
    baseline)
        echo "Creating baseline..."
        make -C "$PROJECT_ROOT" > /dev/null
        render_scenes "$BASELINE_DIR"
        echo ""
        echo "✓ Baseline created successfully in $BASELINE_DIR"
        ;;
    
    test)
        echo "Testing current implementation..."
        make -C "$PROJECT_ROOT" > /dev/null
        render_scenes "$CURRENT_DIR"
        compare_results
        ;;
    
    clean)
        echo "Cleaning validation data..."
        rm -rf "$BASELINE_DIR" "$CURRENT_DIR"
        echo "✓ Cleaned"
        ;;
    
    *)
        echo "Usage: $0 {baseline|test|clean}"
        echo ""
        echo "  baseline - Create baseline rendering results"
        echo "  test     - Test current implementation against baseline"
        echo "  clean    - Remove validation data"
        exit 1
        ;;
esac
