# Data Model: Wiki Content Structure

**Feature**: 019-wiki-update  
**Date**: 2026-01-16  
**Status**: Complete

This document defines the data model for wiki content updates, including entities, relationships, validation rules, and content organization.

---

## Entity Definitions

### Entity: WikiPage

Represents a single wiki markdown file.

**Attributes**:
- `file_name` (string, required): Markdown filename (e.g., "Home.md", "프로그램-개요.md")
- `title` (string, required): Korean page title displayed in content
- `version` (string, required): Version number, must be "2.1.0"
- `last_updated` (string, required): Update date, format "2026-01" or "2026-01-XX"
- `update_type` (enum, required): CREATE | MINOR_UPDATE | MAJOR_UPDATE
- `language` (string, required): "ko" (Korean)
- `sections` (list of ContentSection, required): Ordered content sections

**Relationships**:
- WikiPage HAS_MANY ContentSection (composition, 1:N)
- WikiPage LINKS_TO WikiPage (reference, N:N, internal wiki links)
- WikiPage REFERENCES FeatureDocumentation (reference, N:N)

**Validation Rules**:
- `file_name` must end with ".md"
- `version` must be exactly "2.1.0"
- `last_updated` must be "2026-01" or later
- `sections` must contain at least one section
- All internal links must resolve to existing wiki pages

**Constraints**:
- File name must match existing wiki structure (no renames)
- No file deletions allowed (preservation requirement)
- Korean language required for all content

---

### Entity: ContentSection

Represents a section within a wiki page.

**Attributes**:
- `heading` (string, required): Section title in Korean (e.g., "주요 기능", "성능")
- `heading_level` (integer, required): Markdown heading level (1-6)
- `content_type` (enum, required): FEATURE | PERFORMANCE | USAGE | TROUBLESHOOTING | VERSION_INFO | NAVIGATION
- `technical_depth` (enum, required): HIGH | MEDIUM | LOW
- `markdown_content` (string, required): Section body in markdown format
- `order` (integer, required): Position within page

**Relationships**:
- ContentSection BELONGS_TO WikiPage (composition)
- ContentSection MAY_REFERENCE FeatureDocumentation

**Validation Rules**:
- `heading_level` must be consistent with page hierarchy
- `markdown_content` must be valid GitHub markdown
- Code blocks must use proper syntax highlighting
- Lists must use consistent formatting (- or *)
- Korean text must be grammatically correct

**Content Type Specifications**:
- **FEATURE**: Describes new or existing features with technical details
- **PERFORMANCE**: Performance metrics, benchmarks, optimization info
- **USAGE**: How-to guides, examples, configuration
- **TROUBLESHOOTING**: Error scenarios, solutions, debugging tips
- **VERSION_INFO**: Version metadata, changelog references
- **NAVIGATION**: Links to other pages, table of contents

---

### Entity: FeatureDocumentation

Represents documentation for a specific v2.1.0 feature.

**Attributes**:
- `feature_id` (string, required): Unique identifier (e.g., "bvh-acceleration")
- `feature_name_en` (string, required): English name (e.g., "BVH Acceleration")
- `feature_name_ko` (string, required): Korean name (e.g., "BVH 가속 구조")
- `spec_reference` (string, optional): Spec number (e.g., "Spec 017")
- `technical_details` (object, required): Technical implementation details
- `performance_impact` (string, optional): Performance metrics
- `usage_examples` (list of string, optional): Code or config examples
- `troubleshooting` (list of TroubleshootingEntry, optional): Common issues
- `wiki_pages` (list of string, required): Wiki pages that document this feature

**Technical Details Object**:
```json
{
  "algorithm": "string (e.g., 'Binary tree spatial subdivision')",
  "data_structures": ["list of structures used"],
  "complexity": "string (e.g., 'O(log n) traversal')",
  "parameters": {
    "param_name": "description and default value"
  },
  "implementation_files": ["source file references"]
}
```

**Relationships**:
- FeatureDocumentation IS_DOCUMENTED_IN WikiPage (reference, 1:N)
- FeatureDocumentation HAS_MANY TroubleshootingEntry (composition, 1:N)

**Validation Rules**:
- Both English and Korean names required
- At least one wiki page must document the feature
- Technical details must be accurate (verified against source code)
- Performance impact must use best-effort available data

---

### Entity: TroubleshootingEntry

