# Research: CI/CD Pipeline for 42 School C Projects

**Feature**: CI/CD Pipeline 구축 및 GitHub 워크플로우 개선
**Date**: 2025-12-15
**Status**: Complete

## Executive Summary

This research document consolidates findings on implementing CI/CD pipelines specifically tailored for 42 school projects, with emphasis on norminette integration, C compilation workflows, and GitHub Actions best practices.

---

## Research Task 1: GitHub Actions for C Projects

### Decision
Use GitHub Actions with ubuntu-latest runners for all CI/CD workflows, including build, test, and norminette validation.

### Rationale
1. **Native GitHub Integration**: Zero configuration for repository access, no external service authentication needed
2. **Free Tier Generosity**: 2,000 minutes/month for private repos, 3,000 for public repos sufficient for this project
3. **C Toolchain Availability**: Ubuntu runners pre-installed with gcc, make, and common build tools
4. **Custom Tool Installation**: Simple `apt-get` commands for norminette and MinilibX dependencies
5. **Workflow Reusability**: YAML workflows can be shared across similar projects

### Alternatives Considered
- **Travis CI**: Declining market share, inferior free tier (10,000 credits ≈ 1,000 minutes)
- **CircleCI**: Complex configuration, overkill for C projects
- **GitLab CI**: Would require repository migration
- **Jenkins**: Self-hosted overhead inappropriate for student project

### Implementation Details
```yaml
# Minimal workflow structure
name: CI
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install dependencies
        run: sudo apt-get install -y libx11-dev libxext-dev
      - name: Build project
        run: make
```

