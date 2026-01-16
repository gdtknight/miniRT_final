# Feature Specification: GitHub Wiki Update - Project Latest Changes

**Feature Branch**: `019-wiki-update`  
**Created**: 2026-01-16  
**Status**: Draft  
**Input**: User description: "GitHub Wiki 업데이트 - 프로젝트 최신 변경사항 반영"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Core Feature Documentation Update (Priority: P1)

A user visits the wiki to understand the latest miniRT features (v2.1.0), including BVH acceleration, debounced rendering, and preview mode. They need accurate, up-to-date information about these new capabilities.

**Why this priority**: This is the primary value - users cannot learn about new features without updated documentation. Delivers immediate value by documenting the most significant changes.

**Independent Test**: Can be fully tested by reading the updated wiki pages and verifying all v2.1.0 features are documented with clear descriptions and usage examples.

**Acceptance Scenarios**:

1. **Given** a user reads the "프로그램-개요.md" page, **When** they look for performance features, **Then** they see BVH acceleration documented with performance metrics (2-10x speedup)
2. **Given** a user reads the "인터랙티브-컨트롤.md" page, **When** they look for rendering controls, **Then** they find information about preview mode and debounced rendering behavior
3. **Given** a user reads any wiki page, **When** they check the version info, **Then** it shows version 2.1.0 and update date January 2026

---

### User Story 2 - Version History Transparency (Priority: P2)

A user wants to understand what changed between versions and review the project's evolution over time. They need a changelog or version history page accessible from the wiki.

**Why this priority**: Version transparency builds trust and helps users understand upgrade implications. Secondary to core feature docs but valuable for project credibility.

**Independent Test**: Can be tested by navigating the wiki and finding a dedicated changelog/version history page that lists all versions from 1.0.0 to 2.1.0 with clear change descriptions.

**Acceptance Scenarios**:

1. **Given** a user visits the wiki home page, **When** they look for version information, **Then** they find a link to a new wiki page (e.g., "버전-히스토리.md") that provides highlights/summary and references CHANGELOG.md
2. **Given** a user reads the version history page, **When** they review v2.1.0 changes, **Then** they see all major features listed: BVH, debouncing, preview mode, norminette compliance
3. **Given** a user compares versions, **When** they read the changelog, **Then** each version clearly shows Added/Changed/Fixed categories

---

### User Story 3 - Performance Information Discovery (Priority: P3)

A user encounters rendering performance issues and visits the FAQ to find solutions. They need updated information about BVH acceleration, preview mode, and performance optimization techniques.

**Why this priority**: Helps users self-service performance questions, reducing support burden. Lower priority as it's a supplement to core docs.

**Independent Test**: Can be tested by reading the FAQ page and finding answers to common performance questions with references to new features.

**Acceptance Scenarios**:

1. **Given** a user reads FAQ.md, **When** they search for performance questions, **Then** they find answers referencing BVH acceleration and its benefits
2. **Given** a user experiences slow rendering during interaction, **When** they check the FAQ, **Then** they find an explanation of debounced rendering and preview mode behavior
3. **Given** a user wants to optimize complex scenes, **When** they read performance tips, **Then** they see recommendations about BVH usage and scene complexity guidelines

---

### User Story 4 - Consistent Korean Documentation Style (Priority: P2)

A Korean-speaking user reads multiple wiki pages and expects consistent terminology, formatting, and writing style throughout all documentation.

**Why this priority**: Consistency improves comprehension and professional appearance. Affects all pages but doesn't add new information.

**Independent Test**: Can be tested by reading all wiki pages and verifying consistent use of Korean technical terms, markdown formatting, and section structures.

**Acceptance Scenarios**:

1. **Given** a user reads multiple wiki pages, **When** they encounter technical terms, **Then** the same Korean translations are used consistently (e.g., "렌더링", "광선 추적")
2. **Given** a user navigates between pages, **When** they review formatting, **Then** all pages follow the same heading hierarchy and markdown conventions
3. **Given** a user checks page footers, **When** they look at metadata, **Then** all pages show consistent version and date information

---

### Edge Cases