Represents a common issue and solution.

**Attributes**:
- `issue_id` (string, required): Unique identifier
- `problem_ko` (string, required): Problem description in Korean
- `symptoms_ko` (string, required): Observable symptoms in Korean
- `cause_ko` (string, required): Root cause explanation in Korean
- `solution_steps_ko` (list of string, required): Solution steps in Korean
- `related_feature` (string, required): Feature ID this relates to

**Validation Rules**:
- All Korean text must be clear and actionable
- Solution steps must be numbered and sequential
- Must reference a valid FeatureDocumentation feature_id

---

## Features to Document

### Feature 1: BVH Acceleration

**Attributes**:
```yaml
feature_id: bvh-acceleration
feature_name_en: BVH Acceleration
feature_name_ko: BVH 가속 구조
spec_reference: Spec 017
technical_details:
  algorithm: Binary tree spatial subdivision with AABB bounds
  data_structures:
    - t_bvh_node: Binary tree node with AABB and object references
    - t_aabb: Axis-aligned bounding box (min/max coordinates)
  complexity: O(log n) traversal vs O(n) brute-force
  parameters:
    max_leaf_objects: 8 objects per leaf node
    split_axis: Chosen based on largest spatial extent
  implementation_files:
    - src/spatial/bvh_build_core.c
    - src/spatial/bvh_traverse.c
performance_impact: 2-10x speedup on complex scenes (50+ objects)
wiki_pages:
  - 프로그램-개요.md
  - 레이-트레이싱-알고리즘.md
  - FAQ.md
```

### Feature 2: Debounced Rendering

**Attributes**:
```yaml
feature_id: debounced-rendering
feature_name_en: Debounced Rendering
feature_name_ko: 디바운싱 렌더링
spec_reference: Spec 018
technical_details:
  algorithm: 4-state machine with timer-based transitions
  data_structures:
    - t_debounce_state: State machine state and configuration
    - t_debounce_timer: Timer with microsecond precision
  states:
    - IDLE: No activity
    - ACTIVE: Input detected, timer running (150ms)
    - PREVIEW: Low-quality preview rendering
    - FINAL: High-quality final rendering
  parameters:
    delay_ms: 150ms default debounce delay
    preview_enabled: true (default)
    auto_upgrade: true (default)
  implementation_files:
    - src/render/render_debounce.c
    - src/render/render_debounce_timer.c
performance_impact: 90%+ reduction in unnecessary renders during interaction
wiki_pages:
  - 인터랙티브-컨트롤.md
  - 프로그램-개요.md
  - FAQ.md
```

### Feature 3: Preview Mode

**Attributes**:
```yaml
feature_id: preview-mode
feature_name_en: Preview Mode
feature_name_ko: 프리뷰 모드
spec_reference: Spec 018 (part of debouncing)
technical_details:
  algorithm: Low-resolution/quality rendering for instant feedback
  data_structures:
    - Quality mode settings in render state
  parameters:
    preview_quality: Lower than final render
    auto_transition: true (upgrades to final after completion)
performance_impact: <100ms typical preview render time
wiki_pages:
  - 인터랙티브-컨트롤.md
  - FAQ.md
```

### Feature 4: Render State Tracking

**Attributes**:
```yaml
feature_id: render-state-tracking
feature_name_en: Render State Tracking
feature_name_ko: 렌더 상태 추적
spec_reference: Spec 018
technical_details:
  algorithm: Boolean flag tracking render progress
  data_structures:
    - is_rendering: Boolean flag in render context
  parameters:
    checked_on_input: true (for smart cancellation)
performance_impact: Prevents unnecessary render cancellations
wiki_pages:
  - 인터랙티브-컨트롤.md
```

### Feature 5: Unified Object Identifiers

**Attributes**:
```yaml
feature_id: unified-identifiers
feature_name_en: Unified Object Identifiers
feature_name_ko: 통합 객체 식별자
spec_reference: Spec 015
technical_details:
  algorithm: Consistent object identification across system
  data_structures:
    - Unified ID scheme for spheres, planes, cylinders
performance_impact: N/A (code quality improvement)
wiki_pages:
  - 프로그램-개요.md
  - 씬-파일-형식.md
```

### Feature 6: Norminette Compliance

