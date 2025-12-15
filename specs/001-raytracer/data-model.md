# Data Model: CI/CD Pipeline Entities

**Feature**: CI/CD Pipeline 구축 및 GitHub 워크플로우 개선
**Date**: 2025-12-15

## Overview

This document defines the data structures and entities that comprise the CI/CD pipeline system for miniRT. These entities represent workflow configurations, test artifacts, and validation rules.

---

## Entity 1: GitHub Actions Workflow

Represents a CI/CD workflow executed by GitHub Actions.

### Attributes

| Field | Type | Required | Description | Validation |
|-------|------|----------|-------------|------------|
| `name` | string | Yes | Workflow display name | Non-empty, alphanumeric with spaces |
| `on` | object | Yes | Trigger events | Valid GitHub event types |
| `jobs` | map<string, Job> | Yes | Named jobs to execute | At least 1 job |
| `env` | map<string, string> | No | Global environment variables | Key-value pairs |
| `permissions` | object | No | GitHub token permissions | Valid permission scopes |

### Example
```yaml
name: CI Pipeline
on:
  push:
    branches: [main]
  pull_request:
    branches: [main]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - run: make
```

### State Transitions
```
Created → Queued → In Progress → Completed (Success/Failure/Cancelled)
```

### Relationships
- Workflow **contains many** Jobs
- Workflow **triggered by** GitHub Events
- Workflow **produces** Artifacts

---

## Entity 2: Job

Represents a single job within a workflow, running on a specific runner.

### Attributes

| Field | Type | Required | Description | Validation |
|-------|------|----------|-------------|------------|
| `runs-on` | string | Yes | Runner type | Valid runner label (ubuntu-latest, etc.) |
| `steps` | array<Step> | Yes | Sequential steps to execute | At least 1 step |
| `needs` | array<string> | No | Job dependencies | Must reference existing job names |
| `if` | string | No | Conditional execution | Valid GitHub expression |
| `timeout-minutes` | integer | No | Max execution time | 1-360, default 360 |
| `strategy` | object | No | Matrix strategy | Valid matrix configuration |

### Example
```yaml
build:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v3
    - name: Build
      run: make
    - name: Test
      run: make test
```

### Validation Rules
- Job name must be unique within workflow
- If `needs` specified, referenced jobs must exist
- Total job execution time < 6 hours
- Steps execute sequentially; failure stops job unless `continue-on-error: true`

### Relationships
- Job **belongs to** Workflow
- Job **contains many** Steps
- Job **may depend on** other Jobs

---

## Entity 3: Step

Represents a single executable step within a job.

### Attributes

| Field | Type | Required | Description | Validation |
|-------|------|----------|-------------|------------|
| `name` | string | No | Step display name | Human-readable identifier |
| `uses` | string | Conditional | Pre-built action to run | Valid action reference (owner/repo@version) |
| `run` | string | Conditional | Shell command(s) to execute | Non-empty string if `uses` not present |
| `with` | map<string, string> | No | Action input parameters | Required inputs for specified action |
| `env` | map<string, string> | No | Environment variables | Overrides job/workflow env |
| `if` | string | No | Conditional execution | Valid GitHub expression |
| `continue-on-error` | boolean | No | Allow failure | Default false |

### Validation Rules
- Must specify exactly one of `uses` or `run` (mutually exclusive)
- If `uses` specified, `with` may be required per action documentation
- Shell commands in `run` must use valid bash syntax
- Environment variables accessible via `${{ env.VAR_NAME }}`

### Example
```yaml
- name: Install norminette
  run: pip3 install norminette

- name: Checkout code
  uses: actions/checkout@v3
  with:
    fetch-depth: 0
```

### Relationships
- Step **belongs to** Job
- Step **may use** GitHub Action
- Step **produces** Logs and Artifacts

---

## Entity 4: Issue Template

Represents a structured form for creating GitHub issues.

### Attributes

| Field | Type | Required | Description | Validation |
|-------|------|----------|-------------|------------|
| `name` | string | Yes | Template display name | Non-empty, displayed in issue creation UI |
| `about` | string | Yes | Template description | Brief explanation of when to use |
| `title` | string | No | Default issue title | Prefix like "[BUG]" or "[FEATURE]" |
| `labels` | array<string> | No | Auto-applied labels | Must match existing labels in repo |
| `assignees` | array<string> | No | Auto-assigned users | Must be valid GitHub usernames |
| `body` | string | Yes | Template markdown content | Must include sections for required info |

