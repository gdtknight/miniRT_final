# CI/CD Pipeline Quickstart Guide

**Feature**: CI/CD Pipeline 구축 및 GitHub 워크플로우 개선
**Audience**: miniRT development team
**Last Updated**: 2025-12-15

---

## Table of Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Setup Instructions](#setup-instructions)
4. [Daily Workflow](#daily-workflow)
5. [Troubleshooting](#troubleshooting)
6. [FAQs](#faqs)

---

## Overview

This guide helps you set up and use the CI/CD pipeline for the miniRT project. The pipeline automatically:
- ✅ Checks code style with norminette
- ✅ Builds your code
- ✅ Runs tests
- ✅ Validates commit messages
- ✅ Creates releases

**Time to complete**: ~15 minutes

---

## Prerequisites

### Local Development Environment

```bash
# Ubuntu/Debian
sudo apt-get install -y gcc make git libx11-dev libxext-dev

# Install norminette
pip3 install norminette

# Verify installations
gcc --version        # Should show gcc version
make --version       # Should show GNU Make version
norminette --version # Should show norminette version
```

### GitHub Repository Access

- Write access to repository (to push branches and create PRs)
- GitHub account set up with SSH key or HTTPS authentication

---

## Setup Instructions

### Step 1: Clone Repository

```bash
# Clone the repository
git clone git@github.com:{owner}/miniRT_final.git
cd miniRT_final

# Verify MinilibX is present
ls -la lib/minilibx-linux/
```

### Step 2: Enable GitHub Actions

**If repository is private**:
1. Go to repository Settings
2. Click "Actions" → "General"
3. Enable "Allow all actions and reusable workflows"
4. Click "Save"

**If repository is public**:
GitHub Actions are enabled by default.

### Step 3: Configure Branch Protection (Maintainers Only)

1. Go to Settings → Branches
2. Click "Add rule" for `main` branch
3. Enable:
   - ✅ Require pull request reviews before merging (1 reviewer)
   - ✅ Require status checks to pass before merging
   - ✅ Require branches to be up to date before merging
   - ✅ Include administrators
4. Select required status checks:
   - ✅ `norminette`
   - ✅ `build`
   - ✅ `test`
   - ✅ `validate-commits`
   - ✅ `validate-pr-template`
5. Click "Create" / "Save changes"

### Step 4: Create GitHub Labels

Run this script to create standard labels:

```bash
# Create labels for issues and PRs
gh label create "bug" --color "d73a4a" --description "Something isn't working"
gh label create "enhancement" --color "a2eeef" --description "New feature or request"
gh label create "documentation" --color "0075ca" --description "Improvements to documentation"
gh label create "needs-careful-review" --color "fbca04" --description "Changes to sensitive files"
gh label create "good-first-issue" --color "7057ff" --description "Good for newcomers"

# Or create manually via GitHub UI: Issues → Labels → New label
```

### Step 5: Test Local Build

```bash
# Build the project
make

# Run norminette
norminette src/ includes/

# Run tests (if available)
make test

# Clean up
make fclean
```

If all commands succeed, you're ready to use the CI/CD pipeline!

---

## Daily Workflow

### Starting New Work

**1. Create an Issue**

Go to GitHub Issues → New issue → Select template (Bug Report / Feature Request)

Fill in the template:
```markdown
## 버그 설명 / Bug Description
Shadow rays incorrectly intersect with cylinder caps

## 재현 단계 / Steps to Reproduce
1. Create scene with cylinder and light source
2. Render scene
3. Observe incorrect shadows

## 예상 동작 / Expected Behavior
Shadows should only appear where light is blocked

## 실제 동작 / Actual Behavior
Shadows appear on unobstructed surfaces
```

Click "Submit new issue" → Note the issue number (e.g., #42)

**2. Create Feature Branch**

```bash
# Update main branch
git checkout main
git pull origin main

# Create feature branch (use issue number)
git checkout -b 042-fix-cylinder-shadow

# Verify branch name
git branch --show-current
```

### Making Changes

**3. Write Code**

```bash
# Edit files
vim src/ray/intersections.c

# Check norminette as you go
norminette src/ray/intersections.c

# Build and test
make
./miniRT scenes/test.rt
```

**4. Commit Changes**

Follow Conventional Commits format:

```bash
git add src/ray/intersections.c
git commit -m "fix(ray): correct cylinder cap shadow intersection

Added epsilon offset to ray origin when casting shadow rays
from cylinder cap surfaces to prevent self-intersection.

Fixes: #42"
```

**Commit message template**:
```
<type>(<scope>): <subject>

<body>

Refs: #<issue-number>
```

**Valid types**: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

### Creating Pull Request

**5. Push Branch**

```bash
# Push to GitHub
git push -u origin 042-fix-cylinder-shadow
```

**6. Open Pull Request**

GitHub will show a banner "Compare & pull request" → Click it

Or: Go to Pull requests → New pull request → Select your branch

**7. Fill PR Template**

The template is auto-populated. Fill in the sections:

```markdown
## 변경사항 / Changes
- Fixed cylinder cap shadow ray self-intersection
- Added epsilon offset to ray origin
- Updated shadow calculation in trace.c

## 관련 이슈 / Related Issues
Fixes: #42

## 체크리스트 / Checklist
- [x] 코드가 norminette를 통과합니다 / Code passes norminette
- [x] 컴파일 경고가 없습니다 / No compilation warnings
- [x] 모든 테스트가 통과합니다 / All tests pass
- [x] 새로운 기능에 대한 테스트를 추가했습니다 / Added tests for new features
- [x] 문서를 업데이트했습니다 / Updated documentation
- [x] 메모리 누수가 없습니다 / No memory leaks (verified with valgrind)

## 테스트 방법 / How to Test
```bash
make
./miniRT scenes/cylinder_shadow.rt
# Verify shadows appear correctly
```
```

Click "Create pull request"

### CI Pipeline Execution

**8. Monitor CI Checks**

GitHub will automatically run:
- Norminette check (~30 seconds)
- Build check (~2 minutes)
- Test check (~3 minutes)
- Commit validation (~10 seconds)
- PR template validation (~5 seconds)

Watch the status checks at the bottom of the PR:
- 🟡 Yellow dot = Running
- ✅ Green checkmark = Passed
- ❌ Red X = Failed

**9. Fix Failures (if any)**

Click on failed check → "Details" → View logs

Common failures and fixes:

**Norminette failure**:
```bash
# Check locally
norminette src/ray/intersections.c

# Fix violations
# Commit and push
git add src/ray/intersections.c
git commit -m "style(ray): fix norminette violations

Refs: #42"
git push
```

**Build failure**:
```bash
# Check locally
make

# Fix compilation errors
# Commit and push
git add [fixed files]
git commit -m "fix: resolve compilation errors

Refs: #42"
git push
```

**Commit format failure**:
```bash
# Amend last commit message
git commit --amend

# Edit message to follow format:
# type(scope): subject
#
# body
#
# Refs: #42

# Force push (rewrite history)
git push --force
```

### Code Review and Merge

**10. Request Review**

On the PR page, click "Reviewers" → Select teammate

Reviewer will:
- Read the code changes
- Verify tests pass
- Check that changes address the issue
- Leave comments or approve

**11. Address Review Comments**

```bash
# Make requested changes
vim src/ray/intersections.c

# Commit changes
git add src/ray/intersections.c
git commit -m "refactor(ray): improve variable naming

Renamed variables per review feedback.

Refs: #42"

# Push (CI runs again automatically)
git push
```

**12. Merge PR**

Once approved and all checks pass:
- Click "Squash and merge" (recommended)
- Or "Rebase and merge" (if commit history should be preserved)
- Confirm merge

GitHub automatically:
- Closes the related issue (#42)
- Deletes the feature branch (optional setting)
- Updates main branch

**13. Clean Up Locally**

```bash
# Switch to main branch
git checkout main

# Pull merged changes
git pull origin main

# Delete local feature branch
git branch -d 042-fix-cylinder-shadow
```

---

## Creating a Release

### When to Release

- Major milestone completed (e.g., all mandatory features done)
- Bug fix for critical issue
- Before evaluation or submission

### Release Process

**1. Prepare Release**

```bash
# Ensure main is up to date
git checkout main
git pull origin main

# Verify everything works
make test
norminette src/ includes/

# Update VERSION file (if exists)
echo "1.0.0" > VERSION
git add VERSION
git commit -m "chore: bump version to 1.0.0"
git push origin main
```

**2. Create Tag**

```bash
# Create annotated tag
git tag -a v1.0.0 -m "Release v1.0.0 - Initial release"

# Push tag (triggers release workflow)
git push origin v1.0.0
```

**3. Monitor Release Workflow**

Go to Actions tab → Release workflow

Workflow will:
1. Build clean binary
2. Generate release notes from commits
3. Create GitHub release
4. Upload miniRT binary

**4. Verify Release**

Go to Releases → Latest release

Download and test binary:
```bash
wget https://github.com/{owner}/miniRT_final/releases/download/v1.0.0/miniRT
chmod +x miniRT
./miniRT scenes/example.rt
```

---

## Troubleshooting

### Problem: Norminette fails in CI but passes locally

**Cause**: Different norminette versions

**Solution**:
```bash
# Check local version
norminette --version

# Update to match CI (3.3.50)
pip3 install --upgrade norminette==3.3.50

# Re-check
norminette src/ includes/
```

### Problem: Build fails with "cannot find -lmlx"

**Cause**: MinilibX not built

**Solution**:
```bash
# Build MinilibX first
cd lib/minilibx-linux
make
cd ../..

# Then build miniRT
make
```

### Problem: Commit validation fails

**Cause**: Commit message doesn't follow Conventional Commits

**Solution**:
```bash
# Amend last commit
git commit --amend

# Change message to:
# fix(ray): correct intersection bug
#
# Detailed description of fix.
#
# Fixes: #42

# Force push to update PR
git push --force
```

### Problem: PR template validation fails

**Cause**: Required sections missing from PR description

**Solution**:
1. Edit PR description on GitHub
2. Add missing sections:
   - ## 변경사항 / Changes
   - ## 관련 이슈 / Related Issues
   - ## 체크리스트 / Checklist
   - ## 테스트 방법 / How to Test
3. Ensure "Closes #42" or "Fixes #42" is present
4. Save changes (CI re-runs automatically)

### Problem: Tests fail in CI but pass locally

**Cause**: Different environment or timing issues

**Solution**:
```bash
# Run tests with Xvfb (simulates CI environment)
Xvfb :99 -screen 0 1024x768x24 &
export DISPLAY=:99
./miniRT scenes/test.rt

# Check for race conditions or timing issues
# Add timeouts or retries if needed
```

### Problem: GitHub Actions quota exceeded

**Symptom**: "Usage limit exceeded" error

**Solution**:
- **Free tier**: 2,000 minutes/month for private repos
- **Check usage**: Settings → Billing → Actions minutes
- **Optimize workflows**:
  - Cache MinilibX: Saves ~1 minute per run
  - Run tests only on PR, not every push
  - Reduce test suite if too large

### Problem: Can't merge PR - "Branch is out of date"

**Cause**: Main branch updated since PR created

**Solution**:
```bash
# Update your branch with latest main
git checkout 042-fix-cylinder-shadow
git fetch origin
git rebase origin/main

# Resolve conflicts if any
# Then push
git push --force-with-lease
```

---

## FAQs

### Q: Do I need to run norminette locally?

**A**: Yes! Running norminette locally before pushing saves time. CI will catch issues, but fixing them locally is faster.

```bash
# Run before committing
norminette src/ includes/
```

### Q: Can I push directly to main?

**A**: No, if branch protection is enabled. All changes must go through PRs. This ensures code review and CI checks.

### Q: What if I forget to reference an issue in my commit?

**A**: Amend the commit message:

```bash
git commit --amend
# Add "Refs: #42" to commit message
git push --force
```

Or create a new commit with the reference:

```bash
git commit --allow-empty -m "chore: link to issue #42

Refs: #42"
git push
```

### Q: Can I skip CI checks for a quick fix?

**A**: No, CI checks are mandatory. This ensures quality and prevents broken builds. Emergency hotfixes should still follow the process but can be fast-tracked through review.

### Q: How do I test my changes before creating a PR?

**A**:
```bash
# Build and test locally
make
./miniRT scenes/test.rt

# Run norminette
norminette src/ includes/

# Check for memory leaks
valgrind --leak-check=full ./miniRT scenes/test.rt

# If all pass, create PR
```

### Q: What's the difference between Closes, Fixes, and Refs?

**A**:
- `Closes #42`: Closes issue when PR merges (use for completing features)
- `Fixes #42`: Fixes bug when PR merges (use for bug fixes)
- `Refs #42`: References issue without closing (use for partial work)

### Q: How long should CI take?

**A**: Typical times:
- Norminette: 20-30 seconds
- Build: 1-2 minutes
- Tests: 2-3 minutes
- **Total**: 4-5 minutes

If longer, check for:
- Large test suite
- Slow integration tests
- Missing caching

### Q: Can I customize the CI workflow?

**A**: Yes, edit `.github/workflows/ci.yml`. But discuss with team first to ensure consensus. Changes to workflows affect everyone.

### Q: What if MinilibX build fails in CI?

**A**: Rare, but possible if MinilibX updated. Solutions:
1. Pin MinilibX to specific commit
2. Cache compiled MinilibX library
3. Report issue to MinilibX maintainers

### Q: Do I need to create a release for every change?

**A**: No. Create releases for:
- Major milestones
- Before evaluation
- Stable versions for distribution

Not needed for every PR merge.

---

## Best Practices

### ✅ Do's

- ✅ Run norminette locally before pushing
- ✅ Write descriptive commit messages
- ✅ Reference issues in all commits
- ✅ Keep PRs small and focused (< 300 lines changed)
- ✅ Test your changes thoroughly before PR
- ✅ Respond to review comments promptly
- ✅ Update documentation with code changes

### ❌ Don'ts

- ❌ Don't push directly to main
- ❌ Don't commit without testing locally
- ❌ Don't ignore CI failures ("fix later")
- ❌ Don't force push to shared branches (except your feature branch)
- ❌ Don't skip PR template sections
- ❌ Don't merge your own PRs without review
- ❌ Don't create releases from unstable code

---

## Quick Reference

### Commit Message Template

```
<type>(<scope>): <subject>

<body>

Refs: #<issue-number>
```

### Branch Naming

```
###-descriptive-name
```

Examples: `001-parser-validation`, `042-fix-shadows`

### PR Workflow

```
Issue → Branch → Commits → Push → PR → Review → Merge
```

### Release Workflow

```
Update main → Create tag → Push tag → Auto-release
```

### Getting Help

- **CI issues**: Check workflow logs in Actions tab
- **Git issues**: `git status`, `git log`
- **Build issues**: `make clean && make`
- **Team help**: Ask in team chat or create issue

---

## Next Steps

After completing this guide, you should:
1. ✅ Have CI/CD pipeline fully set up
2. ✅ Know how to create branches, commits, and PRs
3. ✅ Understand how CI checks work
4. ✅ Be able to create releases

**Start developing**:
```bash
# Create your first issue
# Create feature branch
# Make changes
# Push and create PR
# Watch CI work its magic! 🎉
```

For implementation details, see:
- [research.md](./research.md) - Technical decisions and alternatives
- [data-model.md](./data-model.md) - Entity specifications
- [contracts/](./contracts/) - Workflow specifications

Happy coding! 🚀
