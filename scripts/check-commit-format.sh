#!/bin/bash
# check-commit-format.sh
# Validates commit messages follow Conventional Commits format

set -e

COMMIT_MSG_FILE=$1
COMMIT_MSG=$(cat "$COMMIT_MSG_FILE")

# Conventional commit pattern
PATTERN="^(feat|fix|docs|style|refactor|test|chore|perf|ci|build|revert)(\(.+\))?: .+"

if ! echo "$COMMIT_MSG" | grep -qE "$PATTERN"; then
    echo "❌ Invalid commit message format"
    echo ""
    echo "Current message:"
    echo "  $COMMIT_MSG"
    echo ""
    echo "Expected format:"
    echo "  type(scope): description"
    echo ""
    echo "Examples:"
    echo "  feat: add soft shadow rendering"
    echo "  feat(lighting): implement distance attenuation"
    echo "  fix: resolve segfault in vector operations"
    echo "  fix(parser): handle empty scene files"
    echo "  docs: update README with build instructions"
    echo ""
    echo "Valid types: feat, fix, docs, style, refactor, test, chore, perf, ci, build, revert"
    exit 1
fi

echo "✓ Commit message format is valid"