### Example
```markdown
---
name: Bug Report
about: 버그를 제보해주세요
title: '[BUG] '
labels: [bug]
assignees: []
---

## 버그 설명
[Description here]

## 재현 단계
1. Step 1
2. Step 2

## 환경
- OS:
- Compiler:
```

### Validation Rules
- Template file must be named with `.md` extension
- Must be placed in `.github/ISSUE_TEMPLATE/` directory
- Front matter (between `---`) must be valid YAML
- Body must provide clear sections for user input

### Relationships
- Issue Template **creates** GitHub Issue
- Issue Template **applies** Labels automatically
- Issue Template **assigns** Users automatically

---

## Entity 5: Pull Request Template

Represents a structured form for creating pull requests.

### Attributes

| Field | Type | Required | Description | Validation |
|-------|------|----------|-------------|------------|
| `body` | string | Yes | Template markdown content | Must include checklist and description sections |

### Example
```markdown
## 변경사항 / Changes
[Description of changes]

## 관련 이슈 / Related Issues
Closes #123

## 체크리스트 / Checklist
- [ ] Norminette passes
- [ ] Tests pass
- [ ] Documentation updated
```

### Validation Rules
- Must be named `PULL_REQUEST_TEMPLATE.md`
- Must be placed in `.github/` or `.github/PULL_REQUEST_TEMPLATE/`
- Should include checklist with `- [ ]` syntax
- Should reference related issues with `Closes #` or `Fixes #`

### Relationships
- PR Template **creates** Pull Request
- Pull Request **triggers** CI Workflows
- Pull Request **references** Issues

---

## Entity 6: Commit Message

Represents a structured commit message following Conventional Commits.

### Attributes

| Field | Type | Required | Description | Validation |
|-------|------|----------|-------------|------------|
| `type` | string | Yes | Commit type | One of: feat, fix, docs, style, refactor, test, chore |
| `scope` | string | No | Component scope | Alphanumeric, describes affected component |
| `subject` | string | Yes | Short description | Max 72 chars, lowercase, no period |
| `body` | string | No | Detailed description | Multi-line, explains what/why |
| `footer` | string | No | Issue references | Format: "Refs: #123" or "Fixes: #456" |

### Format
```
<type>(<scope>): <subject>

<body>

<footer>
```

### Validation Rules
- Type must match enum: `feat|fix|docs|style|refactor|test|chore`
- Scope optional but recommended: `parser`, `render`, `math`, etc.
- Subject must start with lowercase letter
- Subject must not end with period
- Subject max length: 72 characters
- Footer must reference issue: `Refs: #\d+` or `Fixes: #\d+` or `Closes: #\d+`

### Example
```
feat(parser): add cylinder orientation parsing

Implements parsing of normalized axis vector for cylinder
elements in scene files. Validates vector magnitude is 1.0.

Refs: #42
```

### Relationships
- Commit **references** Issue
- Commit **belongs to** Branch
- Commit **triggers** CI Workflow (when pushed)

---

## Entity 7: Norminette Check Result

Represents the result of a norminette style check.

### Attributes

| Field | Type | Required | Description | Validation |
|-------|------|----------|-------------|------------|
| `file_path` | string | Yes | Path to checked file | Relative path from repo root |
| `status` | string | Yes | Check result | "OK" or "Error" |
| `violations` | array<Violation> | No | List of style violations | Empty if status is "OK" |
| `total_files` | integer | Yes | Number of files checked | Positive integer |
| `passed_files` | integer | Yes | Files with no violations | <= total_files |

### Violation Sub-Entity

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `line` | integer | Yes | Line number with violation |
| `column` | integer | Yes | Column number with violation |
| `code` | string | Yes | Norminette error code |
| `message` | string | Yes | Human-readable error message |

### Example
```json
{
  "file_path": "src/parser/parser.c",
  "status": "Error",
  "violations": [
    {
      "line": 42,
      "column": 80,
      "code": "TOO_MANY_LINES",
      "message": "Function exceeds 25 lines"
    }
  ]
}
```

### Validation Rules
- Status must be "OK" or "Error"
- If status is "Error", violations array must be non-empty
- If status is "OK", violations array must be empty
- passed_files <= total_files

### State Transitions
```
Pending → Running → Completed (OK/Error)
```

