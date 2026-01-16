# Korean Technical Terminology Guide

**Feature**: 019-wiki-update  
**Purpose**: Standardized Korean translations for technical terms

This guide ensures consistent terminology across all wiki pages for miniRT v2.1.0 documentation.

---

## Terminology Standards

### Principle: Korean Primary, English Secondary

**Format**: Korean term (English term in parentheses on first mention)

**Examples**:
- ✅ "BVH 가속 구조 (Bounding Volume Hierarchy)"
- ✅ "디바운싱 타이머"
- ✅ "렌더링 파이프라인"
- ❌ "Bounding Volume Hierarchy acceleration structure" (too much English)
- ❌ "렌더링 pipeline" (inconsistent mixing)

---

## Core Terminology (Established)

### From Existing Wiki Pages

| English | Korean | Notes |
|---------|--------|-------|
| Ray Tracing | 광선 추적 / 레이 트레이싱 | Both acceptable, use consistently |
| Rendering | 렌더링 | Never translate |
| Scene | 장면 | Always in Korean |
| Object | 객체 | Always in Korean |
| Camera | 카메라 | Korean form of camera |
| Light | 광원 / 조명 | 광원 = light source, 조명 = lighting |
| Sphere | 구체 | Always in Korean |
| Plane | 평면 | Always in Korean |
| Cylinder | 원기둥 | Always in Korean |
| Intersection | 교차 (검사) | Context-dependent |
| Normal Vector | 법선 벡터 | Standard translation |
| Shadow | 그림자 | Always in Korean |
| Phong Lighting | Phong 조명 (모델) | Keep "Phong" in English |
| Ambient | 주변광 | Standard lighting term |
| Diffuse | 난반사 | Standard lighting term |
| Specular | 정반사 | Standard lighting term |

---

## New v2.1.0 Terminology

### BVH and Spatial Acceleration

| English | Korean | Usage Example |
|---------|--------|---------------|
| BVH (Bounding Volume Hierarchy) | BVH 가속 구조 | BVH 가속 구조 (Bounding Volume Hierarchy) |
| Acceleration Structure | 가속 구조 | 공간 가속 구조 |
| AABB (Axis-Aligned Bounding Box) | AABB / 축 정렬 경계 상자 | AABB (축 정렬 경계 상자) |
| Bounding Box | 경계 상자 | 객체를 포함하는 경계 상자 |
| Spatial Subdivision | 공간 분할 | 이진 트리 구조로 공간 분할 |
| Tree Traversal | 트리 순회 | 재귀적 트리 순회 |
| Binary Tree | 이진 트리 | 이진 트리 구조 |
| Tree Node | 트리 노드 | BVH 트리 노드 |
| Leaf Node | 리프 노드 | 리프 노드에 객체 저장 |
| Internal Node | 내부 노드 | 내부 노드는 자식 포인터 보유 |
| Partition | 분할 | 객체를 좌/우로 분할 |
| Split Axis | 분할 축 | X, Y, Z 중 분할 축 선택 |
| Centroid | 중심점 / 무게중심 | 객체의 중심점 기준 |
| Early Termination | 조기 종료 | 교차 없으면 조기 종료 |

### Rendering State and Debouncing

| English | Korean | Usage Example |
|---------|--------|---------------|
| Debouncing | 디바운싱 | 150ms 디바운싱 타이머 |
| Debounce Timer | 디바운스 타이머 | 디바운스 타이머 시작 |
| State Machine | 상태 기계 / 상태 머신 | 4-상태 기계 |
| State Transition | 상태 전이 / 상태 전환 | 자동 상태 전이 |
| Preview Mode | 프리뷰 모드 / 미리보기 모드 | 프리뷰 모드 활성화 |
| Render State | 렌더 상태 / 렌더링 상태 | 렌더 상태 추적 |
| Quality Mode | 품질 모드 | 낮은 품질 모드 |
| Interactive | 인터랙티브 / 실시간 | 인터랙티브 컨트롤 |
| Input Lag | 입력 지연 / 입력 렉 | 입력 지연 제거 |
| Input Buffering | 입력 버퍼링 | 입력 버퍼링 문제 해결 |
| Frame | 프레임 | 초당 프레임 수 |
| Timer | 타이머 | 150ms 타이머 |
| Delay | 지연 / 딜레이 | 150ms 지연 |
| Cancel | 취소 | 렌더 취소 |
| Transition | 전환 / 전이 | 자동 전환 |

### Performance and Optimization

