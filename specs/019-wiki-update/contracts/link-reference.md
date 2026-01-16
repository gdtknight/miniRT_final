# Link Reference Guide

**Feature**: 019-wiki-update  
**Purpose**: Standard internal link patterns and anchor references

This guide provides the correct link syntax for all wiki pages and common sections.

---

## Wiki Page Links

### Standard Format

```markdown
[Display Text](파일명.md)
```

### All Wiki Pages

| Page Name (Korean) | File Name | Link Syntax |
|-------------------|-----------|-------------|
| 홈 | Home.md | `[홈](Home.md)` |
| README | README.md | `[README](README.md)` |
| FAQ | FAQ.md | `[FAQ](FAQ.md)` |
| 프로그램 개요 | 프로그램-개요.md | `[프로그램 개요](프로그램-개요.md)` |
| 빠른 시작 가이드 | 빠른-시작-가이드.md | `[빠른 시작 가이드](빠른-시작-가이드.md)` |
| 씬 파일 형식 | 씬-파일-형식.md | `[씬 파일 형식](씬-파일-형식.md)` |
| 레이 트레이싱 알고리즘 | 레이-트레이싱-알고리즘.md | `[레이 트레이싱 알고리즘](레이-트레이싱-알고리즘.md)` |
| 조명 시스템 | 조명-시스템.md | `[조명 시스템](조명-시스템.md)` |
| 인터랙티브 컨트롤 | 인터랙티브-컨트롤.md | `[인터랙티브 컨트롤](인터랙티브-컨트롤.md)` |
| 버전 히스토리 (NEW) | 버전-히스토리.md | `[버전 히스토리](버전-히스토리.md)` |

---

## Section Anchor Links

### Anchor Format

GitHub wiki anchor format:
- Convert Korean to lowercase
- Replace spaces with hyphens
- Keep Korean characters as-is

**Example**: "BVH 가속 구조" → `#bvh-가속-구조`

### Common Sections

#### 프로그램-개요.md Sections

| Section Name | Anchor Link |
|--------------|-------------|
| 주요 기능 | `[주요 기능](프로그램-개요.md#주요-기능)` |
| 성능 최적화 | `[성능 최적화](프로그램-개요.md#성능-최적화)` |
| BVH 가속 구조 | `[BVH 가속 구조](프로그램-개요.md#bvh-가속-구조)` |
| 디바운싱 렌더링 | `[디바운싱 렌더링](프로그램-개요.md#디바운싱-렌더링)` |

#### 레이-트레이싱-알고리즘.md Sections

| Section Name | Anchor Link |
|--------------|-------------|
| BVH 가속 구조 | `[BVH 가속 구조](레이-트레이싱-알고리즘.md#bvh-가속-구조)` |
| 트리 구축 | `[트리 구축](레이-트레이싱-알고리즘.md#트리-구축)` |
| 트리 순회 | `[트리 순회](레이-트레이싱-알고리즘.md#트리-순회)` |

#### 인터랙티브-컨트롤.md Sections

| Section Name | Anchor Link |
|--------------|-------------|
| 렌더링 동작 | `[렌더링 동작](인터랙티브-컨트롤.md#렌더링-동작)` |
| 디바운싱 | `[디바운싱](인터랙티브-컨트롤.md#디바운싱)` |
| 프리뷰 모드 | `[프리뷰 모드](인터랙티브-컨트롤.md#프리뷰-모드)` |

#### FAQ.md Sections

| Section Name | Anchor Link |
|--------------|-------------|
| 성능 관련 질문 | `[성능 관련 질문](FAQ.md#성능-관련-질문)` |
| 렌더링 문제 | `[렌더링 문제](FAQ.md#렌더링-문제)` |

---

## External Links

### CHANGELOG.md

