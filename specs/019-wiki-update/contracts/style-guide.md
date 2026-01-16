# Wiki Style Guide

**Feature**: 019-wiki-update  
**Purpose**: Markdown formatting and style conventions

This guide defines formatting rules, style conventions, and quality standards for miniRT wiki documentation.

---

## Markdown Formatting Rules

### Headings

**Hierarchy Rules**:
- Never skip heading levels (don't jump from H1 to H3)
- Use H1 (`#`) for page title only (once per page)
- Use H2 (`##`) for major sections
- Use H3 (`###`) for subsections
- Use H4 (`####`) for Q&A in FAQ

**Example**:
```markdown
# 프로그램 개요 (H1 - page title)

## 주요 기능 (H2 - major section)

### 렌더링 기능 (H3 - subsection)

#### 지원 객체 (H4 - if needed)
```

**Spacing**:
- One blank line before heading
- One blank line after heading
- Exception: H1 title may have description paragraph directly below

---

### Lists

**Unordered Lists**:
- Use `-` (dash) for all bullet points
- Indent with 2 spaces for nested items
- One blank line before/after list

**Example**:
```markdown
**주요 기능**:
- BVH 가속 구조
  - 2-10배 속도 향상
  - O(log n) 복잡도
- 디바운싱 렌더링
- 프리뷰 모드
```

**Ordered Lists**:
- Use `1. 2. 3.` for numbered steps
- Use consistent numbering (don't restart)

**Example**:
```markdown
**알고리즘**:
1. AABB 계산
2. 분할 축 선택
3. 객체 분할
4. 재귀 호출
```

---

### Code Blocks

**Language Specification**:
- Always specify language for syntax highlighting
- Use `c` for C code, `bash` for shell, `rt` for scene files

**Example**:
````markdown
```c
typedef struct s_bvh_node
{
    t_aabb      bounds;
    void        *left;
    void        *right;
}   t_bvh_node;
```
````

**Inline Code**:
- Use backticks for: function names, variables, file names, paths
- Example: `is_rendering` 플래그, `t_bvh_node` 구조체

---

### Emphasis

**Bold** (`**text**`):
- Section labels: **개요**, **기술 세부사항**, **성능**
- Important terms on first mention
- WARNING or NOTE labels

**Italic** (`*text*`):
- Rarely used in Korean documentation
- Avoid unless absolutely necessary

**Example**:
```markdown
**성능**: BVH 가속 구조를 사용하면 2-10배 속도 향상

*참고: 구체적인 벤치마크 데이터는 CHANGELOG.md 참조*
```

---

### Links

**Internal Wiki Links**:
- Use relative paths: `[텍스트](파일명.md)`
- For anchors: `[텍스트](파일명.md#섹션-제목)`
- Korean heading anchors use lowercase with hyphens

**Example**:
```markdown
자세한 내용은 [BVH 가속 구조](레이-트레이싱-알고리즘.md#bvh-가속-구조)를 참조하세요.
```

**External Links**:
- Use full or relative paths for non-wiki files
- CHANGELOG.md: `../../docs/project/CHANGELOG.md`
- GitHub: Full URL

**Example**:
```markdown
전체 변경 내역은 [CHANGELOG.md](../../docs/project/CHANGELOG.md)를 참조하세요.
```

**Link Text Rules**:
- Use descriptive Korean text
- ✅ `[인터랙티브 컨트롤](인터랙티브-컨트롤.md)`
- ❌ `[여기](인터랙티브-컨트롤.md)` (too vague)
- ❌ `[click here](인터랙티브-컨트롤.md)` (English in Korean doc)

---

### Tables

**Format**:
- Use pipes `|` with proper spacing
- Header separator with dashes `---`
- Align columns for readability (optional but recommended)

**Example**:
```markdown
| 장면 복잡도 | BVH 없음 | BVH 사용 |
|-------------|----------|----------|
| 간단 (<10)  | 빠름     | 빠름     |
| 복잡 (50+)  | 느림     | 빠름     |
```

---

### Horizontal Rules

**Usage**:
- Separate major sections: `---`
- Three dashes minimum
- Blank lines before and after

**Example**:
```markdown
## 섹션 1

내용...

---

## 섹션 2

내용...
```

---

### Blockquotes

**Usage** (rare in wiki):
- Notes or important warnings
- Use `>` symbol

**Example**:
```markdown
> **참고**: v2.1.0부터 BVH가 기본 활성화됩니다.
```

---

## Content Style Conventions

### Korean Writing Style

**Formality Level**: 
- Use polite/formal Korean (합니다체)
- Avoid casual language (해요체, 반말)

**Examples**:
- ✅ "BVH는 성능을 향상시킵니다"
- ✅ "디바운싱을 통해 입력 지연을 제거합니다"
- ❌ "BVH는 성능을 향상시켜요" (too casual)

**Sentence Structure**:
- Clear, concise sentences
- Prefer active voice over passive
- ✅ "알고리즘이 트리를 순회합니다"
- ❌ "트리가 알고리즘에 의해 순회됩니다" (passive, verbose)

---

### Technical Accuracy

**Verification Requirements**:
- All technical claims verified against source code
- Performance metrics from CHANGELOG.md or testing
- Algorithm descriptions match implementation
- No speculation or assumptions

**When Uncertain**:
- Note missing data explicitly
- Use "best-effort" language
- Example: "*참고: 구체적인 데이터는 현재 문서화되지 않았습니다*"

---

### Section Organization

**Standard Page Structure**:
1. Title (H1)
2. Brief overview paragraph
3. Table of contents (if page >200 lines)
4. Major sections (H2)
5. Subsections (H3, H4)
6. Horizontal rule separator
7. Version footer

**Example**:
```markdown
# 프로그램 개요

miniRT는 C 언어로 구현한 레이 트레이싱 렌더러입니다.

## 목차
[if needed]

## 주요 기능

### 렌더링 기능
...

---

**Last Updated**: 2026-01  
**Version**: 2.1.0
```

---

### Version Information

**Footer Format**:
```markdown
---

**Last Updated**: YYYY-MM  
**Version**: X.Y.Z
```

**Rules**:
- Always at end of every page
- Horizontal rule before version block
- Date format: YYYY-MM or YYYY-MM-DD
- Version: Semantic versioning (2.1.0)

---

## Quality Standards

### Link Validation

**Pre-commit Checklist**:
- [ ] All internal links point to existing wiki pages
- [ ] Anchor links reference actual headings
- [ ] External links use correct paths
- [ ] No broken or 404 links

**Testing**:
```bash
# Check for broken wiki links
grep -r "\.md" wiki/*.md | grep -v "http" | # extract .md links
while read line; do
  # verify file exists
done
```

---

### Terminology Consistency

**Validation**:
- [ ] Korean terms match terminology guide
- [ ] English terms introduced in parentheses on first mention
- [ ] Consistent throughout page
- [ ] Code terms in backticks

**Cross-page Consistency**:
- [ ] Same feature uses same Korean translation across all pages
- [ ] Example: "프리뷰 모드" not "미리보기 모드" if wiki uses former

---

### Markdown Syntax

**Linting**:
- Valid GitHub-flavored markdown
- No syntax errors that break rendering
- Code blocks properly closed
- Tables properly formatted

**Common Errors to Avoid**:
- ❌ Unclosed code blocks (missing closing ```)
- ❌ Inconsistent list markers (mixing - and *)
- ❌ Broken table alignment
- ❌ Heading level skips (H1 → H3)

---

### Korean Grammar

**Grammar Checks**:
- [ ] No spelling errors (맞춤법)
- [ ] Proper spacing (띄어쓰기)
- [ ] Correct particles (조사: 은/는, 이/가, 을/를)
- [ ] Formal tone consistency

**Common Mistakes**:
- ❌ "BVH는" vs "BVH는" (double particle)
- ❌ "렌더링 이" vs "렌더링이" (spacing in particle)
- ✅ "BVH는", "렌더링이"

---

## Page-Specific Guidelines

### Home.md

**Purpose**: Wiki landing page, navigation hub

**Content**:
- Brief project introduction (1-2 paragraphs)
- Comprehensive table of contents
- Quick links (install, controls)
- Learning path recommendations
- Key features summary

**Style**: User-friendly, welcoming, organized

---

### 프로그램-개요.md

**Purpose**: Detailed project overview

**Content**:
- Project goals (educational + technical)
- Major features with technical details
- Performance characteristics
- Code quality achievements

**Style**: Balanced technical depth, comprehensive

---

### 레이-트레이싱-알고리즘.md

**Purpose**: Algorithm explanations

**Content**:
- Ray tracing fundamentals
- BVH acceleration algorithm
- Implementation details
- Complexity analysis

**Style**: High technical detail, algorithm-focused

---

### 인터랙티브-컨트롤.md

**Purpose**: User interaction guide

**Content**:
- Keyboard/mouse controls
- Debounced rendering behavior
- Preview mode explanation
- User experience features

**Style**: User-facing, practical, clear

---

### FAQ.md

**Purpose**: Common questions and troubleshooting

**Content**:
- Performance questions
- Rendering issues
- Feature usage
- Troubleshooting steps

**Style**: Q&A format, solution-oriented, helpful

**Format**:
```markdown
#### Q: [질문]

**증상**: [observable symptoms]

**원인**: [root cause]

**해결 방법**:
1. [step]
2. [step]
```

---

### 버전-히스토리.md (New)

**Purpose**: Version changelog with highlights

**Content**:
- Version entries (newest first)
- Major changes summary per version
- Link to detailed CHANGELOG.md

**Style**: Concise summaries, chronological

---

## Pre-Publication Checklist

Before committing wiki updates:

### Content Quality
- [ ] Technical accuracy verified against source code
- [ ] Performance claims backed by data
- [ ] All features documented completely
- [ ] Troubleshooting tested with actual scenarios

### Formatting
- [ ] Heading hierarchy correct (no skips)
- [ ] Code blocks have language specifiers
- [ ] Lists use consistent markers (-)
- [ ] Tables properly formatted
- [ ] Links are relative and valid

### Korean Language
- [ ] Terminology matches guide
- [ ] Grammar and spacing correct
- [ ] Formal tone (합니다체) throughout
- [ ] English terms in parentheses on first mention

### Cross-Page Consistency
- [ ] Version "2.1.0" on all pages
- [ ] Date "2026-01" or later on all pages
- [ ] Feature names consistent across pages
- [ ] Link text descriptive and accurate

### Validation
- [ ] All internal links tested
- [ ] Markdown renders without errors
- [ ] No broken images/references
- [ ] Footer version block present

---

## Common Patterns

### Feature Introduction Pattern

```markdown
### [기능명] ([English Name])

**개요**: [1-2 sentence description]

**기술 세부사항**:
- [Technical detail 1]
- [Technical detail 2]

**성능**: [Metrics if applicable]

자세한 내용은 [관련 페이지](파일명.md)를 참조하세요.
```

### Cross-Reference Pattern

```markdown
[주제]에 대한 자세한 내용은 [페이지명](파일명.md#섹션)을 참조하세요.
```

### Note/Warning Pattern

```markdown
**참고**: [Important information]

*주의: [Warning or caveat]*
```

### Performance Metric Pattern

```markdown
**성능**: [Specific metric] ([Conditions])

*참고: [Data source or limitations]*
```

---

**Style Guide Status**: ✅ Complete  
**Related Documents**: content-templates.md, korean-terminology.md
