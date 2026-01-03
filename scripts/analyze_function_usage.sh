#!/bin/bash
# Function usage analysis script
# Created as part of 012-code-cleanup-refactor feature
# Analyzes function declarations and their usage across the codebase

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
OUTPUT_FILE="${1:-/tmp/function_usage_report.txt}"

echo "=== MiniRT Function Usage Analysis ===" | tee "$OUTPUT_FILE"
echo "Project Root: $PROJECT_ROOT" | tee -a "$OUTPUT_FILE"
echo "Generated: $(date)" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"

# Extract all function declarations from headers
echo "=== PART 1: Function Declarations in Headers ===" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"

for header in "$PROJECT_ROOT"/includes/*.h; do
    header_name=$(basename "$header")
    echo "--- $header_name ---" | tee -a "$OUTPUT_FILE"
    
    # Extract function declarations (simplified pattern)
    # Looks for: return_type function_name(params);
    grep -E '^[a-z_][a-z0-9_]*\s+[a-z_][a-z0-9_]*\s*\(' "$header" | \
        sed 's/^\([a-z_][a-z0-9_]*\)\s\+\([a-z_][a-z0-9_]*\)\s*(.*/\2/' | \
        sort -u | tee -a "$OUTPUT_FILE" || echo "  (no declarations found)" | tee -a "$OUTPUT_FILE"
    
    echo "" | tee -a "$OUTPUT_FILE"
done

# Extract all function definitions from source files
echo "=== PART 2: Function Definitions in Source Files ===" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"

for src_dir in "$PROJECT_ROOT"/src/*/; do
    dir_name=$(basename "$src_dir")
    echo "--- $dir_name/ ---" | tee -a "$OUTPUT_FILE"
    
    # Find all .c files in directory
    find "$src_dir" -name "*.c" -exec basename {} \; | while read -r src_file; do
        echo "  File: $src_file" | tee -a "$OUTPUT_FILE"
        
        # Extract function definitions (simplified)
        grep -E '^[a-z_][a-z0-9_]*\s+[a-z_][a-z0-9_]*\s*\(' "$src_dir$src_file" | \
            sed 's/^\([a-z_][a-z0-9_]*\)\s\+\([a-z_][a-z0-9_]*\)\s*(.*/    \2/' | \
            tee -a "$OUTPUT_FILE" || echo "    (no definitions found)" | tee -a "$OUTPUT_FILE"
    done
    
    echo "" | tee -a "$OUTPUT_FILE"
done

# Analyze function calls
echo "=== PART 3: Function Call Analysis ===" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"
echo "Searching for function calls in codebase..." | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"

# Create temporary file with all function names
temp_functions=$(mktemp)

# Extract function names from headers
for header in "$PROJECT_ROOT"/includes/*.h; do
    grep -E '^[a-z_][a-z0-9_]*\s+[a-z_][a-z0-9_]*\s*\(' "$header" | \
        sed 's/^\([a-z_][a-z0-9_]*\)\s\+\([a-z_][a-z0-9_]*\)\s*(.*/\2/' >> "$temp_functions" || true
done

# Sort and unique
sort -u "$temp_functions" > "${temp_functions}.sorted"
mv "${temp_functions}.sorted" "$temp_functions"

# For each function, count usages
echo "Function Name | Declaration Count | Call Sites" | tee -a "$OUTPUT_FILE"
echo "--------------|-------------------|------------" | tee -a "$OUTPUT_FILE"

while IFS= read -r func_name; do
    [ -z "$func_name" ] && continue
    
    # Count declarations (should be 1 ideally)
    decl_count=$(grep -r "^[a-z_][a-z0-9_]*\s\+${func_name}\s*(" "$PROJECT_ROOT"/includes/ 2>/dev/null | wc -l | tr -d ' ')
    
    # Count call sites (usage in .c files)
    call_count=$(grep -r "${func_name}(" "$PROJECT_ROOT"/src/ "$PROJECT_ROOT"/main.c 2>/dev/null | wc -l | tr -d ' ')
    
    echo "$func_name | $decl_count | $call_count" | tee -a "$OUTPUT_FILE"
done < "$temp_functions"

# Cleanup
rm -f "$temp_functions"

echo "" | tee -a "$OUTPUT_FILE"
echo "=== PART 4: Potentially Unused Functions ===" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"
echo "Functions with 0 call sites (may be callbacks or unused):" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"

# Re-extract and check for zero usage
for header in "$PROJECT_ROOT"/includes/*.h; do
    grep -E '^[a-z_][a-z0-9_]*\s+[a-z_][a-z0-9_]*\s*\(' "$header" | \
        sed 's/^\([a-z_][a-z0-9_]*\)\s\+\([a-z_][a-z0-9_]*\)\s*(.*/\2/' | \
        sort -u | while IFS= read -r func_name; do
            [ -z "$func_name" ] && continue
            
            call_count=$(grep -r "${func_name}(" "$PROJECT_ROOT"/src/ "$PROJECT_ROOT"/main.c 2>/dev/null | wc -l | tr -d ' ')
            
            if [ "$call_count" -eq 0 ]; then
                header_name=$(basename "$header")
                echo "  - $func_name (declared in $header_name)" | tee -a "$OUTPUT_FILE"
            fi
        done
done

echo "" | tee -a "$OUTPUT_FILE"
echo "=== Analysis Complete ===" | tee -a "$OUTPUT_FILE"
echo "Report saved to: $OUTPUT_FILE" | tee -a "$OUTPUT_FILE"