**Attributes**:
```yaml
feature_id: norminette-compliance
feature_name_en: 100% Norminette Compliance
feature_name_ko: 100% Norminette 준수
spec_reference: Spec 016
technical_details:
  algorithm: N/A (code style)
  parameters:
    compliance: 100% (all files pass norminette)
    prohibited_functions_removed: Yes
performance_impact: N/A (code quality)
wiki_pages:
  - 프로그램-개요.md
  - Home.md
```

---

## Wiki Page Update Specifications

### Page 1: Home.md (MINOR_UPDATE)

**Current Version**: 2.0.0  
**Target Version**: 2.1.0

**Sections to Update**:
1. **주요 특징 → 성능** section:
   - Add BVH acceleration mention
   - Update performance claims (2-10x speedup)

2. **목차** section:
   - Add link to new 버전-히스토리.md page

3. **Footer**:
   - Update version to 2.1.0
   - Update date to 2026-01

**Estimated Changes**: ~10 lines

---

### Page 2: 프로그램-개요.md (MAJOR_UPDATE)

**Current Version**: Partial content (viewed up to line 50)  
**Target Version**: 2.1.0

**Sections to Add/Update**:
1. **성능 최적화** (new section or expand existing):
   - BVH 가속 구조 (full technical detail)
   - 디바운싱 렌더링 (state machine explanation)
   - 프리뷰 모드 (instant feedback description)

2. **주요 기능** section:
   - Add v2.1.0 features list
   - Performance metrics

3. **코드 품질** section:
   - Mention 100% norminette compliance
   - Zero memory leaks

4. **Footer**:
   - Update version to 2.1.0
   - Update date to 2026-01

**Estimated Changes**: ~50-100 lines (major content additions)

---

### Page 3: 레이-트레이싱-알고리즘.md (MAJOR_UPDATE)

**Target**: Add comprehensive BVH algorithm section

**New Section to Add**:
```markdown
## BVH 가속 구조 (Bounding Volume Hierarchy)

### 개요
BVH는 공간 분할 기법으로 광선-객체 교차 검사를 O(n)에서 O(log n)으로 
최적화합니다.

### 알고리즘

#### 트리 구축
1. 장면의 모든 객체를 포함하는 AABB 계산
2. 가장 긴 축 선택 (X, Y, Z 중)
3. 중간점에서 객체 분할
4. 각 자식 노드에 대해 재귀적으로 반복
5. 리프 노드: 8개 이하의 객체 저장

#### 트리 순회
1. 광선-AABB 교차 검사
2. 교차하지 않으면 조기 종료
3. 리프 노드: 모든 객체와 교차 검사
4. 내부 노드: 자식 노드 재귀 탐색

### 성능
- 복잡한 장면 (50+ 객체): 2-10배 속도 향상
- 간단한 장면 (<10 객체): 오버헤드 최소
```

**Estimated Changes**: ~40-60 lines

---

### Page 4: 인터랙티브-컨트롤.md (MAJOR_UPDATE)

**Target**: Document debounced rendering and preview mode

**Sections to Add**:
1. **렌더링 동작**:
   - 디바운싱 설명 (150ms timer)
   - 4-state machine visualization
   - Preview mode description

2. **사용자 경험 개선**:
   - Input lag elimination
   - Instant preview feedback
   - Smooth transitions

**Estimated Changes**: ~40-60 lines

---

### Page 5: FAQ.md (MAJOR_UPDATE)

**Target**: Add performance Q&A and troubleshooting

**New Questions to Add**:
1. **Q: BVH란 무엇인가요?**
2. **Q: 성능이 느린 이유는?** (with BVH troubleshooting)
3. **Q: 검은 화면이 나타납니다** (BVH black screen issue)
4. **Q: 렌더링이 지연됩니다** (debounce explanation)
5. **Q: 프리뷰 모드는 무엇인가요?**
6. **Q: 메모리 사용량이 증가했어요** (BVH overhead)

**Estimated Changes**: ~60-80 lines

---

### Page 6: 버전-히스토리.md (CREATE)

**Target**: New version history page linking to CHANGELOG.md

