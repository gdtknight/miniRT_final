# Tasks: GitHub Wiki Update - Project Latest Changes (v2.1.0)

**Input**: Design documents from `/home/yoshin/work/miniRT/specs/019-wiki-update/`
**Prerequisites**: plan.md ✅, spec.md ✅, research.md ✅, data-model.md ✅, contracts/ ✅, quickstart.md ✅

**Tests**: This is a documentation-only feature. No automated tests required.

**Organization**: Tasks are grouped by user story to enable independent implementation and validation of each documentation update.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1, US2, US3, US4)
- All file paths are absolute starting from `/home/yoshin/work/miniRT/wiki/`

---

## Phase 1: Setup (Content Preparation)

**Purpose**: Prepare content sources and verify prerequisites before wiki updates

- [X] T001 Verify all design documents are complete in /home/yoshin/work/miniRT/specs/019-wiki-update/
- [X] T002 Verify CHANGELOG.md v2.1.0 content is accessible at /home/yoshin/work/miniRT/docs/project/CHANGELOG.md
- [X] T003 Review Korean terminology guide at /home/yoshin/work/miniRT/specs/019-wiki-update/contracts/korean-terminology.md
- [X] T004 Review content templates at /home/yoshin/work/miniRT/specs/019-wiki-update/contracts/content-templates.md

**Estimated Time**: 15 minutes

---

## Phase 2: Foundational (New Version History Page)

**Purpose**: Create version history page FIRST to provide link target for other pages

**⚠️ CRITICAL**: This page must exist before updating Home.md (which links to it)

- [X] T005 Create version history page at /home/yoshin/work/miniRT/wiki/버전-히스토리.md with v2.1.0 entry using Template 5
- [X] T006 Add v2.0.0 and earlier version placeholders in /home/yoshin/work/miniRT/wiki/버전-히스토리.md
- [X] T007 Add CHANGELOG.md reference link (../../docs/project/CHANGELOG.md) in /home/yoshin/work/miniRT/wiki/버전-히스토리.md
- [X] T008 Add version footer block to /home/yoshin/work/miniRT/wiki/버전-히스토리.md (Version: 2.1.0, Date: 2026-01)
- [X] T009 Validate markdown syntax and link to CHANGELOG.md in /home/yoshin/work/miniRT/wiki/버전-히스토리.md

**Estimated Time**: 30-40 minutes

**Checkpoint**: Version history page exists and can be linked from other pages

---

## Phase 3: User Story 1 - Core Feature Documentation Update (Priority: P1) 🎯 MVP

**Goal**: Document all v2.1.0 features (BVH acceleration, debounced rendering, preview mode) with accurate technical details and performance metrics

**Independent Test**: Read 프로그램-개요.md, 레이-트레이싱-알고리즘.md, and 인터랙티브-컨트롤.md to verify all v2.1.0 features are documented with clear descriptions, technical details, algorithms, data structures, and performance metrics (2-10x speedup for BVH, 90%+ render reduction for debouncing)

### Implementation for User Story 1

**Major Page Updates (Most Complex)**:

