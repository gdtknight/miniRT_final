#!/bin/bash
# 42 Function Constraint Validation Script
# Checks for prohibited functions in miniRT codebase

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "=== 42 School Function Constraint Validation ==="
echo "Project: miniRT"
echo "Date: $(date)"
echo ""

# Prohibited functions list
PROHIBITED_FUNCS=(
    "pthread_create"
    "pthread_join"
    "pthread_mutex_lock"
    "pthread_mutex_unlock"
    "pthread_mutex_init"
    "pthread_mutex_destroy"
    "fork"
    "pipe"
    "waitpid"
    "execve"
    "system"
)

violations=0

echo "Searching for prohibited functions..."
echo ""

for func in "${PROHIBITED_FUNCS[@]}"; do
    echo -n "  Checking $func... "
    
    # Search in source files and headers
    matches=$(grep -r "${func}(" "$PROJECT_ROOT/src/" "$PROJECT_ROOT/includes/" "$PROJECT_ROOT/main.c" 2>/dev/null | wc -l | tr -d ' ')
    
    if [ "$matches" -gt 0 ]; then
        echo "✗ FOUND ($matches occurrences)"
        violations=$((violations + matches))
        grep -rn "${func}(" "$PROJECT_ROOT/src/" "$PROJECT_ROOT/includes/" "$PROJECT_ROOT/main.c" 2>/dev/null | head -5
    else
        echo "✓ OK"
    fi
done

echo ""
echo "=== Validation Results ==="

if [ $violations -eq 0 ]; then
    echo "✓ PASSED: No prohibited functions found"
    echo "✓ Codebase complies with 42 School function constraints"
    exit 0
else
    echo "✗ FAILED: $violations prohibited function usage(s) found"
    echo "✗ Codebase violates 42 School function constraints"
    exit 1
fi