**Complete Structure**:
```markdown
# 버전 히스토리 (Version History)

miniRT 프로젝트의 버전별 변경 사항을 기록합니다.

---

## [2.1.0] - 2026-01-15

### 주요 변경사항
- **BVH 가속 구조**: 2-10배 렌더링 속도 향상
- **디바운싱 렌더링**: 90% 이상 불필요한 렌더링 제거
- **프리뷰 모드**: 인터랙션 중 즉각적인 시각적 피드백
- **100% Norminette 준수**: 모든 코드 스타일 규칙 준수
- **통합 객체 식별자**: 일관된 객체 관리

### 세부사항
전체 변경 내역은 [CHANGELOG.md](../../docs/project/CHANGELOG.md)를 
참조하세요.

---

## [2.0.0] - 2025-01-XX

### 주요 변경사항
- 프로젝트 구조 재편성
- 문서화 대폭 개선

---

## [1.2.0] - 2025-XX-XX
## [1.1.0] - 2025-XX-XX
## [1.0.1] - 2025-XX-XX
## [1.0.0] - 2025-XX-XX

[Earlier versions - brief summaries]

---

**참고**: 각 버전의 상세 변경 내역, 기술적 세부사항, 마이그레이션 
가이드는 CHANGELOG.md를 참조하세요.
```

**Estimated Changes**: ~80-100 lines (new file)

---

### Pages 7-9: Minor Updates

**씬-파일-형식.md**: Version + date only (~2 lines)
**조명-시스템.md**: Version + date only (~2 lines)
**빠른-시작-가이드.md**: Version + date only (~2 lines)
**README.md**: Version + date, brief feature mention (~5 lines)

---

## Validation Rules Summary

### Content Validation
- ✅ All version references are "2.1.0"
- ✅ All dates are "2026-01" or later
- ✅ Korean terminology is consistent (see research.md RT-004)
- ✅ Technical details are accurate (verified against source code)
- ✅ Performance metrics use best-effort available data
- ✅ Missing data is explicitly noted

### Markdown Validation
- ✅ Valid GitHub-flavored markdown syntax
- ✅ Code blocks use language specifiers (```c, ```bash)
- ✅ Consistent heading hierarchy (no skipped levels)
- ✅ Lists use consistent markers (- for unordered)
- ✅ Tables are properly formatted

### Link Validation
- ✅ All internal links point to existing wiki pages
- ✅ External links (CHANGELOG.md) use correct relative paths
- ✅ Anchor links (within page) reference existing headings
- ✅ No broken links after updates

### Style Validation
- ✅ Korean primary language
- ✅ English technical terms in parentheses on first mention
- ✅ Consistent tone and voice
- ✅ Clear, concise explanations
- ✅ Code examples are readable and accurate

---

## State Transitions

### Content States
1. **PLANNED**: Section identified in specification
2. **RESEARCHED**: Technical details gathered (research.md)
3. **DRAFTED**: Markdown content written
4. **REVIEWED**: Content validated against rules
5. **PUBLISHED**: Committed to wiki directory

### Quality Gates
- PLANNED → RESEARCHED: Research.md complete with verified details
- RESEARCHED → DRAFTED: Markdown follows templates and style guide
- DRAFTED → REVIEWED: All validation rules pass
- REVIEWED → PUBLISHED: No blocking issues, all links valid

---

## Relationships Map

```
WikiPage (Home.md)
  ├─ LINKS_TO: 버전-히스토리.md (new link)
  ├─ LINKS_TO: 프로그램-개요.md (existing)
  └─ REFERENCES: norminette-compliance

WikiPage (프로그램-개요.md)
  ├─ REFERENCES: bvh-acceleration
  ├─ REFERENCES: debounced-rendering
  ├─ REFERENCES: preview-mode
  ├─ REFERENCES: norminette-compliance
  └─ REFERENCES: unified-identifiers

WikiPage (레이-트레이싱-알고리즘.md)
  └─ REFERENCES: bvh-acceleration

WikiPage (인터랙티브-컨트롤.md)
  ├─ REFERENCES: debounced-rendering
  ├─ REFERENCES: preview-mode
  └─ REFERENCES: render-state-tracking

WikiPage (FAQ.md)
  ├─ REFERENCES: bvh-acceleration (troubleshooting)
  ├─ REFERENCES: debounced-rendering (explanation)
  └─ REFERENCES: preview-mode (explanation)

WikiPage (버전-히스토리.md) [NEW]
  └─ LINKS_TO: ../../docs/project/CHANGELOG.md
```

---

**Data Model Status**: ✅ Complete  
**Next Artifact**: contracts/ directory with templates and style guides