**Relative path from wiki/**:
```markdown
[CHANGELOG.md](../../docs/project/CHANGELOG.md)
```

**Usage Example**:
```markdown
상세한 변경 내역은 [CHANGELOG.md](../../docs/project/CHANGELOG.md)를 참조하세요.
```

### GitHub Repository

**Full URL** (replace with actual repository):
```markdown
[GitHub 저장소](https://github.com/USERNAME/miniRT)
```

---

## Common Link Patterns

### Cross-Reference Pattern

**From any page to feature explanation**:
```markdown
자세한 내용은 [BVH 가속 구조](레이-트레이싱-알고리즘.md#bvh-가속-구조)를 참조하세요.
```

**From FAQ to feature page**:
```markdown
더 많은 정보는 [프로그램 개요](프로그램-개요.md)를 참조하세요.
```

### Navigation Pattern (Home.md)

**Table of Contents Links**:
```markdown
- **[빠른 시작 가이드](빠른-시작-가이드.md)** - 설치, 빌드, 실행 방법
- **[프로그램 개요](프로그램-개요.md)** - 프로젝트 소개 및 주요 기능
- **[버전 히스토리](버전-히스토리.md)** - 변경 사항 및 업데이트 내역
```

### Version History Link Pattern

**From any page to version history**:
```markdown
v2.1.0의 전체 변경 내역은 [버전 히스토리](버전-히스토리.md)를 참조하세요.
```

---

## Link Validation

### Pre-commit Checklist

Before committing wiki updates, verify:

1. **File name exists**:
   ```bash
   ls wiki/파일명.md  # Should exist
   ```

2. **Anchor exists**:
   ```bash
   grep "^## 섹션명" wiki/파일명.md  # Should return matching heading
   ```

3. **Relative path correct**:
   - Wiki to wiki: `파일명.md`
   - Wiki to docs: `../../docs/project/파일명.md`

### Common Link Errors

**Error**: Link returns 404
- **Cause**: File name misspelled or doesn't exist
- **Fix**: Check `ls wiki/` for exact file name

**Error**: Anchor doesn't work
- **Cause**: Heading format mismatch
- **Fix**: Use lowercase, hyphens, keep Korean characters

**Error**: External link broken
- **Cause**: Incorrect relative path
- **Fix**: Count directory levels (wiki/ → repo root = `../../`)

---

## Link Testing Script

```bash
#!/bin/bash
# test-wiki-links.sh - Validate all internal wiki links

WIKI_DIR="/home/yoshin/work/miniRT/wiki"

echo "Checking internal wiki links..."

# Extract all .md links
grep -r "\[.*\](.*\.md" "$WIKI_DIR"/*.md | while read -r line; do
    # Parse link target
    file=$(echo "$line" | sed -n 's/.*(\(.*\.md\).*/\1/p' | cut -d'#' -f1)
    
    if [ ! -z "$file" ] && [ ! -f "$WIKI_DIR/$file" ]; then
        echo "BROKEN: $line"
    fi
done

echo "Link check complete"
```

**Usage**:
```bash
chmod +x test-wiki-links.sh
./test-wiki-links.sh
```

---

## Reference Examples

### Example 1: Cross-Reference in 프로그램-개요.md

```markdown
**BVH 가속 구조**는 광선 추적 성능을 크게 향상시킵니다. 
알고리즘의 상세한 설명은 
[레이 트레이싱 알고리즘](레이-트레이싱-알고리즘.md#bvh-가속-구조)을 
참조하세요.
```

### Example 2: FAQ Answer with Multiple Links

```markdown
#### Q: BVH에 대해 더 알고 싶어요

**답변**: BVH (Bounding Volume Hierarchy)는 공간 가속 구조입니다.

자세한 내용은 다음 페이지를 참조하세요:
- [프로그램 개요 - 성능 최적화](프로그램-개요.md#성능-최적화)
- [레이 트레이싱 알고리즘 - BVH 가속 구조](레이-트레이싱-알고리즘.md#bvh-가속-구조)
- [CHANGELOG.md](../../docs/project/CHANGELOG.md) (v2.1.0 섹션)
```

### Example 3: Version History with External Link

```markdown
## [2.1.0] - 2026-01-15

### 세부사항

상세한 변경 내역은 [CHANGELOG.md](../../docs/project/CHANGELOG.md)를 
참조하세요.
```

---

**Link Reference Status**: ✅ Complete  
**Related Documents**: style-guide.md, quickstart.md