- [X] T010 [P] [US1] Add "성능 최적화" section to /home/yoshin/work/miniRT/wiki/프로그램-개요.md with BVH technical details from research.md RT-001
- [X] T011 [P] [US1] Add debounced rendering section to /home/yoshin/work/miniRT/wiki/프로그램-개요.md with 4-state machine details from research.md RT-002
- [X] T012 [P] [US1] Add preview mode section to /home/yoshin/work/miniRT/wiki/프로그램-개요.md with technical details from research.md
- [X] T013 [US1] Add "코드 품질" section to /home/yoshin/work/miniRT/wiki/프로그램-개요.md documenting 100% norminette compliance and zero memory leaks
- [X] T014 [US1] Update version footer in /home/yoshin/work/miniRT/wiki/프로그램-개요.md to 2.1.0 and 2026-01
- [X] T015 [P] [US1] Add "BVH 가속 구조" major section to /home/yoshin/work/miniRT/wiki/레이-트레이싱-알고리즘.md with algorithm details using Template 2
- [X] T016 [P] [US1] Document BVH tree construction algorithm in /home/yoshin/work/miniRT/wiki/레이-트레이싱-알고리즘.md (5 steps from research.md RT-001)
- [X] T017 [P] [US1] Document BVH tree traversal algorithm in /home/yoshin/work/miniRT/wiki/레이-트레이싱-알고리즘.md (ray-AABB intersection, early termination)
- [X] T018 [P] [US1] Add data structures section in /home/yoshin/work/miniRT/wiki/레이-트레이싱-알고리즘.md (t_bvh_node, t_aabb)
- [X] T019 [P] [US1] Add complexity analysis section in /home/yoshin/work/miniRT/wiki/레이-트레이싱-알고리즘.md (O(log n) vs O(n))
- [X] T020 [US1] Add performance metrics to BVH section in /home/yoshin/work/miniRT/wiki/레이-트레이싱-알고리즘.md (2-10x speedup, best-effort notation)
- [X] T021 [US1] Update version footer in /home/yoshin/work/miniRT/wiki/레이-트레이싱-알고리즘.md to 2.1.0 and 2026-01
- [X] T022 [P] [US1] Add "렌더링 동작" section to /home/yoshin/work/miniRT/wiki/인터랙티브-컨트롤.md with debouncing explanation
- [X] T023 [P] [US1] Document 4-state machine in /home/yoshin/work/miniRT/wiki/인터랙티브-컨트롤.md (IDLE → ACTIVE → PREVIEW → FINAL)
- [X] T024 [P] [US1] Add 150ms timer explanation in /home/yoshin/work/miniRT/wiki/인터랙티브-컨트롤.md
- [X] T025 [P] [US1] Add preview mode section to /home/yoshin/work/miniRT/wiki/인터랙티브-컨트롤.md using Template 3 (user-friendly language)
- [X] T026 [P] [US1] Document user experience benefits in /home/yoshin/work/miniRT/wiki/인터랙티브-컨트롤.md (smooth interaction, instant feedback)
- [X] T027 [US1] Update version footer in /home/yoshin/work/miniRT/wiki/인터랙티브-컨트롤.md to 2.1.0 and 2026-01

**Validation Checkpoints**:
- [X] T028 [US1] Verify all technical details in 프로그램-개요.md match research.md sources
- [X] T029 [US1] Verify BVH algorithm steps in 레이-트레이싱-알고리즘.md match research.md RT-001
- [X] T030 [US1] Verify Korean terminology consistency across all three major pages using korean-terminology.md
- [X] T031 [US1] Verify performance metrics use best-effort language and note missing data per Template 6

**Estimated Time**: 3-4 hours

**Checkpoint**: At this point, all v2.1.0 features are fully documented with technical details. Users can read 프로그램-개요.md → 레이-트레이싱-알고리즘.md → 인터랙티브-컨트롤.md to understand BVH, debouncing, and preview mode.

---

## Phase 4: User Story 2 - Version History Transparency (Priority: P2)

**Goal**: Provide users with a dedicated version history page that shows project evolution from 1.0.0 to 2.1.0 with clear change descriptions and link to detailed CHANGELOG.md

**Independent Test**: Navigate from Home.md → click version history link → verify 버전-히스토리.md shows all versions with v2.1.0 highlights and CHANGELOG.md link works

### Implementation for User Story 2

- [X] T032 [US2] Add navigation link to 버전-히스토리.md in /home/yoshin/work/miniRT/wiki/Home.md table of contents
- [X] T033 [US2] Add brief version 2.1.0 feature mention in /home/yoshin/work/miniRT/wiki/Home.md "주요 특징 → 성능" section
- [X] T034 [US2] Update version footer in /home/yoshin/work/miniRT/wiki/Home.md to 2.1.0 and 2026-01
- [X] T035 [US2] Verify link from Home.md to 버전-히스토리.md resolves correctly

