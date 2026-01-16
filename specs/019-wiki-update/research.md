# Research: GitHub Wiki Update - Technical Details

**Feature**: 019-wiki-update  
**Date**: 2026-01-16  
**Status**: Complete

This document consolidates all technical research required to accurately document miniRT v2.1.0 features in the GitHub Wiki.

---

## RT-001: BVH Algorithm Implementation Details

### Decision
Document BVH (Bounding Volume Hierarchy) as a spatial acceleration structure using AABB (Axis-Aligned Bounding Box) tree with recursive subdivision.

### Technical Implementation

**Data Structure**:
- **BVH Tree Node** (`t_bvh_node`): Binary tree structure
  - Internal nodes: Store AABB bounds and pointers to left/right children
  - Leaf nodes: Store references to actual scene objects (1-8 objects per leaf)
  - AABB: Min/max 3D coordinate bounds for volume containment

**Algorithm - Tree Construction**:
1. **Compute Scene Bounds**: Calculate AABB encompassing all objects
2. **Choose Split Axis**: Select axis (X, Y, or Z) with largest spatial extent
3. **Calculate Split Position**: Midpoint along chosen axis
4. **Partition Objects**: Divide objects into left/right groups based on centroid
5. **Recursive Subdivision**: Repeat for each child until leaf threshold reached (≤8 objects)

**Algorithm - Tree Traversal** (Ray Intersection):
1. **AABB Ray Intersection**: Test ray against node's bounding box
2. **Early Termination**: Skip node if ray misses AABB
3. **Leaf Node Processing**: Test ray against all objects in leaf
4. **Recursive Descent**: Visit child nodes if ray hits internal node
5. **Result**: Return closest intersection found

**Complexity**:
- Construction: O(n log n) where n = object count
- Traversal: O(log n) average case vs O(n) brute-force

**Performance Impact**:
- Complex scenes (50+ objects): 2-10x speedup
- Simple scenes (<10 objects): Minimal overhead
- Memory: ~80 bytes per BVH node

### Source Files
- `src/spatial/bvh_build_core.c`: Tree construction
- `src/spatial/bvh_build_partition.c`: Object partitioning
- `src/spatial/bvh_build_split.c`: Split axis selection
- `src/spatial/bvh_traverse.c`: Ray-AABB traversal
- `includes/bvh_internal.h`: Internal structures

### Rationale
BVH is an algorithmic optimization (allowed under 42 School constraints) that reduces intersection tests from O(n) to O(log n). Critical for interactive performance with complex scenes.

### Alternatives Considered
- **Octree**: More memory overhead, complex implementation
- **k-d tree**: Similar performance, more complex balancing
- **Grid**: Poor performance with uneven object distribution

---

## RT-002: Debounced Rendering State Machine

### Decision
Implement 4-state debounce state machine with 150ms timer to eliminate input buffering and provide smooth interactive rendering.

### Technical Implementation

**State Machine** (`t_debounce_state_enum`):

```
IDLE → ACTIVE → PREVIEW → FINAL
  ↑                          ↓
  └──────────────────────────┘
```

**State Definitions**:
1. **DEBOUNCE_IDLE**: No user input, no rendering
   - Entry condition: No activity
   - Exit trigger: User input detected

2. **DEBOUNCE_ACTIVE**: Input detected, timer running
   - Entry condition: User input (camera movement, rotation)
   - Timer: 150ms countdown
   - Exit trigger: Timer expires OR new input (reset timer)

3. **DEBOUNCE_PREVIEW**: Low-quality preview rendering
   - Entry condition: Timer expired in ACTIVE state
   - Rendering: Lower resolution/quality for speed
   - Exit trigger: Render complete → auto-transition to FINAL

4. **DEBOUNCE_FINAL**: High-quality final rendering
   - Entry condition: Preview render complete
   - Rendering: Full resolution/quality
   - Exit trigger: Render complete → return to IDLE

**Timing Parameters**:
- `DEBOUNCE_DEFAULT_DELAY_MS`: 150ms (configurable)
- Timer implementation: `struct timeval` with microsecond precision
- Timer checked every frame update

**Render Cancellation**:
- Only cancels when `is_rendering` flag is true
- Prevents canceling idle/preview states
- Smart cancellation logic: Don't cancel if not actively rendering

**Configuration**:
- `preview_enabled`: Enable/disable preview mode (default: ON)
- `auto_upgrade`: Auto-transition preview→final (default: ON)
- `delay_ms`: Debounce delay (default: 150ms)

### Source Files
- `src/render/render_debounce.c`: State machine logic
- `src/render/render_debounce_timer.c`: Timer implementation
- `includes/render_debounce.h`: State definitions

### Performance Impact
- 90%+ reduction in unnecessary renders during interaction
- Smooth experience: No stuttering from input buffering
- Preview mode: Instant visual feedback (<100ms)

