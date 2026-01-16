# Quickstart: Wiki Update Workflow

**Feature**: 019-wiki-update  
**Purpose**: Step-by-step guide for executing wiki updates

This guide provides the workflow for updating miniRT wiki to document v2.1.0 features.

---

## Prerequisites

### ✅ Phase 0 Complete
- `research.md` exists with all technical details resolved
- BVH algorithm details documented
- Debounce state machine explained
- Performance metrics gathered
- Korean terminology defined
- Troubleshooting scenarios identified

### ✅ Phase 1 Complete
- `data-model.md` defines content structure
- `contracts/` directory contains:
  - `content-templates.md`: Markdown patterns
  - `korean-terminology.md`: Translation guide
  - `style-guide.md`: Formatting rules

### ✅ Environment Ready
- Git repository: `/home/yoshin/work/miniRT`
- Wiki directory: `/home/yoshin/work/miniRT/wiki/`
- Branch: `019-wiki-update` (checked out)
- CHANGELOG.md available at: `/docs/project/CHANGELOG.md`

---

## Workflow Overview

```
1. Prepare Content     → Gather all information from research/data-model
2. Create New Page     → 버전-히스토리.md (no dependencies)
3. Update Major Pages  → 프로그램-개요.md, 레이-트레이싱-알고리즘.md
4. Update Interactive  → 인터랙티브-컨트롤.md
5. Update FAQ          → FAQ.md with troubleshooting
6. Update Minor Pages  → Home.md, README.md, others
7. Validate            → Links, terminology, formatting
8. Review              → Quality gates, consistency
9. Commit              → Git commit and push
```

**Estimated Time**: 4-6 hours for complete wiki update

---

## Step 1: Prepare Content

### 1.1 Gather Source Materials

**Checklist**:
- [ ] `research.md` (technical details)
- [ ] `data-model.md` (content structure)
- [ ] `contracts/content-templates.md` (markdown patterns)
- [ ] `contracts/korean-terminology.md` (translations)
- [ ] `docs/project/CHANGELOG.md` (v2.1.0 info)

### 1.2 Create Content Preparation Document

**Optional but Recommended**: Create temporary notes file

```bash
cd /home/yoshin/work/miniRT/specs/019-wiki-update
touch content-notes.md  # Work-in-memory alternative
```

**Content to Prepare**:
1. BVH technical description (Korean, 100-150 words)
2. Debounce state machine explanation (Korean, 80-100 words)
3. Preview mode description (Korean, 50-80 words)
4. Performance metrics summary (Korean, 40-60 words)
5. FAQ Q&A entries (6 questions, Korean)

---

## Step 2: Create New Page (버전-히스토리.md)

### Why First?
- No dependencies on other pages
- Provides link target for Home.md update
- Simplest page to create (mostly structure)

### 2.1 Create File

```bash
cd /home/yoshin/work/miniRT/wiki
touch 버전-히스토리.md
```

### 2.2 Use Template

Reference: `contracts/content-templates.md` → Template 5 (Version History Entry)

**Content Structure**:
```markdown
# 버전 히스토리 (Version History)

[Brief introduction paragraph]

---

## [2.1.0] - 2026-01-15
[Use Template 5 with v2.1.0 details from CHANGELOG.md]

---

## [2.0.0] - 2025-01-XX
[Brief summary]

---

[Earlier versions with brief summaries]

---

**Last Updated**: 2026-01  
**Version**: 2.1.0
```

### 2.3 Populate with CHANGELOG Content

**Source**: `/docs/project/CHANGELOG.md` lines 8-40 (v2.1.0 section)

**Key Information to Include**:
- 주요 변경사항 (Major changes)
- 추가됨 (Added features)
- 변경됨 (Changed)
- 수정됨 (Fixed)
- 성능 (Performance)
- Link to full CHANGELOG.md

### 2.4 Validate

- [ ] Version is "2.1.0"
- [ ] Date is "2026-01-15"
- [ ] Link to CHANGELOG.md is correct: `../../docs/project/CHANGELOG.md`
- [ ] Markdown renders correctly
- [ ] Footer version block present

**Estimated Time**: 30-40 minutes

---

## Step 3: Update Major Pages