| English | Korean | Usage Example |
|---------|--------|---------------|
| Speedup | 속도 향상 | 2-10배 속도 향상 |
| Performance | 성능 | 렌더링 성능 개선 |
| Optimization | 최적화 | 알고리즘 최적화 |
| Overhead | 오버헤드 | 오버헤드 최소 |
| Complexity | 복잡도 | 알고리즘 복잡도 O(log n) |
| Algorithm | 알고리즘 | BVH 트리 구축 알고리즘 |
| Benchmark | 벤치마크 | 성능 벤치마크 |
| Metric | 메트릭 / 지표 | 성능 메트릭 |
| Latency | 지연 시간 / 레이턴시 | 렌더링 지연 시간 |
| Throughput | 처리량 | 초당 렌더링 처리량 |

### Code Quality and Development

| English | Korean | Usage Example |
|---------|--------|---------------|
| Norminette | Norminette | Norminette 준수 |
| Compliance | 준수 | 100% 준수 |
| Code Style | 코딩 스타일 / 코드 스타일 | 코딩 스타일 규칙 |
| Memory Leak | 메모리 누수 | 메모리 누수 제로 |
| Bug | 버그 | 버그 수정 |
| Fix | 수정 | 버그 수정 |
| Feature | 기능 | 새로운 기능 |
| Implementation | 구현 | 기술적 구현 |
| Data Structure | 자료 구조 / 데이터 구조 | 트리 자료 구조 |
| Identifier | 식별자 | 통합 객체 식별자 |

---

## Domain-Specific Terms

### Computer Graphics

| English | Korean | Notes |
|---------|--------|-------|
| Pixel | 픽셀 | Never translate |
| Resolution | 해상도 | Standard term |
| Color | 색상 / 컬러 | Prefer 색상 |
| RGB | RGB | Keep in English |
| Coordinate | 좌표 | Standard term |
| Vector | 벡터 | Standard term |
| Matrix | 행렬 | Standard term |
| Transformation | 변환 | Standard term |
| Rotation | 회전 | Standard term |
| Translation | 이동 | Standard term |

### User Interface

| English | Korean | Notes |
|---------|--------|-------|
| HUD (Heads-Up Display) | HUD / 화면 정보 표시 | HUD preferred |
| Control | 컨트롤 / 조작 | Context-dependent |
| Keyboard | 키보드 | Standard term |
| Mouse | 마우스 | Standard term |
| Input | 입력 | Standard term |
| Output | 출력 | Standard term |
| Display | 표시 / 디스플레이 | Prefer 표시 |
| Screen | 화면 | Standard term |

---

## Usage Patterns

### First Mention Pattern

**Rule**: Introduce English term in parentheses on first mention per page

**Example**:
```markdown
BVH 가속 구조 (Bounding Volume Hierarchy)를 사용하면 복잡한 장면에서 
렌더링 성능이 크게 향상됩니다. BVH는 공간 분할 기법으로...
```

### Subsequent Mentions Pattern

**Rule**: Use Korean term only after first mention

**Example**:
```markdown
BVH 트리는 이진 구조로 구성되며, 각 노드는 AABB를 저장합니다. 
트리 순회 시 광선이 AABB와 교차하지 않으면 조기 종료됩니다.
```

### Acronym Pattern

**Rule**: Well-known acronyms (BVH, AABB, HUD, RGB) can remain in English

**Example**:
```markdown
✅ "BVH 트리 노드"
✅ "AABB 경계 상자"
✅ "HUD 정보 표시"
❌ "경계 체적 계층 구조 트리 노드" (too verbose)
```

### Code Terms Pattern

**Rule**: Code identifiers, function names, variables remain in English

**Example**:
```markdown
`is_rendering` 플래그를 사용하여 렌더 상태를 추적합니다.
`t_bvh_node` 구조체는 트리 노드 정보를 저장합니다.
```

---

## Consistency Rules

### 1. Pick One Translation and Stick to It

**Examples**:
- ✅ Always use "광선 추적" OR always use "레이 트레이싱"
- ✅ Always use "프리뷰 모드" OR always use "미리보기 모드"
- ❌ Mix "광선 추적" and "레이 트레이싱" in same document

**Recommendation for v2.1.0 Wiki**:
- Use "레이 트레이싱" (matches existing wiki style)
- Use "프리뷰 모드" (shorter, matches technical tone)
- Use "상태 기계" (more formal than "상태 머신")

### 2. Respect Existing Wiki Terminology