### Rationale
Without debouncing, every input event (key press, mouse move) triggers a full render, causing input lag and buffer overflow. The 150ms delay balances responsiveness (feels immediate) with efficiency (avoids wasted renders).

### Alternatives Considered
- **No debouncing**: Causes input buffering and lag
- **Simple delay only**: No preview feedback (feels unresponsive)
- **Frame-based throttling**: Less precise than time-based

---

## RT-003: Performance Benchmark Data

### Decision
Document best-effort performance metrics from CHANGELOG.md with available data, noting where full benchmark details are unavailable.

### Available Metrics

**BVH Acceleration**:
- **Speedup**: 2-10x on complex scenes (50+ objects)
- **Test methodology**: Compared BVH-enabled vs brute-force rendering
- **Memory overhead**: ~80 bytes per node, negligible compared to scene data
- **Construction time**: O(n log n), typically <10ms for 100 objects

**Note**: Specific test scene names and exact timing breakdowns (e.g., "scene X: 5.2s → 0.8s") are not documented in CHANGELOG.md. Documentation will use available aggregate metrics.

**Debounced Rendering**:
- **Render reduction**: 90%+ reduction in unnecessary renders during interaction
- **Timer delay**: 150ms (configurable)
- **Preview rendering**: <100ms typical preview render time
- **Input responsiveness**: No perceptible lag (subjective, <150ms feels instant)

**Overall Performance**:
- **Memory leaks**: Zero (verified with valgrind)
- **Heap allocations**: Zero in new rendering code paths
- **Complexity**: O(log n) BVH traversal vs O(n) brute-force

### Documentation Strategy

**What to include**:
- Aggregate performance metrics (2-10x, 90%+ reduction)
- Algorithmic complexity (O(log n) vs O(n))
- Qualitative improvements (smooth interaction, instant feedback)

**What to note as unavailable**:
- Specific test scene specifications (scene file names, object counts)
- Exact timing breakdowns per scene (before/after in seconds)
- Hardware specifications used for benchmarking

**Template for wiki**:
```markdown
**성능**: BVH 가속 구조를 사용하면 복잡한 장면(50개 이상의 객체)에서 
2-10배의 렌더링 속도 향상을 달성합니다. 디바운싱을 통해 인터랙션 중 
불필요한 렌더링이 90% 이상 감소하여 부드러운 사용자 경험을 제공합니다.

*참고: 구체적인 벤치마크 장면 사양은 현재 문서화되지 않았습니다.*
```

### Rationale
Best-effort documentation with available data is better than no documentation. Transparency about missing metrics builds trust.

---

## RT-004: Korean Technical Terminology Mapping

### Decision
Maintain existing Korean terminology from wiki pages and create standard translations for new v2.1.0 technical terms.

### Established Terminology (from existing wiki)

**Core Concepts**:
- Ray Tracing → 광선 추적 / 레이 트레이싱
- Rendering → 렌더링
- Scene → 장면
- Object → 객체
- Camera → 카메라
- Light → 광원 / 조명

**Technical Terms**:
- Phong Lighting → Phong 조명 모델
- Shadow → 그림자
- Intersection → 교차 (검사)
- Normal Vector → 법선 벡터
- Ambient/Diffuse/Specular → 주변광/난반사/정반사

### New v2.1.0 Terminology

**BVH Related**:
- BVH (Bounding Volume Hierarchy) → BVH 가속 구조
- Acceleration Structure → 가속 구조
- AABB (Axis-Aligned Bounding Box) → AABB (축 정렬 경계 상자)
- Spatial Subdivision → 공간 분할
- Tree Traversal → 트리 순회

**Rendering State**:
- Debouncing → 디바운싱
- Preview Mode → 프리뷰 모드 / 미리보기 모드
- State Machine → 상태 기계 / 상태 머신
- Render State → 렌더 상태
- Quality Mode → 품질 모드

**Performance**:
- Speedup → 속도 향상
- Optimization → 최적화
- Algorithm Complexity → 알고리즘 복잡도
- Interactive → 인터랙티브 / 실시간

**Code Quality**:
- Norminette Compliance → Norminette 준수
- Memory Leak → 메모리 누수
- Code Style → 코딩 스타일

### Style Guidelines

**Korean-English Mixing**:
- Use Korean primary, English in parentheses for first mention
- Technical acronyms in English: BVH, AABB, HUD
- Well-known terms in English: Phong, Ray Tracing (can use Korean equivalent)

**Examples**:
- ✅ "BVH 가속 구조 (Bounding Volume Hierarchy)"
- ✅ "150ms 디바운싱 타이머"
- ✅ "AABB (축 정렬 경계 상자)를 사용한 트리 순회"
- ❌ "Bounding Volume Hierarchy acceleration structure" (too much English)