### 3.1 Update 프로그램-개요.md

**Complexity**: MAJOR_UPDATE (~50-100 new lines)

**Changes Required**:

#### A. Add Performance Optimization Section

**Location**: After "주요 기능" section, before footer

**Content** (use Template 2 - High Technical Detail):
```markdown
## 성능 최적화

### BVH 가속 구조 (Bounding Volume Hierarchy)
[Full technical detail from research.md RT-001]
- Algorithm explanation
- Data structures (t_bvh_node, t_aabb)
- Performance metrics (2-10x speedup)

### 디바운싱 렌더링
[Detail from research.md RT-002]
- 4-state machine (IDLE → ACTIVE → PREVIEW → FINAL)
- 150ms timer
- 90%+ render reduction
```

#### B. Update Version and Date

**Location**: Footer

```markdown
---

**Last Updated**: 2026-01  
**Version**: 2.1.0
```

#### C. Add Code Quality Mention

**Location**: Within appropriate section

```markdown
### 코드 품질
- 100% Norminette 준수
- 메모리 누수 제로
- 포괄적인 테스트 커버리지
```

**Validation**:
- [ ] BVH section has full technical detail
- [ ] Debounce section explains state machine
- [ ] Performance metrics included
- [ ] Korean terminology consistent
- [ ] Version updated to 2.1.0

**Estimated Time**: 1.5-2 hours

---

### 3.2 Update 레이-트레이싱-알고리즘.md

**Complexity**: MAJOR_UPDATE (~40-60 new lines)

**Changes Required**:

#### A. Add BVH Algorithm Section

**Location**: After existing ray tracing algorithm sections

**Content Structure**:
```markdown
## BVH 가속 구조 (Bounding Volume Hierarchy)

### 개요
[1-2 paragraph intro from research.md]

### 알고리즘

#### 트리 구축
1. [Steps from research.md RT-001]
2. ...

#### 트리 순회
1. [Steps from research.md RT-001]
2. ...

### 데이터 구조
- **t_bvh_node**: [Description]
- **t_aabb**: [Description]

### 복잡도 분석
- 구축: O(n log n)
- 순회: O(log n) vs O(n) brute-force

### 성능
[Metrics from research.md RT-003]
```

#### B. Update Footer

**Validation**:
- [ ] Algorithm steps are accurate
- [ ] Complexity analysis correct
- [ ] Korean terms match guide
- [ ] Links to 프로그램-개요.md if cross-referenced

**Estimated Time**: 1-1.5 hours

---

## Step 4: Update 인터랙티브-컨트롤.md

**Complexity**: MAJOR_UPDATE (~40-60 new lines)

**Changes Required**:

### A. Add Rendering Behavior Section

**Content** (Template 3 - Medium Technical Detail):
```markdown
## 렌더링 동작

### 디바운싱 (Debouncing)
[User-friendly explanation from research.md RT-002]
- 150ms timer explanation
- Why it improves experience
- No perceptible input lag

### 프리뷰 모드 (Preview Mode)
[Description from research.md]
- Instant visual feedback
- Auto-upgrade to final render
- Quality vs speed tradeoff
```

### B. Update Footer

**Validation**:
- [ ] User-facing language (not too technical)
- [ ] Benefits clearly explained
- [ ] Preview mode behavior described

**Estimated Time**: 45-60 minutes

---

## Step 5: Update FAQ.md

**Complexity**: MAJOR_UPDATE (~60-80 new lines)

**Changes Required**:

### A. Add Performance Section

**New Q&A Entries** (Template 4 - Troubleshooting):

```markdown
## 성능 관련 질문

#### Q: BVH란 무엇인가요?
[Answer from research.md RT-001]

#### Q: 성능이 느린 이유는?
[Troubleshooting from research.md RT-005, Issue 2]

#### Q: 메모리 사용량이 증가했어요
[Troubleshooting from research.md RT-005, Issue 3]
```

### B. Add Rendering Issues Section

```markdown
## 렌더링 문제

#### Q: 화면이 검은색으로 나옵니다
[Troubleshooting from research.md RT-005, Issue 1]

#### Q: 렌더링이 지연됩니다
[Troubleshooting from research.md RT-005, Issue 4]

#### Q: 프리뷰 모드가 보이지 않습니다
[Troubleshooting from research.md RT-005, Issue 5]
```

