# Content Templates

**Feature**: 019-wiki-update  
**Purpose**: Standard markdown patterns for wiki content updates

This document provides reusable templates for consistent wiki documentation.

---

## Template 1: Version Information Block

**Usage**: Footer of every wiki page

**Template** (Korean):
```markdown
---

**Last Updated**: 2026-01  
**Version**: 2.1.0
```

**Example**:
```markdown
---

**Last Updated**: 2026-01  
**Version**: 2.1.0
```

---

## Template 2: Feature Description (High Technical Detail)

**Usage**: Documenting v2.1.0 features in 프로그램-개요.md, 레이-트레이싱-알고리즘.md

**Template** (Korean):
```markdown
### [기능명] ([Feature Name in English])

**개요**: [1-2 sentence description in Korean - what it does and why it matters]

**기술 세부사항**:
- **[Technical aspect 1]**: [Detailed explanation]
- **[Technical aspect 2]**: [Detailed explanation]
- **[Technical aspect 3]**: [Detailed explanation]

**알고리즘**:
1. [Step 1 of algorithm]
2. [Step 2 of algorithm]
3. [Step 3 of algorithm]

**데이터 구조**:
- **[Structure name]**: [Description and purpose]
- **[Structure name]**: [Description and purpose]

**성능**: [Performance metrics with numbers]

**사용법**: [How users experience or interact with this feature]
```

**Example (BVH)**:
```markdown
### BVH 가속 구조 (Bounding Volume Hierarchy)

**개요**: BVH는 공간 분할 기법으로 광선-객체 교차 검사를 최적화하여 
복잡한 장면에서 2-10배의 렌더링 속도 향상을 제공합니다.

**기술 세부사항**:
- **공간 분할**: 이진 트리 구조로 장면을 재귀적으로 분할
- **AABB 경계 상자**: 축 정렬 경계 상자로 객체 그룹 포함
- **조기 종료**: 광선이 경계 상자와 교차하지 않으면 하위 트리 건너뜀

**알고리즘**:
1. 모든 객체를 포함하는 AABB 계산
2. 가장 긴 축 (X, Y, Z 중) 선택
3. 중간점에서 객체를 좌/우로 분할
4. 각 자식 노드에 대해 재귀적으로 반복
5. 리프 노드에 8개 이하 객체 저장

**데이터 구조**:
- **t_bvh_node**: 이진 트리 노드 (AABB + 자식 포인터)
- **t_aabb**: 경계 상자 (min/max 3D 좌표)

**성능**: 복잡한 장면(50개 이상 객체)에서 2-10배 속도 향상, 
간단한 장면에서는 오버헤드 최소

**사용법**: v2.1.0부터 기본 활성화, 별도 설정 불필요
```

---

## Template 3: Feature Description (Medium Technical Detail)

**Usage**: User-facing documentation in 인터랙티브-컨트롤.md, FAQ.md

**Template** (Korean):
```markdown
### [기능명]

**설명**: [What it does in user terms]

**동작 방식**:
- [Key behavior 1]
- [Key behavior 2]
- [Key behavior 3]

**효과**: [Observable benefits to user]

**참고**: [Additional notes or tips]
```

**Example (Preview Mode)**:
```markdown
### 프리뷰 모드 (Preview Mode)

**설명**: 인터랙션 중 낮은 품질의 빠른 프리뷰를 먼저 렌더링하여 
즉각적인 시각적 피드백을 제공합니다.

**동작 방식**:
- 입력 후 150ms 대기 (디바운싱)
- 낮은 품질 프리뷰 렌더링 시작 (<100ms)
- 프리뷰 완료 후 자동으로 최종 품질 렌더링

**효과**: 카메라 이동이나 회전 시 즉각적인 반응으로 부드러운 
사용자 경험 제공

**참고**: 프리뷰는 임시 이미지이며, 항상 최종 고품질 렌더로 
자동 업그레이드됩니다.
```

---

## Template 4: Troubleshooting Entry

**Usage**: FAQ.md troubleshooting sections

**Template** (Korean):
```markdown
#### Q: [문제 설명 (질문 형식)]

**증상**: [사용자가 관찰하는 현상]

**원인**: [문제가 발생하는 이유]

**해결 방법**:
1. [구체적인 해결 단계 1]
2. [구체적인 해결 단계 2]
3. [구체적인 해결 단계 3]

**참고**: [추가 정보나 관련 링크]
```