- What happens when a wiki page references a feature that was removed or changed in v2.1.0? (Ensure all references are updated or removed)
- How do we handle Korean vs English terminology consistency? (Maintain existing Korean style with English terms in parentheses where appropriate)
- What if future versions add breaking changes? (Structure documentation to clearly indicate version-specific behavior)

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Wiki MUST document all v2.1.0 features including BVH acceleration (with implementation details: spatial subdivision tree structure, AABB bounding volumes, recursive tree traversal algorithm), debounced rendering (150ms timer implementation, state machine transitions), preview mode (low-resolution preview rendering before final render), and unified object identifiers
- **FR-002**: Wiki MUST update performance metrics to reflect BVH acceleration with best-effort benchmark data from CHANGELOG.md; if full test scene specifications or timing breakdowns are unavailable, document available metrics and note any missing details (target: 2-10x speedup on complex scenes); include algorithmic complexity (O(log n) traversal vs O(n) linear)
- **FR-003**: Wiki MUST document the 4-state rendering state machine (IDLE → ACTIVE → PREVIEW → FINAL) with technical details: state transition triggers, timers, and render quality differences between preview and final states
- **FR-004**: Wiki MUST include debounce timing information (150ms debounce timer)
- **FR-005**: Wiki MUST update "프로그램-개요.md" with latest features and capabilities
- **FR-006**: Wiki MUST update "인터랙티브-컨트롤.md" with new rendering control behaviors
- **FR-007**: Wiki MUST create a new wiki page (e.g., "버전-히스토리.md") that links to or references CHANGELOG.md with highlights/summary of each version
- **FR-008**: Wiki MUST update FAQ with performance-related answers referencing new features, including common error scenarios and troubleshooting tips for BVH, preview mode, and debounced rendering
- **FR-009**: Wiki MUST maintain Korean language style and terminology consistency across all pages
- **FR-010**: Wiki MUST update version numbers from 2.0.0 to 2.1.0 across all pages
- **FR-011**: Wiki MUST update "Last Updated" dates to January 2026
- **FR-012**: Wiki MUST preserve existing wiki structure (no file deletions or major reorganizations)
- **FR-013**: Wiki MUST maintain markdown formatting compliance and readability
- **FR-014**: Wiki MUST include code examples or usage descriptions for new features where applicable, with technical implementation details including data structures (BVH tree node structure), algorithms (tree construction, traversal), and configuration parameters (debounce timing, preview resolution)
- **FR-015**: Wiki MUST document BVH visualization feature (`--bvh-vis` flag) if relevant to users, including technical details about visual debug rendering of bounding volumes and tree structure
- **FR-016**: Wiki MUST clarify 100% norminette compliance achievement and prohibited function removal

### Key Entities

- **Wiki Pages**: Existing markdown files in wiki directory that need updates (Home.md, 프로그램-개요.md, 인터랙티브-컨트롤.md, FAQ.md, etc.)
- **Version Information**: Version numbers (2.1.0), dates (2026-01-15), and changelog entries from CHANGELOG.md
- **Feature Descriptions**: BVH acceleration (including spatial partitioning algorithm, AABB construction, tree traversal implementation), debounced rendering (timer-based state machine, event handling), preview mode (resolution reduction strategy, quality vs performance tradeoff), unified identifiers, norminette compliance
- **Performance Metrics**: Speedup ratios (2-10x), debounce timing (150ms), render states (4-state machine)

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: All wiki pages reference version 2.1.0 and show January 2026 update dates
- **SC-002**: Users can find documentation for all v2.1.0 features within 2 clicks from the wiki home page
- **SC-003**: Performance documentation includes best-effort benchmark data from CHANGELOG.md; full metrics preferred (test scene specifications and timing breakdowns, e.g., scene X: before 5.2s, after 0.8s with BVH), but partial data is acceptable with notation of missing details
- **SC-004**: Version history page lists all versions from 1.0.0 to 2.1.0 with complete change descriptions
- **SC-005**: FAQ answers at least 3 performance-related questions with references to new features, including at least 1 troubleshooting scenario per major feature (BVH, preview mode, debounced rendering)
- **SC-006**: All Korean technical terminology remains consistent across updated pages
- **SC-007**: Zero broken internal wiki links after updates
- **SC-008**: All markdown files render correctly without formatting errors

## Assumptions

- The wiki directory contains the authoritative documentation that will be pushed to GitHub Wiki
- CHANGELOG.md in docs/project/ contains accurate v2.1.0 information
- Existing wiki pages use Korean as the primary language with English technical terms
- Version 2.1.0 release date is 2026-01-15 based on CHANGELOG.md
- BVH acceleration is enabled by default in v2.1.0
- Users expect Korean-language documentation given existing page names
- Wiki updates do not require source code changes, only documentation updates

## Dependencies

- Access to docs/project/CHANGELOG.md for accurate version information
- Access to existing wiki markdown files in wiki/ directory
- Understanding of v2.1.0 features from CHANGELOG.md
- Preservation of existing Korean terminology and style conventions

## Out of Scope

- Creating entirely new wiki pages beyond version history (unless explicitly needed)
- Translating documentation to other languages
- Updating source code documentation or comments
- Creating visual diagrams or images for wiki pages
- Reorganizing wiki page structure or navigation
- Adding features not listed in v2.1.0 CHANGELOG.md
- Documenting internal implementation details beyond what's necessary for user understanding (focus on user-facing aspects of algorithms and data structures, not internal helper functions or memory management specifics)

## Clarifications

### Session 2026-01-16

- Q: How should version history be presented in the wiki? → A: B - Create a new wiki page linking to or referencing CHANGELOG.md with highlights/summary
- Q: What level of detail should BVH performance metrics include in the wiki? → A: A - Full benchmark data with test scene specs and timing breakdowns
- Q: If CHANGELOG.md lacks full benchmark data, how should wiki documentation proceed? → A: A - Best effort, note any missing metrics and proceed with available data
- Q: Should the FAQ include troubleshooting guidance for new features? → A: A - Yes, include common error scenarios and troubleshooting tips for new features
- Q: What level of technical detail should be included when documenting new features like BVH, preview mode, and debounced rendering? → A: A - Full technical detail with implementation specifics (algorithms, data structures)

## Risks

- **Risk**: Inconsistent terminology between old and new content
  - **Mitigation**: Review all Korean technical terms before finalizing updates
  
- **Risk**: Outdated performance metrics if actual performance differs
  - **Mitigation**: Use metrics directly from CHANGELOG.md which reflects actual implementation
  
- **Risk**: Broken links if page names or sections are renamed
  - **Mitigation**: Verify all internal links after updates