**Estimated Time**: 15-20 minutes

**Checkpoint**: Users can now navigate from wiki home page to version history and see all v2.1.0 changes with link to detailed CHANGELOG.md

---

## Phase 5: User Story 3 - Performance Information Discovery (Priority: P3)

**Goal**: Help users self-service performance questions by updating FAQ with BVH acceleration, preview mode, and debouncing information including troubleshooting scenarios

**Independent Test**: Read FAQ.md and verify presence of performance questions (BVH benefits, performance optimization), rendering issue troubleshooting (black screen, delayed rendering, preview mode), and feature usage guidance with actionable solutions

### Implementation for User Story 3

**FAQ Page Updates**:

- [X] T036 [P] [US3] Add "성능 관련 질문" section to /home/yoshin/work/miniRT/wiki/FAQ.md
- [X] T037 [P] [US3] Add Q&A "BVH란 무엇인가요?" in /home/yoshin/work/miniRT/wiki/FAQ.md using Template 3
- [X] T038 [P] [US3] Add Q&A "성능이 느린 이유는?" in /home/yoshin/work/miniRT/wiki/FAQ.md with BVH troubleshooting from research.md RT-005 Issue 2
- [X] T039 [P] [US3] Add Q&A "메모리 사용량이 증가했어요" in /home/yoshin/work/miniRT/wiki/FAQ.md from research.md RT-005 Issue 3
- [X] T040 [P] [US3] Add "렌더링 문제" section to /home/yoshin/work/miniRT/wiki/FAQ.md
- [X] T041 [P] [US3] Add Q&A "화면이 검은색으로 나옵니다" in /home/yoshin/work/miniRT/wiki/FAQ.md using Template 4, from research.md RT-005 Issue 1
- [X] T042 [P] [US3] Add Q&A "렌더링이 지연됩니다" in /home/yoshin/work/miniRT/wiki/FAQ.md explaining debounce behavior from research.md RT-005 Issue 4
- [X] T043 [P] [US3] Add Q&A "프리뷰 모드가 보이지 않습니다" in /home/yoshin/work/miniRT/wiki/FAQ.md from research.md RT-005 Issue 5
- [X] T044 [P] [US3] Add "기능 사용법" section to /home/yoshin/work/miniRT/wiki/FAQ.md
- [X] T045 [P] [US3] Add Q&A "프리뷰 모드는 무엇인가요?" in /home/yoshin/work/miniRT/wiki/FAQ.md with user-friendly explanation
- [X] T046 [US3] Update version footer in /home/yoshin/work/miniRT/wiki/FAQ.md to 2.1.0 and 2026-01

**Validation Checkpoints**:
- [X] T047 [US3] Verify FAQ has at least 6 new Q&A entries covering BVH, debouncing, and preview mode
- [X] T048 [US3] Verify all troubleshooting solutions are actionable with numbered steps per Template 4
- [X] T049 [US3] Verify cross-references to other wiki pages use correct link format per style-guide.md

**Estimated Time**: 1-1.5 hours

**Checkpoint**: Users encountering performance issues can find solutions in FAQ with references to new features and clear troubleshooting steps

---

## Phase 6: User Story 4 - Consistent Korean Documentation Style (Priority: P2)

**Goal**: Ensure all wiki pages use consistent Korean technical terms, markdown formatting, and section structures for professional appearance and improved comprehension

**Independent Test**: Read all 10 wiki pages and verify consistent Korean terms (e.g., "렌더링", "BVH 가속 구조"), uniform markdown formatting, consistent heading hierarchy, and uniform version/date metadata in footers

### Implementation for User Story 4

**Minor Page Updates (Version & Date Only)**:

- [X] T050 [P] [US4] Update version footer in /home/yoshin/work/miniRT/wiki/README.md to 2.1.0 and 2026-01
- [X] T051 [P] [US4] Add brief v2.1.0 feature mention in /home/yoshin/work/miniRT/wiki/README.md
- [X] T052 [P] [US4] Update version footer in /home/yoshin/work/miniRT/wiki/씬-파일-형식.md to 2.1.0 and 2026-01
- [X] T053 [P] [US4] Update version footer in /home/yoshin/work/miniRT/wiki/조명-시스템.md to 2.1.0 and 2026-01
- [X] T054 [P] [US4] Update version footer in /home/yoshin/work/miniRT/wiki/빠른-시작-가이드.md to 2.1.0 and 2026-01

**Cross-Page Consistency Validation**:

- [X] T055 [US4] Validate Korean terminology consistency across all 10 wiki pages using /home/yoshin/work/miniRT/specs/019-wiki-update/contracts/korean-terminology.md
- [X] T056 [US4] Validate markdown formatting consistency using /home/yoshin/work/miniRT/specs/019-wiki-update/contracts/style-guide.md
- [X] T057 [US4] Verify all pages use consistent heading hierarchy (no H1→H3 jumps)
- [X] T058 [US4] Verify all version references are exactly "2.1.0" across all pages
- [X] T059 [US4] Verify all date references are "2026-01" or later across all pages
- [X] T060 [US4] Verify all footer blocks follow Template 1 format exactly

**Estimated Time**: 30 minutes

**Checkpoint**: All wiki pages now have consistent styling, terminology, and metadata. Version 2.1.0 and date 2026-01 appear uniformly across all pages.

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Final validation, link checking, and quality assurance across all updated pages

**Link Validation**:

- [X] T061 [P] Validate all internal wiki links resolve correctly using grep pattern search in /home/yoshin/work/miniRT/wiki/
- [X] T062 [P] Validate CHANGELOG.md external link (../../docs/project/CHANGELOG.md) from 버전-히스토리.md
- [X] T063 [P] Validate anchor links to BVH section (#bvh-가속-구조) resolve correctly
- [X] T064 Verify no broken links or 404s in any of the 10 updated wiki pages

**Korean Language Quality**:

- [X] T065 [P] Review Korean grammar and spacing (띄어쓰기) in all major updated sections
- [X] T066 [P] Verify formal tone (합니다체) consistency across all pages
- [X] T067 [P] Check particle usage correctness (은/는, 이/가, 을/를) in new content
- [X] T068 Verify English technical terms appear in parentheses on first mention per korean-terminology.md

**Content Quality Verification**:

- [X] T069 [P] Verify BVH technical details match source code documentation
- [X] T070 [P] Verify debounce state machine details match source code implementation
- [X] T071 [P] Verify performance metrics match CHANGELOG.md v2.1.0 section
- [X] T072 Verify all best-effort performance data includes notation about missing details per Template 6

**Markdown Syntax**:

- [X] T073 [P] Verify all code blocks have language specifiers (```c, ```bash, ```rt)
- [X] T074 [P] Verify all tables are properly formatted with aligned pipes
- [X] T075 [P] Verify all lists use consistent dash markers (-) not asterisks
- [X] T076 Check markdown renders correctly without syntax errors in all pages

**Success Criteria Validation** (from spec.md):

- [X] T077 Verify SC-001: All wiki pages reference version 2.1.0 and show January 2026 dates
- [X] T078 Verify SC-002: v2.1.0 features documented within 2 clicks from Home.md
- [X] T079 Verify SC-003: Performance documentation includes best-effort benchmark data with notation
- [X] T080 Verify SC-004: Version history page lists all versions 1.0.0 to 2.1.0
- [X] T081 Verify SC-005: FAQ answers at least 3 performance questions with troubleshooting scenarios
- [X] T082 Verify SC-006: Korean terminology consistent across all updated pages
- [X] T083 Verify SC-007: Zero broken internal wiki links after updates
- [X] T084 Verify SC-008: All markdown files render correctly without formatting errors

**Quickstart Workflow Validation**:

- [X] T085 Execute workflow validation from /home/yoshin/work/miniRT/specs/019-wiki-update/quickstart.md Steps 7-8
- [X] T086 Complete all quality gate checks from quickstart.md

**Estimated Time**: 1-1.5 hours

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup - creates 버전-히스토리.md before other pages link to it
- **User Story 1 (Phase 3)**: Can start after Foundational - major page updates (프로그램-개요.md, 레이-트레이싱-알고리즘.md, 인터랙티브-컨트롤.md)
- **User Story 2 (Phase 4)**: Depends on Foundational Phase 2 (needs 버전-히스토리.md to exist) - updates Home.md with link
- **User Story 3 (Phase 5)**: Independent of other user stories - can run parallel to US1/US2
- **User Story 4 (Phase 6)**: Should run after US1-US3 complete to validate consistency across all changes
- **Polish (Phase 7)**: Depends on all user stories complete - final validation and QA

### User Story Dependencies

- **User Story 1 (P1)**: Depends only on Foundational (버전-히스토리.md exists) - No dependencies on other stories
- **User Story 2 (P2)**: Depends on Foundational (버전-히스토리.md exists) - Can run parallel to US1 and US3
- **User Story 3 (P3)**: Independent - Can run parallel to US1 and US2
- **User Story 4 (P2)**: Should run AFTER US1, US2, US3 to validate consistency - Acts as quality gate

### Optimal Execution Sequence

**Sequential (Single Person)**:
1. Phase 1: Setup (15 min)
2. Phase 2: Foundational - Create 버전-히스토리.md (40 min)
3. Phase 3: User Story 1 - Major pages (3-4 hours) ← MVP checkpoint after this
4. Phase 4: User Story 2 - Home.md link (20 min)
5. Phase 5: User Story 3 - FAQ updates (1.5 hours)
6. Phase 6: User Story 4 - Minor pages + consistency (30 min)
7. Phase 7: Polish - Final validation (1.5 hours)

**Total Estimated Time**: 6-8 hours

**Parallel Opportunities (Multiple Team Members)**:

After Phase 2 completes:
- Person A: User Story 1 (T010-T031) - Major technical pages
- Person B: User Story 3 (T036-T049) - FAQ updates
- Person C: User Story 2 (T032-T035) + User Story 4 minor updates (T050-T054)
- Final: All validate together in Phase 6 & 7

### Parallel Execution Examples

**Within User Story 1 (Major Pages)**:
```bash
# All these tasks can run simultaneously (different files):
T010 [P] [US1] Add "성능 최적화" section to 프로그램-개요.md
T015 [P] [US1] Add "BVH 가속 구조" section to 레이-트레이싱-알고리즘.md
T022 [P] [US1] Add "렌더링 동작" section to 인터랙티브-컨트롤.md
```

**Within User Story 3 (FAQ Sections)**:
```bash
# All Q&A entries can be added in parallel:
T037 [P] [US3] Add Q&A "BVH란 무엇인가요?"
T038 [P] [US3] Add Q&A "성능이 느린 이유는?"
T039 [P] [US3] Add Q&A "메모리 사용량이 증가했어요"
T041 [P] [US3] Add Q&A "화면이 검은색으로 나옵니다"
```

**Within User Story 4 (Minor Pages)**:
```bash
# All version footer updates can run in parallel:
T050 [P] [US4] Update version footer in README.md
T052 [P] [US4] Update version footer in 씬-파일-형식.md
T053 [P] [US4] Update version footer in 조명-시스템.md
T054 [P] [US4] Update version footer in 빠른-시작-가이드.md
```

---

## Implementation Strategy

### MVP First (User Stories 1 + 2 Only)

**Minimum Viable Documentation Update**:
1. Complete Phase 1: Setup (15 min)
2. Complete Phase 2: Foundational - 버전-히스토리.md (40 min)
3. Complete Phase 3: User Story 1 - Major pages (3-4 hours)
4. Complete Phase 4: User Story 2 - Home.md link (20 min)
5. **STOP and VALIDATE**: Read through 프로그램-개요.md, 레이-트레이싱-알고리즘.md, 인터랙티브-컨트롤.md, 버전-히스토리.md, Home.md
6. Verify users can find v2.1.0 features within 2 clicks
7. Deploy/commit if ready

**MVP Delivers**:
- ✅ All v2.1.0 features documented with technical details
- ✅ Version history page accessible from home
- ✅ Performance metrics included (best-effort)
- ✅ BVH algorithm fully explained
- ✅ Debouncing and preview mode documented

**MVP Does NOT Include**:
- ❌ FAQ troubleshooting (User Story 3)
- ❌ Minor page updates (User Story 4)
- ❌ Complete consistency validation (Phase 7)

### Incremental Delivery

**Milestone 1 - Foundation Ready** (Phases 1-2):
- Setup complete
- 버전-히스토리.md created
- Can start all user story work in parallel

**Milestone 2 - Core Features Documented** (Phase 3):
- User Story 1 complete
- All major technical pages updated
- BVH, debouncing, preview mode fully documented
- **CHECKPOINT**: Can ship MVP here

**Milestone 3 - Navigation Complete** (Phase 4):
- User Story 2 complete
- Home.md links to version history
- Users can discover changes easily

**Milestone 4 - Self-Service Support** (Phase 5):
- User Story 3 complete
- FAQ has troubleshooting guidance
- Performance questions answered
- **CHECKPOINT**: Can ship full feature here

**Milestone 5 - Production Quality** (Phases 6-7):
- User Story 4 complete
- All pages consistent
- Full validation passed
- **SHIP**: Complete wiki update ready

---

## Notes

- [P] tasks target different files, can run in parallel
- [Story] label maps task to specific user story for traceability
- Each user story can be validated independently
- Stop at any checkpoint to validate story independently
- All file paths are absolute for clarity
- Korean terminology guide and content templates must be followed strictly
- Best-effort approach for performance metrics - note missing data explicitly
- No source code changes - documentation only

---

## Success Metrics

**From spec.md Success Criteria**:

✅ **SC-001**: All wiki pages reference version 2.1.0 and show January 2026 update dates
- Verified by: T077

✅ **SC-002**: Users can find documentation for all v2.1.0 features within 2 clicks from the wiki home page
- Verified by: T078

✅ **SC-003**: Performance documentation includes best-effort benchmark data from CHANGELOG.md with notation of missing details
- Verified by: T079

✅ **SC-004**: Version history page lists all versions from 1.0.0 to 2.1.0 with complete change descriptions
- Verified by: T080

✅ **SC-005**: FAQ answers at least 3 performance-related questions with references to new features and at least 1 troubleshooting scenario per major feature
- Verified by: T081

✅ **SC-006**: All Korean technical terminology remains consistent across updated pages
- Verified by: T082

✅ **SC-007**: Zero broken internal wiki links after updates
- Verified by: T083

✅ **SC-008**: All markdown files render correctly without formatting errors
- Verified by: T084

**Total Tasks**: 86 tasks
- Setup: 4 tasks
- Foundational: 5 tasks
- User Story 1: 22 tasks (including 4 validation tasks)
- User Story 2: 4 tasks
- User Story 3: 14 tasks (including 3 validation tasks)
- User Story 4: 11 tasks (including 6 validation tasks)
- Polish & QA: 26 tasks

**Parallelization Potential**: 
- Setup phase: 0 parallel tasks
- Foundational phase: 0 parallel tasks (sequential)
- User Story 1: 15 parallel tasks (T010-T012, T015-T020, T022-T026 within different files)
- User Story 2: 0 parallel tasks (small, sequential)
- User Story 3: 9 parallel tasks (T036-T045 all in FAQ.md but different sections)
- User Story 4: 5 parallel tasks (T050-T054 different files)
- Polish: 15 parallel tasks (validation tasks across different files)

**Total Parallelizable**: 44 out of 86 tasks (51%)

---

**Tasks Status**: ✅ Complete and ready for execution
**Estimated Total Time**: 6-8 hours (sequential), 3-4 hours (with 2-3 parallel contributors)
**MVP Time**: 4-5 hours (Phases 1-4 only)