**Example**:
```markdown
#### Q: BVH를 활성화했는데 화면이 검은색으로 나옵니다

**증상**: 복잡한 장면을 렌더링할 때 화면 전체가 검은색으로 표시됨

**원인**: v2.0.x 버전의 BVH 통합 버그 (v2.1.0에서 수정됨)

**해결 방법**:
1. miniRT를 v2.1.0 이상으로 업데이트
2. 간단한 장면(객체 수 적음)으로 먼저 테스트
3. 장면 파일의 객체 정의가 올바른지 확인

**참고**: v2.1.0부터 BVH가 기본 활성화되며 이 문제는 해결되었습니다. 
자세한 내용은 [CHANGELOG.md](../../docs/project/CHANGELOG.md)를 참조하세요.
```

---

## Template 5: Version History Entry

**Usage**: 버전-히스토리.md page

**Template** (Korean):
```markdown
## [버전 번호] - YYYY-MM-DD

### 주요 변경사항
- **[Feature 1]**: [Brief description]
- **[Feature 2]**: [Brief description]
- **[Feature 3]**: [Brief description]

### 추가됨 (Added)
- [New feature or capability]

### 변경됨 (Changed)
- [Modified behavior or improvement]

### 수정됨 (Fixed)
- [Bug fix or correction]

### 성능 (Performance)
- [Performance improvements with metrics]

### 세부사항
상세한 변경 내역은 [CHANGELOG.md](../../docs/project/CHANGELOG.md)를 참조하세요.

---
```

**Example (v2.1.0)**:
```markdown
## [2.1.0] - 2026-01-15

### 주요 변경사항
- **BVH 가속 구조 (Spec 017)**: 복잡한 장면에서 2-10배 렌더링 속도 향상
- **디바운싱 렌더링 (Spec 018)**: 150ms 디바운스 타이머로 입력 버퍼링 제거
- **프리뷰 모드**: 4-상태 기계 (IDLE → ACTIVE → PREVIEW → FINAL)
- **100% Norminette 준수 (Spec 016)**: 모든 코드가 norminette 통과

### 추가됨 (Added)
- BVH 트리 시각화 기능 (`--bvh-vis` 플래그)
- 통합 객체 식별자 시스템 (Spec 015)
- `is_rendering` 플래그로 정밀한 렌더 진행 추적

### 변경됨 (Changed)
- 헤더 파일 구조 재편성 (모듈성 향상)
- 모든 스펙 번호 일관성 수정

### 수정됨 (Fixed)
- BVH 활성화 시 검은 화면 문제 해결
- 렌더 취소 로직 개선 (실제 렌더링 중일 때만 취소)

### 성능 (Performance)
- BVH: 복잡한 장면(50개 이상 객체)에서 2-10배 속도 향상
- 디바운싱: 인터랙션 중 불필요한 렌더링 90% 이상 감소
- 메모리 누수 제로, 힙 할당 제로

### 세부사항
상세한 변경 내역은 [CHANGELOG.md](../../docs/project/CHANGELOG.md)를 참조하세요.

---
```

---

## Template 6: Performance Metrics Statement

**Usage**: When documenting performance improvements

**Template** (Korean):
```markdown
**성능**: [구체적 메트릭] ([조건/환경])

*참고: [데이터 출처 또는 제한 사항]*
```

**Example (Best-effort with missing data)**:
```markdown
**성능**: BVH 가속 구조를 사용하면 복잡한 장면(50개 이상의 객체)에서 
2-10배의 렌더링 속도 향상을 달성합니다. 디바운싱을 통해 인터랙션 중 
불필요한 렌더링이 90% 이상 감소하여 부드러운 사용자 경험을 제공합니다.

*참고: 성능 메트릭은 CHANGELOG.md를 기반으로 하며, 구체적인 벤치마크 
장면 사양은 현재 문서화되지 않았습니다. 실제 성능은 장면 복잡도와 
시스템 사양에 따라 달라질 수 있습니다.*
```

---

## Template 7: Code Example Block

**Usage**: When including code snippets or configuration examples

**Template** (Korean):
```markdown
**사용 예시**:
```[language]
[code content]
\```

[설명 또는 주석]
```

**Example (Scene file)**:
```markdown
**사용 예시**:
```rt
# 복잡한 장면 - BVH 가속 혜택
A  0.2                    255,255,255
C  0,0,20    0,0,-1   70
L  -40,40,30   0.7      255,255,255