### Relationships
- Norminette Check **validates** Source File
- Norminette Check **blocks** Pull Request (if failed)
- Norminette Check **reports to** CI Workflow

---

## Entity 8: Release

Represents a versioned release of the miniRT software.

### Attributes

| Field | Type | Required | Description | Validation |
|-------|------|----------|-------------|------------|
| `tag_name` | string | Yes | Version tag | Semantic version: v{MAJOR}.{MINOR}.{PATCH} |
| `name` | string | Yes | Release name | Human-readable title |
| `body` | string | Yes | Release notes | Markdown-formatted changelog |
| `draft` | boolean | No | Draft status | Default false |
| `prerelease` | boolean | No | Pre-release flag | Default false |
| `assets` | array<Asset> | Yes | Attached binaries | At least miniRT executable |
| `created_at` | timestamp | Yes | Creation timestamp | ISO 8601 format |
| `published_at` | timestamp | No | Publication timestamp | ISO 8601 format |

### Asset Sub-Entity

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `name` | string | Yes | Asset filename |
| `size` | integer | Yes | File size in bytes |
| `download_url` | string | Yes | Public download URL |
| `content_type` | string | Yes | MIME type |

### Example
```yaml
tag_name: v1.2.3
name: miniRT v1.2.3 - Cylinder Caps Fix
body: |
  ## Features
  - feat(render): add specular lighting support
  
  ## Bug Fixes
  - fix(ray): correct cylinder cap intersection
  
  ## Documentation
  - docs: update Korean implementation guide
assets:
  - name: miniRT
    size: 245760
    content_type: application/octet-stream
```

### Validation Rules
- Tag name must match pattern: `v\d+\.\d+\.\d+`
- Tag must not already exist in repository
- Assets must include compiled `miniRT` binary
- Release body should be auto-generated from commit messages

### Relationships
- Release **tagged from** Commit
- Release **includes** Binary Assets
- Release **generated by** Release Workflow

---

## Data Flow Diagrams

### CI Workflow Data Flow
```
Push/PR Event → Workflow Triggered → Jobs Execute → Steps Run → 
Norminette Check → Build → Test → Result (Pass/Fail) → 
PR Status Updated → Merge Decision
```

### Release Workflow Data Flow
```
Version Tag Pushed → Release Workflow Triggered → 
Clean Build → Artifacts Collected → 
Release Created → Assets Uploaded → 
Release Published
```

### Issue/PR Template Flow
```
User Clicks "New Issue" → Template Selection → 
Form Pre-filled → User Fills Remaining Fields → 
Issue Created → Labels/Assignees Applied → 
CI Workflow Triggered (if PR)
```

---

## Validation Summary

All entities enforce validation at multiple levels:

1. **Schema Validation**: YAML syntax, required fields, data types
2. **Business Logic Validation**: Semantic rules (e.g., tag naming, commit format)
3. **Relationship Validation**: Foreign key integrity (job dependencies, issue references)
4. **GitHub API Validation**: Final validation when creating resources via API

CI pipeline enforces these validations automatically, failing builds when violations detected.

---

## Storage

- **Workflow Definitions**: Stored as YAML files in `.github/workflows/`
- **Issue Templates**: Stored as Markdown files in `.github/ISSUE_TEMPLATE/`
- **PR Template**: Stored as `PULL_REQUEST_TEMPLATE.md` in `.github/`
- **Workflow Runs**: Stored in GitHub Actions run history (90-day retention)
- **Releases**: Stored in GitHub Releases (permanent)
- **Artifacts**: Stored in GitHub Actions artifacts (90-day retention unless in release)

---

## Performance Considerations

- **Workflow Caching**: Cache MinilibX build to speed up subsequent runs
- **Parallel Jobs**: Run norminette, build, and test in parallel where possible
- **Incremental Builds**: Use ccache for C compilation caching
- **Test Optimization**: Run fast unit tests before slow integration tests
- **Resource Limits**: Stay within GitHub Actions free tier limits (2000 min/month)

---

## Security Considerations

- **Secret Management**: No secrets required for this project (public repo)
- **Permission Scoping**: Workflows use minimal required permissions
- **Branch Protection**: Prevent direct pushes to main, require PR reviews
- **Dependency Scanning**: Pin action versions (@v3) to prevent supply chain attacks
- **Code Injection**: Avoid using `${{ github.event.issue.title }}` in shell commands
