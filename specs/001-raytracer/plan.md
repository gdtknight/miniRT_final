# Implementation Plan: CI/CD Pipeline 구축 및 GitHub 워크플로우 개선

**Branch**: `001-cicd-pipeline-github-workflow` | **Date**: 2025-12-15 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/001-raytracer/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Establish comprehensive CI/CD pipeline using GitHub Actions for the miniRT project to automate build verification, code style validation, testing, and release management. This includes implementing automated norminette checks, pull request validation, issue/PR templates, and enforcing commit conventions aligned with the 42 school project standards.

## Technical Context

**Language/Version**: C (C99 standard), Makefile, YAML (GitHub Actions), Shell scripts (Bash)
**Primary Dependencies**: 
- GitHub Actions (CI/CD platform)
- norminette 3.3.50+ (42 code style checker)
- cc/gcc (C compiler with -Wall -Wextra -Werror)
- MinilibX (already integrated at lib/minilibx-linux)
- make (build automation)
**Storage**: Git repository, GitHub Actions artifacts for build logs and releases
**Testing**: 
- Build verification (compilation success/failure)
- Norminette compliance checks (zero tolerance for violations)
- Unit tests (existing test framework at /tests directory)
- Integration tests with scene files
**Target Platform**: Linux (Ubuntu/Debian-based systems with X11), GitHub-hosted runners (ubuntu-latest)
**Project Type**: Single C project with Makefile build system
**Performance Goals**: 
- CI pipeline completion < 5 minutes for standard PR checks
- Norminette validation < 30 seconds
- Build time < 2 minutes
**Constraints**: 
- 42 Norminette compliance (NON-NEGOTIABLE)
- No external CI services beyond GitHub Actions
- Free tier GitHub Actions limits (2000 minutes/month for private repos)
- Must support manual workflow triggers for testing
**Scale/Scope**: 
- ~15 C source files, ~1500 lines of code
- 3-5 concurrent branches during active development
- Expected 10-20 PRs per month
- Release cadence: milestone-based (not time-based)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Gate 1: 42 Norminette Compliance (NON-NEGOTIABLE)
**Status**: ✅ PASS
**Check**: CI/CD pipeline enforces norminette checks on all .c and .h files
**Evidence**: Workflow includes `norminette` step that fails pipeline on violations

### Gate 2: Mandatory Unit Testing (NON-NEGOTIABLE)
**Status**: ✅ PASS
**Check**: CI/CD pipeline runs unit tests and blocks merges on failures
**Evidence**: Workflow includes test execution step; existing /tests directory structure preserved

### Gate 3: Build Verification Workflow (NON-NEGOTIABLE)
**Status**: ✅ PASS
**Check**: Every PR triggers automated build-test-verify cycle
**Evidence**: GitHub Actions workflow configured for pull_request events with compilation, execution, and test steps

### Gate 4: Readability-First Code
**Status**: ✅ PASS
**Check**: Templates encourage descriptive commit messages and PR descriptions
**Evidence**: PR template includes sections for change description and testing details

### Gate 5: Bilingual Documentation
**Status**: ✅ PASS
**Check**: Issue templates support both English and Korean
**Evidence**: Templates use Korean for local team, English for code/technical terms

**Overall Status**: ✅ ALL GATES PASSED - Proceeding to Phase 0

## Project Structure

### Documentation (this feature)

```text
specs/001-raytracer/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output - CI/CD best practices research
├── data-model.md        # Phase 1 output - Workflow data structures
├── quickstart.md        # Phase 1 output - CI/CD setup guide
├── contracts/           # Phase 1 output - Workflow YAML schemas
│   ├── ci-main.yml.schema      # Main CI workflow contract
│   ├── release.yml.schema      # Release workflow contract
│   └── pr-validation.yml.schema # PR validation contract
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
# CI/CD and GitHub configurations
.github/
├── workflows/
│   ├── ci.yml                    # Main CI pipeline (build, test, norminette)
│   ├── release.yml               # Release automation (tag → GitHub release)
│   └── pr-validation.yml         # PR-specific checks (conventional commits, issue links)
├── ISSUE_TEMPLATE/
│   ├── bug_report.md             # Bug report template
│   ├── feature_request.md        # Feature request template
│   └── documentation.md          # Documentation improvement template
├── PULL_REQUEST_TEMPLATE.md      # Standard PR template
└── agents/
    └── copilot-instructions.md   # Existing agent instructions (preserved)

# Scripts for CI/CD utilities
scripts/
├── check-commit-format.sh        # Validates conventional commit messages
├── check-issue-reference.sh      # Ensures commits reference issues
└── setup-ci-environment.sh       # CI environment setup helper

# Existing project structure (unchanged)
src/
├── lighting/
├── math/
├── parser/
├── ray/
├── render/
├── utils/
├── window/
└── main.c

includes/
lib/minilibx-linux/
tests/
scenes/
docs/
```

**Structure Decision**: Using standard GitHub repository structure with `.github/` directory for all CI/CD configurations. Existing source code structure remains unchanged. Scripts directory added for reusable CI utilities that can also be run locally.

## Complexity Tracking

**No constitutional violations requiring justification.**

All CI/CD implementations align with constitution principles:
- Enforces norminette compliance automatically (Principle I)
- Templates encourage readable commits and PRs (Principle II)
- Workflow includes mandatory unit testing (Principle III)
- Templates support bilingual documentation (Principle IV)
- Automates build verification workflow (Principle V)
