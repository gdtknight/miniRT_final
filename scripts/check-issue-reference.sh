#!/bin/bash
# check-issue-reference.sh
# Validates that commits reference an issue number when required

set -e

COMMIT_MSG_FILE=$1
COMMIT_MSG=$(cat "$COMMIT_MSG_FILE")

# Skip check for certain commit types that don't require issues
if echo "$COMMIT_MSG" | grep -qE "^(chore|docs|style): "; then
    echo "✓ Commit type does not require issue reference"
    exit 0
fi

# Check for issue reference in commit message
if ! echo "$COMMIT_MSG" | grep -qE "(#[0-9]+|issue [0-9]+|closes [0-9]+|fixes [0-9]+|resolves [0-9]+)"; then
    echo "⚠️  Warning: No issue reference found in commit message"
    echo ""
    echo "Current message:"
    echo "  $COMMIT_MSG"
    echo ""
    echo "Consider adding an issue reference:"
    echo "  feat: add feature (#123)"
    echo "  fix: resolve bug (closes #456)"
    echo "  refactor: improve code (issue #789)"
    echo ""
    echo "This is a warning, not an error. Press Enter to continue or Ctrl+C to abort."
    read -r
fi

echo "✓ Issue reference check passed"