### References
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [GitHub Actions for C/C++ Projects](https://github.com/actions/starter-workflows/blob/main/ci/cmake.yml)

---

## Research Task 2: Norminette Integration in CI/CD

### Decision
Install norminette via pip3 in GitHub Actions workflow, run with recursive file search to fail CI on violations.

### Rationale
1. **Zero Tolerance Policy**: 42 school requires perfect norminette compliance; CI must enforce this
2. **Early Detection**: Catch style violations before code review, saving reviewer time
3. **Automated Enforcement**: No manual checking needed, consistent across all contributors
4. **Fast Execution**: Norminette typically runs in < 30 seconds for projects of this size
5. **Clear Feedback**: norminette output shows exact line/column of violations

### Alternatives Considered
- **Pre-commit Hooks**: Local only, easily bypassed with `--no-verify`
- **Manual Checks**: Error-prone, inconsistent, wastes reviewer time
- **Linting in IDE**: Requires all devs to use same IDE/plugin configuration
- **Custom Style Checker**: Reinventing the wheel, won't match evaluator's norminette

### Implementation Details
```yaml
- name: Install norminette
  run: pip3 install norminette

- name: Run norminette
  run: |
    norminette src/ includes/ || exit 1
    echo "✅ Norminette check passed"
```

### Edge Cases Handled
- **Ignore MinilibX**: Don't run norminette on `lib/minilibx-linux/` (external dependency)
- **Exit on First Error**: Use `|| exit 1` to fail pipeline immediately
- **Directory Pattern**: Check only `src/` and `includes/` directories

### References
- [norminette GitHub Repository](https://github.com/42School/norminette)
- [42 Norminette Documentation](https://github.com/42School/norminette/blob/master/README.md)

---

## Research Task 3: Conventional Commits for 42 Projects

### Decision
Adopt Conventional Commits specification with mandatory issue number references in commit body or footer.

### Rationale
1. **Automated Changelog**: Structured commits enable automatic CHANGELOG.md generation
2. **Semantic Versioning**: Commit types (feat/fix/breaking) drive version bumps
3. **Clear History**: Git log becomes self-documenting with consistent format
4. **Tooling Support**: Many tools (semantic-release, conventional-changelog) work with this format
5. **Industry Standard**: Widely adopted (Angular, Vue, React, Node.js use this)

### Alternatives Considered
- **Free-form Commits**: No structure, impossible to automate or parse
- **GitHub Issue Keywords**: Only works in PR titles, not individual commits
- **Custom Format**: Would require custom tooling, no ecosystem support
- **Gitmoji**: Emojis add noise, not searchable, non-standard

### Implementation Details

**Format**:
```
<type>(<scope>): <subject>

<body>

<footer>
```

**Required Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation only
- `style`: Formatting, no code change (e.g., norminette fixes)
- `refactor`: Code restructuring, no behavior change
- `test`: Add/modify tests
- `chore`: Maintenance (e.g., build config)

**Examples**:
```
feat(parser): add cylinder parsing support

Implements parsing for cy element in .rt files
with diameter, height, and axis validation.

Refs: #12
```

```
fix(render): correct shadow ray intersection

Shadow rays were incorrectly checking self-intersection.
Added epsilon offset to ray origin.

Fixes: #45
```

**Validation Script**:
```bash
#!/bin/bash
# scripts/check-commit-format.sh
commit_msg=$(git log -1 --pretty=%B)
if ! echo "$commit_msg" | grep -qE '^(feat|fix|docs|style|refactor|test|chore)(\(.+\))?: .+'; then
  echo "❌ Commit message does not follow Conventional Commits format"
  exit 1
fi
if ! echo "$commit_msg" | grep -qE '(Refs|Fixes|Closes): #[0-9]+'; then
  echo "❌ Commit must reference an issue number"
  exit 1
fi
```

### References
- [Conventional Commits Specification](https://www.conventionalcommits.org/)
- [Angular Commit Guidelines](https://github.com/angular/angular/blob/main/CONTRIBUTING.md#commit)

---

## Research Task 4: Branch Strategy for Team Development

### Decision
Adopt GitHub Flow variant with protected main branch and feature branches using pattern `###-feature-name`.

### Rationale
1. **Simplicity**: Only main + feature branches, no complex gitflow hierarchy
2. **CI Integration**: Every PR triggers full CI suite before merge
3. **Protected Main**: Require PR reviews + passing CI to merge to main
4. **Clear Naming**: Issue number prefix enables traceability
5. **Single Source of Truth**: main branch always production-ready

### Alternatives Considered
- **Git Flow**: Too complex (develop/release/hotfix branches) for 2-3 person team
- **Trunk-Based**: Feature flags required, inappropriate for school project
- **GitLab Flow**: Environment branches unnecessary for non-deployed app
- **No Strategy**: Chaos, merge conflicts, broken main branch

### Implementation Details

**Branch Types**:
```
main                      # Protected, always stable
001-parser-validation     # Feature branch
002-cylinder-rendering    # Feature branch
hotfix-segfault-sphere    # Emergency fix (if main broken)
```

**Branch Protection Rules** (configured in GitHub):
- ✅ Require pull request reviews before merging (1 reviewer)
- ✅ Require status checks to pass (CI workflow)
- ✅ Require branches to be up to date before merging
- ✅ Include administrators (no bypass for anyone)
- ✅ Require linear history (no merge commits, rebase preferred)

**Workflow**:
1. Create issue → assigned issue number (e.g., #15)
2. Create branch: `git checkout -b 015-feature-name`
3. Make commits following Conventional Commits
4. Push branch: `git push -u origin 015-feature-name`
5. Create PR: GitHub Actions runs CI
6. Request review: Reviewer checks code + verifies tests pass
7. Merge PR: Use "Squash and merge" for clean history

### Merge Strategies
- **Squash and Merge**: Default choice, keeps main history clean
- **Rebase and Merge**: Use for multi-commit features that need history preserved
- **Merge Commit**: Avoid (creates unnecessary merge commits)

### References
- [GitHub Flow](https://guides.github.com/introduction/flow/)
- [Branch Protection Rules](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-protected-branches/about-protected-branches)

---

## Research Task 5: Automated Release Management

### Decision
Use GitHub Actions to automatically create releases when version tags are pushed, including compiled binary and release notes.

### Rationale
1. **Manual Releases Error-Prone**: Forgetting to attach binary, inconsistent notes
2. **Tag-Driven**: Tags are immutable, permanent markers in git history
3. **Artifact Storage**: GitHub Releases provide permanent download URLs
4. **Release Notes**: Auto-generated from commit history using Conventional Commits
5. **Evaluator Convenience**: Evaluators can download specific version without building

### Alternatives Considered
- **Manual Releases**: Time-consuming, inconsistent, error-prone
- **External Hosting**: S3/hosting costs, requires separate auth
- **Git Tags Only**: No downloadable assets, evaluators must build
- **Release Branches**: Overhead for versioning, tags sufficient

### Implementation Details

**Tagging Convention**:
```bash
# Semantic versioning: MAJOR.MINOR.PATCH
git tag v1.0.0
git push origin v1.0.0
```

**Workflow Trigger**:
```yaml
on:
  push:
    tags:
      - 'v*.*.*'
```

**Release Creation**:
```yaml
- name: Build release binary
  run: make clean && make

- name: Create Release
  uses: softprops/action-gh-release@v1
  with:
    files: miniRT
    body: |
      ## Changes in this release
      [Auto-generated from commits since last tag]
      
      ## Installation
      ```bash
      chmod +x miniRT
      ./miniRT scenes/example.rt
      ```
```

### Version Numbering Strategy
- **MAJOR**: Breaking changes (new mandatory .rt format)
- **MINOR**: New features (new object types, bonus features)
- **PATCH**: Bug fixes only (incorrect intersections, memory leaks)

### References
- [Semantic Versioning](https://semver.org/)
- [GitHub Releases](https://docs.github.com/en/repositories/releasing-projects-on-github/about-releases)
- [action-gh-release](https://github.com/softprops/action-gh-release)

---

## Research Task 6: Issue and PR Template Best Practices

### Decision
Create structured templates using GitHub's ISSUE_TEMPLATE and PULL_REQUEST_TEMPLATE features with mandatory fields.

### Rationale
1. **Consistent Information**: Every bug report includes reproduction steps
2. **Faster Triage**: Templates ensure all necessary info provided upfront
3. **Better Collaboration**: Clear expectations for what to include
4. **Reduced Back-and-Forth**: Fewer "please provide more details" comments
5. **Searchable**: Structured issues easier to search and categorize

### Alternatives Considered
- **No Templates**: Inconsistent reports, missing critical info
- **Wiki Instructions**: Often ignored, not enforced
- **Single Generic Template**: Doesn't adapt to issue type
- **Over-Complicated Forms**: Too many required fields discourages reporting

### Implementation Details

**Bug Report Template Structure**:
```markdown
---
name: Bug Report
about: 버그를 제보해주세요 / Report a bug
title: '[BUG] '
labels: bug
assignees: ''
---

## 버그 설명 / Bug Description
<!-- 버그에 대한 명확하고 간결한 설명 -->
A clear description of what the bug is.

## 재현 단계 / Steps to Reproduce
1. 명령어 실행: `./miniRT [file.rt]`
2. 관찰된 동작...

## 예상 동작 / Expected Behavior
<!-- 예상했던 동작 설명 -->

## 실제 동작 / Actual Behavior
<!-- 실제로 발생한 동작 설명 -->

## 환경 / Environment
- OS: [e.g., Ubuntu 22.04]
- Compiler: [e.g., gcc 11.4.0]
- Norminette version: [e.g., 3.3.50]

## 추가 정보 / Additional Context
<!-- 스크린샷, 에러 로그 등 -->
```

**Feature Request Template**:
```markdown
---
name: Feature Request
about: 새로운 기능을 제안해주세요 / Suggest a new feature
title: '[FEATURE] '
labels: enhancement
assignees: ''
---

## 기능 설명 / Feature Description
<!-- 원하는 기능에 대한 설명 -->

## 동기 / Motivation
<!-- 왜 이 기능이 필요한가요? -->
<!-- 어떤 문제를 해결하나요? -->

## 제안된 구현 / Proposed Implementation
<!-- 이 기능을 어떻게 구현할 수 있을지 아이디어가 있다면 -->

## 대안 / Alternatives
<!-- 고려한 다른 대안들 -->
```

**Pull Request Template**:
```markdown
## 변경사항 / Changes
<!-- 이 PR에서 무엇을 변경했나요? -->

## 관련 이슈 / Related Issues
Closes #(issue number)

## 체크리스트 / Checklist
- [ ] 코드가 norminette를 통과합니다 / Code passes norminette
- [ ] 컴파일 경고가 없습니다 / No compilation warnings
- [ ] 모든 테스트가 통과합니다 / All tests pass
- [ ] 새로운 기능에 대한 테스트를 추가했습니다 / Added tests for new features
- [ ] 문서를 업데이트했습니다 / Updated documentation
- [ ] 메모리 누수가 없습니다 / No memory leaks (verified with valgrind)

## 테스트 방법 / How to Test
<!-- 이 변경사항을 어떻게 테스트할 수 있나요? -->
```

### Template Features
- **Bilingual**: Korean for primary communication, English for technical terms
- **Markdown Formatting**: Rich formatting support
- **Front Matter**: Auto-labels, assignees, title prefixes
- **Checkbox Lists**: Clear actionable items for PRs
- **Comment Hints**: `<!-- -->` guides what to write in each section

### References
- [GitHub Issue Templates](https://docs.github.com/en/communities/using-templates-to-encourage-useful-issues-and-pull-requests/configuring-issue-templates-for-your-repository)
- [Pull Request Templates](https://docs.github.com/en/communities/using-templates-to-encourage-useful-issues-and-pull-requests/creating-a-pull-request-template-for-your-repository)

---

## Research Task 7: MinilibX Dependency Management in CI

### Decision
Clone MinilibX during CI build and compile it as part of the workflow, treating it as a submodule-equivalent dependency.

### Rationale
1. **Already Integrated**: Project already has `lib/minilibx-linux/` directory
2. **Build Dependency**: MinilibX must compile before miniRT links against it
3. **X11 Requirements**: Ubuntu runners need `libx11-dev` and `libxext-dev` packages
4. **No Display Server**: Use Xvfb (virtual framebuffer) for tests requiring window creation
5. **Consistent Environment**: CI environment matches local development

### Alternatives Considered
- **Pre-compiled MinilibX**: Would need to maintain binaries for different platforms
- **System Package**: MinilibX not available in apt repositories
- **Skip Tests**: Would miss window/rendering integration issues
- **Docker Container**: Overkill, slower builds, more complex

### Implementation Details

**Install X11 Dependencies**:
```yaml
- name: Install MinilibX dependencies
  run: |
    sudo apt-get update
    sudo apt-get install -y libx11-dev libxext-dev xorg-dev xvfb
```

**Build MinilibX**:
```yaml
- name: Build MinilibX
  run: |
    cd lib/minilibx-linux
    make
```

**Run Tests with Virtual Display**:
```yaml
- name: Run tests
  run: |
    # Start virtual X server
    Xvfb :99 -screen 0 1024x768x24 &
    export DISPLAY=:99
    
    # Run tests that create windows
    ./miniRT scenes/test.rt
```

**Makefile Integration**:
Existing Makefile already handles MinilibX:
```makefile
$(NAME): $(OBJS)
    @make -C $(MLX_DIR)  # Build MinilibX first
    @$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
```

### References
- [MinilibX GitHub](https://github.com/42Paris/minilibx-linux)
- [Xvfb for Headless Testing](https://www.x.org/releases/X11R7.6/doc/man/man1/Xvfb.1.xhtml)

---

## Research Task 8: Test Automation Strategy

### Decision
Create two-tier testing: unit tests for individual functions (math/vector operations, parsing) and integration tests for full scene rendering.

### Rationale
1. **Fast Feedback**: Unit tests run in < 1 second, catch regressions immediately
2. **Comprehensive Coverage**: Integration tests verify end-to-end functionality
3. **Maintainability**: Unit tests easier to debug than full renders
4. **CI Efficiency**: Run unit tests on every commit, integration tests on PR only
5. **Local Development**: Developers can run quick unit tests locally

### Alternatives Considered
- **Manual Testing Only**: Slow, error-prone, doesn't scale
- **Integration Tests Only**: Slow, hard to debug, poor CI performance
- **Visual Regression Testing**: Requires image comparison, too complex for scope
- **Property-Based Testing**: Overkill for deterministic ray tracing math

### Implementation Details

**Unit Test Structure** (existing `/tests` directory):
```
tests/
├── test_vector.c          # Vector math (dot, cross, normalize)
├── test_parser.c          # Scene file parsing
├── test_intersections.c   # Ray-object intersection math
└── test_lighting.c        # Lighting calculations
```

**Unit Test Framework**:
Use simple assert-based tests, no heavy framework needed:
```c
// tests/test_vector.c
#include "../includes/vector.h"
#include <assert.h>
#include <math.h>

void test_vector_normalize(void) {
    t_vec3 v = {3.0, 4.0, 0.0};
    t_vec3 result = vec_normalize(v);
    double magnitude = sqrt(result.x * result.x + 
                           result.y * result.y + 
                           result.z * result.z);
    assert(fabs(magnitude - 1.0) < 0.0001);
}

int main(void) {
    test_vector_normalize();
    // ... more tests
    printf("✅ All vector tests passed\n");
    return 0;
}
```

**Integration Test Structure**:
```
tests/integration/
├── scenes/
│   ├── test_basic.rt      # Simple scene: 1 sphere, 1 light
│   ├── test_shadows.rt    # Shadow validation
│   └── test_cylinders.rt  # Cylinder rendering
└── test_render.sh         # Shell script to run renders
```

**Integration Test Script**:
```bash
#!/bin/bash
# tests/integration/test_render.sh

PASS=0
FAIL=0

for scene in tests/integration/scenes/*.rt; do
    echo "Testing $scene..."
    timeout 30s ./miniRT "$scene" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "✅ PASS: $scene"
        ((PASS++))
    else
        echo "❌ FAIL: $scene"
        ((FAIL++))
    fi
done

echo "Results: $PASS passed, $FAIL failed"
[ $FAIL -eq 0 ] && exit 0 || exit 1
```

**CI Workflow Integration**:
```yaml
- name: Run unit tests
  run: |
    make tests
    ./tests/run_unit_tests.sh

- name: Run integration tests
  run: |
    chmod +x tests/integration/test_render.sh
    tests/integration/test_render.sh
```

### Test Coverage Goals
- **Unit Tests**: 80%+ coverage of math/parsing functions
- **Integration Tests**: 100% coverage of example scene files
- **Performance Tests**: Max 5 seconds per scene in CI

### References
- [Criterion Testing Framework](https://github.com/Snaipe/Criterion) (optional upgrade)
- [Google Test for C](https://github.com/google/googletest) (C++ alternative)

---

## Implementation Priority

Based on research findings, implement in this order:

1. **Phase 1A - Core CI Workflow** (Highest Priority)
   - `.github/workflows/ci.yml` with build + norminette
   - Validates every push/PR
   - Blocks merge on failures

2. **Phase 1B - Issue/PR Templates** (High Priority)
   - Bug report, feature request, documentation templates
   - PR template with checklist
   - Immediate value for team collaboration

3. **Phase 2A - Commit Validation** (Medium Priority)
   - `scripts/check-commit-format.sh`
   - Add to CI workflow
   - Enforces Conventional Commits

4. **Phase 2B - Test Automation** (Medium Priority)
   - Unit test runner in CI
   - Integration test runner
   - Xvfb setup for headless testing

5. **Phase 3 - Release Automation** (Lower Priority)
   - `.github/workflows/release.yml`
   - Triggered on version tags
   - Creates GitHub release with binary

6. **Phase 4 - Branch Protection** (Configuration)
   - Enable branch protection rules
   - Require PR reviews
   - Require status checks

---

## Success Metrics

- ✅ CI pipeline runs in < 5 minutes
- ✅ Zero norminette violations in main branch
- ✅ 100% of PRs use template
- ✅ 100% of commits follow Conventional Commits
- ✅ Zero failed builds in main branch
- ✅ All tests passing before merge
- ✅ Automated releases on tags

---

## Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|-----------|
| GitHub Actions quota exceeded | CI stops working | Monitor usage, optimize workflows, cache dependencies |
| Norminette version mismatch | False positives/negatives | Pin specific norminette version in workflow |
| Flaky integration tests | CI instability | Add retries, increase timeouts, use Xvfb properly |
| MinilibX compilation fails in CI | Blocks all builds | Pre-validate MinilibX build, cache compiled libs |
| Contributors skip commit format | Inconsistent history | Enforce with CI check, reject non-compliant PRs |

---

## Conclusion

Research complete. All technical decisions documented with rationale, alternatives, and implementation details. Ready to proceed to Phase 1: Design & Contracts.