### Consistency Rules
1. First mention: Korean + English in parentheses
2. Subsequent mentions: Korean only (unless acronym)
3. Code terms: Keep English (e.g., `is_rendering` flag)
4. Measurements: Use consistent units (ms for time, x for multiplier)

---

## RT-005: Common Error Scenarios

### Decision
Document common user errors and troubleshooting for BVH, preview mode, and debounced rendering.

### BVH Acceleration Issues

#### Issue 1: Black Screen with Complex Scenes
**증상 (Symptoms)**: Scene renders as black screen when BVH is enabled
**원인 (Cause)**: BVH tree construction bug or AABB calculation error (fixed in v2.1.0)
**해결 방법 (Solution)**:
1. Verify using miniRT v2.1.0 or later
2. Test with simple scene first (fewer objects)
3. Check scene file for invalid object definitions

#### Issue 2: No Performance Improvement
**증상**: BVH enabled but no speed improvement
**원인**: Scene has too few objects (BVH overhead dominates)
**해결 방법**:
1. BVH provides benefit with 50+ objects
2. For simple scenes (<10 objects), overhead may exceed benefit
3. This is expected behavior, not a bug

#### Issue 3: Increased Memory Usage
**증상**: Higher memory consumption with BVH enabled
**원인**: BVH tree structure requires additional memory (~80 bytes/node)
**해결 방법**:
1. Normal behavior - memory trade-off for speed
2. Memory increase is proportional to object count
3. Typically <1MB for scenes with <1000 objects

### Debounced Rendering Issues

#### Issue 4: Delayed Rendering Response
**증상**: Rendering starts 150ms after input stops
**원인**: Debounce timer working as designed
**해결 방법**:
1. This is intentional behavior for performance
2. Preview mode provides instant feedback
3. Can disable debouncing if needed (not recommended)

#### Issue 5: Preview Mode Not Showing
**증상**: No preview render, only final render
**원인**: Preview mode disabled in configuration
**해결 방법**:
1. Verify `preview_enabled` is ON (default)
2. Check that scene complexity allows preview rendering
3. Ensure no errors in render pipeline

### Preview Mode Issues

#### Issue 6: Preview Quality Too Low
**증상**: Preview render is blocky or low quality
**원인**: Preview mode intentionally uses lower quality for speed
**해결 방법**:
1. Wait for auto-upgrade to final render
2. Preview is temporary visual feedback only
3. Final render provides full quality

### General Troubleshooting

#### Issue 7: Performance Degradation
**증상**: Overall slowdown compared to v2.0.0
**원인**: Possible configuration issue
**해결 방법**:
1. Ensure BVH is enabled (default in v2.1.0)
2. Check that scene file is well-formed
3. Verify no memory leaks with valgrind
4. Test with known-good scene files in `scenes/` directory

#### Issue 8: Input Lag During Interaction
**증상**: Controls feel sluggish or unresponsive
**원인**: System load or render queue backup
**해결 방법**:
1. Debouncing should prevent this - verify it's enabled
2. Reduce scene complexity if persistent
3. Check system resources (CPU, memory)

### FAQ Content Structure

**Recommended FAQ Sections**:
1. **성능 관련 질문** (Performance Questions)
   - Q: BVH란 무엇인가요? (What is BVH?)
   - Q: 성능이 느린 이유는? (Why is performance slow?)
   - Q: 메모리 사용량이 증가했어요 (Memory usage increased)

2. **렌더링 문제** (Rendering Issues)
   - Q: 검은 화면이 나옵니다 (Black screen appears)
   - Q: 프리뷰 모드가 동작하지 않아요 (Preview mode not working)
   - Q: 렌더링이 지연됩니다 (Rendering is delayed)

3. **기능 사용법** (Feature Usage)
   - Q: 디바운싱을 비활성화할 수 있나요? (Can I disable debouncing?)
   - Q: BVH를 끄고 싶어요 (How to disable BVH?)
   - Q: 프리뷰 품질을 높일 수 있나요? (Can I improve preview quality?)

---

## Implementation Checklist

**Research Complete**:
- ✅ BVH algorithm details (tree structure, AABB, traversal)
- ✅ Debounce state machine (4 states, 150ms timer, transitions)
- ✅ Performance metrics (best-effort, noted missing data)
- ✅ Korean terminology (established + new v2.1.0 terms)
- ✅ Troubleshooting scenarios (BVH, debouncing, preview mode)

**Ready for Phase 1**:
- All technical details researched and documented
- Korean translations standardized
- FAQ content structure defined
- Performance metrics strategy established

---

**Research Status**: ✅ Complete  
**Next Phase**: Phase 1 - Design & Contracts (data-model.md, contracts/)