### C. Update Footer

**Validation**:
- [ ] 6+ new Q&A entries
- [ ] Solutions are actionable
- [ ] Cross-references to other wiki pages where relevant

**Estimated Time**: 1-1.5 hours

---

## Step 6: Update Minor Pages

### 6.1 Update Home.md

**Changes**:
1. Add link to 버전-히스토리.md in navigation
2. Update performance features mention (BVH)
3. Update footer (version + date)

**Location of Changes**:
- Navigation/TOC section: Add version history link
- 주요 특징 → 성능 section: Mention BVH (1-2 lines)

**Estimated Time**: 15-20 minutes

---

### 6.2 Update README.md

**Changes** (if exists in wiki/):
1. Version number
2. Date
3. Brief feature mention

**Estimated Time**: 10 minutes

---

### 6.3 Update Remaining Pages

**Pages with Minimal Changes**:
- 씬-파일-형식.md: Version + date only
- 조명-시스템.md: Version + date only
- 빠른-시작-가이드.md: Version + date only

**Changes Per Page**:
```markdown
---

**Last Updated**: 2026-01  
**Version**: 2.1.0
```

**Estimated Time**: 5 minutes per page (15 minutes total)

---

## Step 7: Validate All Changes

### 7.1 Link Validation

**Check Internal Links**:
```bash
cd /home/yoshin/work/miniRT/wiki

# List all .md links in wiki pages
grep -r "\[.*\](.*\.md" *.md

# Verify each target file exists
```

**Common Links to Check**:
- Home.md → 버전-히스토리.md (NEW)
- Any page → 프로그램-개요.md
- FAQ.md → 레이-트레이싱-알고리즘.md#bvh-가속-구조

**Validation**:
- [ ] All internal wiki links resolve
- [ ] CHANGELOG.md link correct: `../../docs/project/CHANGELOG.md`
- [ ] Anchor links reference actual headings

---

### 7.2 Terminology Consistency

**Check Across All Pages**:
```bash
# Check for consistent BVH usage
grep -i "bvh" wiki/*.md | grep -v "BVH"

# Check for consistent preview mode term
grep -E "(프리뷰|미리보기)" wiki/*.md
```

**Validation**:
- [ ] "BVH 가속 구조" used consistently (not "BVH 가속구조")
- [ ] "프리뷰 모드" used consistently (not "미리보기 모드")
- [ ] "디바운싱" used consistently
- [ ] English terms in parentheses on first mention

---

### 7.3 Version Consistency

**Check All Footers**:
```bash
# Check version references
grep -E "Version.*:" wiki/*.md

# Should all show: 2.1.0
```

**Validation**:
- [ ] All pages show "Version**: 2.1.0"
- [ ] All pages show "Last Updated**: 2026-01" or later
- [ ] No references to old version "2.0.0" except in version history

---

### 7.4 Markdown Syntax

**Validate Syntax**:
```bash
# Check for unclosed code blocks
grep -c "^\`\`\`" wiki/*.md  # Should be even numbers

# Check heading hierarchy (no H1 → H3 jumps)
grep "^#" wiki/프로그램-개요.md
```

**Validation**:
- [ ] Code blocks properly opened and closed
- [ ] Heading hierarchy correct (no skipped levels)
- [ ] Tables properly formatted
- [ ] Lists use consistent markers (-)

---

## Step 8: Quality Review

### 8.1 Content Quality Checklist

**Per Page**:
- [ ] Technical accuracy verified against source code
- [ ] Performance claims backed by CHANGELOG.md
- [ ] Korean grammar correct (띄어쓰기, 조사)
- [ ] Formal tone (합니다체) throughout
- [ ] Clear, concise explanations

### 8.2 Cross-Page Consistency

- [ ] Feature names identical across pages
- [ ] BVH described consistently (same algorithm steps)
- [ ] Performance metrics match across pages
- [ ] Troubleshooting aligned with feature descriptions

### 8.3 User Experience Test

