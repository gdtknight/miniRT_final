#!/bin/bash
# validate_norminette.sh - Incremental norminette validation script

echo "=== Norminette Validation ===" 
echo "Date: $(date)"
echo ""

# Run norminette
norminette src/ includes/ 2>&1 | tee norminette_current.txt

# Count errors
error_count=$(grep "Error:" norminette_current.txt | wc -l | tr -d ' ')

echo ""
echo "=== Summary ==="
echo "Total errors: $error_count"
echo ""

# Error breakdown
echo "Error breakdown:"
grep "Error:" norminette_current.txt | cut -d: -f2 | sed 's/^ *//' | sort | uniq -c | sort -rn

echo ""
if [ "$error_count" -eq 0 ]; then
    echo "✅ SUCCESS: Zero norminette errors!"
    exit 0
else
    echo "⚠️  In progress: $error_count errors remaining"
    exit 1
fi