sp 0,0,0     5.0        255,0,0
sp 10,0,0    5.0        0,255,0
sp 0,10,0    5.0        0,0,255
# ... 50+ objects total
\```

50개 이상의 객체를 포함한 장면에서 BVH 가속 구조의 성능 향상을 
체감할 수 있습니다.
```

---

## Template 8: Cross-Reference Link

**Usage**: Linking to other wiki pages or external docs

**Template** (Korean):
```markdown
자세한 내용은 [링크 텍스트](파일명-또는-경로.md)를 참조하세요.
```

**Examples**:
```markdown
자세한 내용은 [BVH 가속 구조](레이-트레이싱-알고리즘.md#bvh-가속-구조)를 참조하세요.

전체 변경 내역은 [CHANGELOG.md](../../docs/project/CHANGELOG.md)를 참조하세요.

키보드 컨트롤은 [인터랙티브 컨트롤](인터랙티브-컨트롤.md)을 참조하세요.
```

---

## Template 9: Table of Contents (TOC)

**Usage**: Navigation section in longer pages

**Template** (Korean):
```markdown
## 목차

- [섹션 1](#섹션-1)
- [섹션 2](#섹션-2)
  - [하위 섹션 2.1](#하위-섹션-21)
  - [하위 섹션 2.2](#하위-섹션-22)
- [섹션 3](#섹션-3)
```

**Example**:
```markdown
## 목차

- [개요](#개요)
- [주요 기능](#주요-기능)
  - [렌더링 기능](#렌더링-기능)
  - [성능 최적화](#성능-최적화)
- [기술 세부사항](#기술-세부사항)
  - [BVH 가속 구조](#bvh-가속-구조)
  - [디바운싱 렌더링](#디바운싱-렌더링)
```

---

## Template 10: Feature Comparison Table

**Usage**: Comparing before/after or different options

**Template** (Korean):
```markdown
| 항목 | [Option A] | [Option B] |
|------|------------|------------|
| [Criterion 1] | [Value] | [Value] |
| [Criterion 2] | [Value] | [Value] |
| [Criterion 3] | [Value] | [Value] |
```

**Example (BVH Performance)**:
```markdown
| 장면 복잡도 | BVH 없음 (O(n)) | BVH 사용 (O(log n)) |
|-------------|-----------------|---------------------|
| 간단 (<10 객체) | 빠름 | 빠름 (오버헤드 최소) |
| 중간 (10-50 객체) | 중간 | 빠름 (2-5배 향상) |
| 복잡 (50+ 객체) | 느림 | 빠름 (2-10배 향상) |
```

---

## Usage Guidelines

### When to Use Each Template

**Template 1 (Version Block)**: 
- Every wiki page footer
- Mandatory for all updates

**Template 2 (High Technical Detail)**:
- 프로그램-개요.md features section
- 레이-트레이싱-알고리즘.md algorithm explanations
- When documenting implementation specifics

**Template 3 (Medium Technical Detail)**:
- 인터랙티브-컨트롤.md user-facing features
- FAQ.md explanatory answers
- When focusing on user experience

**Template 4 (Troubleshooting)**:
- FAQ.md problem-solving section
- When addressing common issues

**Template 5 (Version History)**:
- 버전-히스토리.md entries
- One per version

**Template 6 (Performance Metrics)**:
- Any page discussing performance
- When citing benchmark data

**Template 7 (Code Example)**:
- 씬-파일-형식.md examples
- When showing configuration or usage

**Template 8 (Cross-Reference)**:
- Any page linking to related content
- Use consistently for navigation

**Template 9 (TOC)**:
- Long pages (>200 lines)
- Home.md navigation

**Template 10 (Comparison Table)**:
- When comparing options or versions
- Performance before/after

---

## Content Quality Checklist

Before using any template, verify:
- ✅ Korean language primary (English in parentheses)
- ✅ Technical accuracy verified against source code
- ✅ Performance claims backed by CHANGELOG.md or testing
- ✅ Links are valid and properly formatted
- ✅ Code blocks use language specifiers
- ✅ Version references are "2.1.0"
- ✅ Dates are "2026-01" or later
- ✅ Terminology consistent with style guide

---

**Templates Status**: ✅ Complete  
**Related Documents**: korean-terminology.md, style-guide.md