**Read Through as User**:
1. Start at Home.md
2. Navigate to 프로그램-개요.md
3. Follow link to 레이-트레이싱-알고리즘.md
4. Check FAQ.md for troubleshooting

**Questions**:
- [ ] Can user find v2.1.0 features within 2 clicks?
- [ ] Are performance claims clear and credible?
- [ ] Is troubleshooting helpful and actionable?
- [ ] Do links flow naturally between pages?

---

## Step 9: Commit Changes

### 9.1 Git Status Check

```bash
cd /home/yoshin/work/miniRT
git status

# Should show:
# - Modified: wiki/Home.md, wiki/프로그램-개요.md, etc.
# - New file: wiki/버전-히스토리.md
```

### 9.2 Stage Changes

```bash
git add wiki/버전-히스토리.md
git add wiki/Home.md
git add wiki/프로그램-개요.md
git add wiki/레이-트레이싱-알고리즘.md
git add wiki/인터랙티브-컨트롤.md
git add wiki/FAQ.md
git add wiki/README.md
git add wiki/씬-파일-형식.md
git add wiki/조명-시스템.md
git add wiki/빠른-시작-가이드.md
```

### 9.3 Commit with Descriptive Message

```bash
git commit -m "docs(wiki): Update wiki to v2.1.0 with BVH, debouncing, preview mode

- Add 버전-히스토리.md with version changelog
- Document BVH acceleration (2-10x speedup) in 프로그램-개요.md, 레이-트레이싱-알고리즘.md
- Document debounced rendering (150ms, 4-state machine) in 인터랙티브-컨트롤.md
- Add FAQ entries for performance and troubleshooting
- Update all pages to version 2.1.0, date 2026-01
- Maintain Korean language consistency throughout

Closes #019-wiki-update
"
```

### 9.4 Push to Branch

```bash
git push origin 019-wiki-update
```

---

## Post-Commit Verification

### GitHub Preview Test

1. Navigate to GitHub wiki in browser
2. Check Home.md renders correctly
3. Click links to verify navigation
4. Read 버전-히스토리.md to verify CHANGELOG link

### Quality Gate Verification

**Success Criteria from Spec**:
- ✅ SC-001: All pages reference v2.1.0, January 2026 dates
- ✅ SC-002: Features documented within 2 clicks from home
- ✅ SC-003: Performance data included (best-effort)
- ✅ SC-004: Version history lists all versions
- ✅ SC-005: FAQ answers 3+ performance questions
- ✅ SC-006: Korean terminology consistent
- ✅ SC-007: Zero broken links
- ✅ SC-008: Markdown renders correctly

---

## Troubleshooting

### Issue: Broken Links After Update

**Symptom**: Links return 404 or don't navigate

**Solution**:
1. Check file name spelling (case-sensitive on Linux)
2. Verify target file exists in wiki/
3. Use relative paths (not absolute)
4. Check anchor format (#bvh-가속-구조 not #BVH-가속-구조)

---

### Issue: Markdown Not Rendering

**Symptom**: Raw markdown visible instead of formatted

**Solution**:
1. Check for unclosed code blocks (missing closing ```)
2. Verify tables have proper pipe alignment
3. Check heading syntax (# with space)
4. Test with online markdown previewer

---

### Issue: Korean Text Garbled

**Symptom**: Korean characters display incorrectly

**Solution**:
1. Verify file encoding is UTF-8
2. Save file with UTF-8 encoding in editor
3. Check Git config: `git config core.quotepath false`

---

## Summary

**Total Estimated Time**: 4-6 hours

**Pages Updated**: 10 total
- Created: 1 (버전-히스토리.md)
- Major updates: 4 (프로그램-개요.md, 레이-트레이싱-알고리즘.md, 인터랙티브-컨트롤.md, FAQ.md)
- Minor updates: 5 (Home.md, README.md, 씬-파일-형식.md, 조명-시스템.md, 빠른-시작-가이드.md)

**Key Deliverables**:
- Complete v2.1.0 feature documentation
- Version history page with changelog
- Performance metrics and troubleshooting
- Consistent Korean terminology
- Validated links and formatting

---

**Quickstart Status**: ✅ Complete  
**Ready for Execution**: Phase 2 (Task Breakdown) or direct implementation