**Check existing pages first**: Before introducing new translation, verify if term already appears in wiki

**Example**:
- Existing wiki uses "광선 추적" → continue using it
- Existing wiki uses "렌더링" → never change to "렌더 처리"

### 3. Technical vs. User-Friendly Trade-off

**High technical detail pages** (레이-트레이싱-알고리즘.md):
- Use more formal Korean: "알고리즘 복잡도", "시간 복잡도"
- Include English terms in parentheses

**User-facing pages** (빠른-시작-가이드.md):
- Use simpler Korean: "속도가 빨라집니다"
- Minimize English technical jargon

---

## Common Phrases

### Performance Descriptions

| English | Korean |
|---------|--------|
| "2-10x speedup" | "2-10배 속도 향상" |
| "significant performance improvement" | "큰 성능 향상" / "성능이 크게 개선" |
| "minimal overhead" | "오버헤드 최소" / "최소한의 오버헤드" |
| "complex scenes" | "복잡한 장면" |
| "50+ objects" | "50개 이상의 객체" |
| "on average" | "평균적으로" |
| "in practice" | "실제로" / "실사용 시" |

### Algorithm Descriptions

| English | Korean |
|---------|--------|
| "binary tree structure" | "이진 트리 구조" |
| "recursive subdivision" | "재귀적 분할" |
| "spatial partitioning" | "공간 분할" |
| "early termination" | "조기 종료" |
| "O(log n) complexity" | "O(log n) 복잡도" |
| "traversal algorithm" | "순회 알고리즘" |

### User Experience Descriptions

| English | Korean |
|---------|--------|
| "smooth interaction" | "부드러운 인터랙션" / "부드러운 조작감" |
| "instant feedback" | "즉각적인 피드백" / "즉시 반영" |
| "responsive controls" | "반응이 빠른 컨트롤" |
| "no perceptible lag" | "체감 지연 없음" |
| "seamless experience" | "끊김 없는 경험" |

---

## Quality Checklist

Before finalizing wiki content:

- ✅ First mention includes English in parentheses
- ✅ Subsequent mentions use Korean only
- ✅ Acronyms (BVH, AABB) kept in English
- ✅ Code terms (`is_rendering`) kept in English
- ✅ Consistent translation throughout page
- ✅ Matches existing wiki terminology
- ✅ Appropriate technical depth for target audience
- ✅ Korean grammar is correct and natural

---

## Examples in Context

### Example 1: High Technical Detail (프로그램-개요.md)

```markdown
### BVH 가속 구조 (Bounding Volume Hierarchy)

BVH는 공간 분할 기법으로 광선-객체 교차 검사를 O(n)에서 O(log n)으로 
최적화합니다. 이진 트리 구조를 사용하여 장면을 재귀적으로 분할하고, 
각 노드는 AABB (축 정렬 경계 상자)를 저장합니다.

**알고리즘**:
1. 모든 객체를 포함하는 AABB 계산
2. 가장 긴 분할 축 선택 (X, Y, Z)
3. 중간점에서 객체 분할
4. 재귀적으로 트리 구축

**성능**: 복잡한 장면(50개 이상 객체)에서 2-10배 속도 향상
```

### Example 2: User-Friendly (인터랙티브-컨트롤.md)

```markdown
### 디바운싱 렌더링

카메라를 이동하거나 회전할 때, 프로그램이 모든 키 입력마다 렌더링하지 
않고 150ms 기다린 후 렌더링을 시작합니다. 이를 통해:

- 입력 지연 없는 부드러운 조작
- 불필요한 렌더링 90% 이상 감소
- 프리뷰 모드로 즉각적인 시각 피드백

입력을 멈추면 먼저 빠른 프리뷰를 보여주고, 이후 자동으로 최종 
고품질 렌더링으로 전환됩니다.
```

### Example 3: Troubleshooting (FAQ.md)

```markdown
#### Q: BVH를 활성화했는데 성능이 오히려 느려졌어요

**원인**: 장면의 객체 수가 너무 적어서 BVH 오버헤드가 이득보다 큽니다.

**해결 방법**: BVH는 50개 이상의 객체가 있는 복잡한 장면에서 효과적입니다. 
간단한 장면(10개 미만 객체)에서는 오버헤드로 인해 성능 향상이 미미할 
수 있습니다. 이는 정상적인 동작입니다.
```

---

**Terminology Guide Status**: ✅ Complete  
**Related Documents**: content-templates.md, style-guide.md
