# Implementation Plan: GitHub Wiki Update - Project Latest Changes (v2.1.0)

**Branch**: `019-wiki-update` | **Date**: 2026-01-16 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/019-wiki-update/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Update GitHub Wiki to document miniRT v2.1.0 features including BVH acceleration (2-10x speedup), debounced rendering (150ms, 4-state machine), preview mode, render state tracking, 100% norminette compliance, and unified object identifiers. Create version history page, update all existing wiki pages with technical details, enhance FAQ with troubleshooting guidance, and maintain Korean documentation consistency.

## Technical Context

**Language/Version**: Markdown (GitHub Wiki flavor)  
**Primary Dependencies**: None (pure markdown documentation)  
**Storage**: File-based markdown files in `/home/yoshin/work/miniRT/wiki/`  
**Testing**: Manual review for accuracy, link validation, markdown rendering check  
**Target Platform**: GitHub Wiki (browser-based)
**Project Type**: Documentation update (non-code)  
**Performance Goals**: N/A (documentation)  
**Constraints**: Maintain Korean language consistency, preserve existing wiki structure, no file deletions  
**Scale/Scope**: 10 existing wiki pages to update, 1 new version history page, comprehensive technical documentation

**Content Sources**:
- Version information: `/docs/project/CHANGELOG.md` (v2.1.0 released 2026-01-15)
- Technical implementation: Source code comments, BVH algorithm documentation
- Performance metrics: CHANGELOG.md reports 2-10x speedup, 90%+ render reduction
- Feature details: Specs 014-018 (BVH visualization, unified IDs, norminette, BVH acceleration, debounced rendering)

**Documentation Requirements**:
- Full technical detail: algorithms (BVH spatial subdivision, AABB traversal), data structures (BVH tree nodes), implementation specifics
- Performance metrics: Best-effort from CHANGELOG (2-10x speedup, 90%+ reduction in unnecessary renders)
- Troubleshooting: Common error scenarios and solutions for BVH, preview mode, debounced rendering
- Korean technical terminology: Maintain existing style (e.g., "렌더링", "광선 추적", "가속 구조")

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Project Structure Standards (Principle I)**:
- ✅ Wiki files located in `wiki/` directory (proper documentation organization)
- ✅ Documentation follows clear, descriptive naming (Korean wiki page names)
- ✅ No deprecated/backup files will be created (direct updates only)
- ✅ Active documentation with clear purpose

**Documentation and Wiki Synchronization (Principle III)**:
- ✅ Source docs in `docs/` are single source of truth (CHANGELOG.md)
- ✅ Wiki content will match release version 2.1.0
- ✅ Wiki pages will display version 2.1.0 and January 2026 dates
- ✅ Content requirements: Korean documentation with version information

**Bilingual Specification Management (Principle VI)**:
- ✅ Korean documentation maintained in `wiki/` (primary language)
- ✅ Existing Korean terminology preserved and used consistently
- ✅ No translation work required (already Korean)

**42 School Constraints Check (Principle VII)**:
- ✅ N/A - Documentation update only, no code changes
- ✅ Documents allowed optimization techniques (BVH = algorithmic optimization)
- ✅ No prohibited functions documented as recommended practices
- ✅ Compliance maintained in documentation narrative

**Quality Assurance**:
- ✅ Markdown formatting compliance required
- ✅ Link validation required (internal wiki links)
- ✅ Korean terminology consistency required
- ✅ Version consistency required (2.1.0 across all pages)

**GATE STATUS**: ✅ PASSED - All constitution principles satisfied. Documentation-only change with no structural violations.

## Project Structure

### Documentation (this feature)

```text
specs/019-wiki-update/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output - Technical details research
├── data-model.md        # Phase 1 output - Wiki content structure model
├── quickstart.md        # Phase 1 output - Wiki update workflow guide
├── contracts/           # Phase 1 output - Content templates and style guide
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Wiki Files (repository wiki directory)

```text
wiki/
├── Home.md                         # UPDATE: Add v2.1.0 features, version history link
├── README.md                       # UPDATE: Version, features
├── FAQ.md                          # UPDATE: Performance Q&A, troubleshooting
├── 프로그램-개요.md                  # UPDATE: BVH, debouncing, performance metrics
├── 빠른-시작-가이드.md               # UPDATE: Version reference
├── 씬-파일-형식.md                   # UPDATE: Version reference (minimal changes)
├── 레이-트레이싱-알고리즘.md         # UPDATE: BVH algorithm section
├── 조명-시스템.md                   # UPDATE: Version reference (minimal changes)
├── 인터랙티브-컨트롤.md              # UPDATE: Preview mode, debounced rendering behavior
└── 버전-히스토리.md                 # CREATE: New version history page
```

**Structure Decision**: Documentation-only update using existing wiki directory structure. No source code changes required. All updates maintain existing Korean language style and markdown formatting. New version history page added to document project evolution from v1.0.0 to v2.1.0.

## Complexity Tracking

> **No violations** - Documentation-only change with no complexity concerns.

---

# Phase 0: Outline & Research

## Research Tasks

### RT-001: BVH Algorithm Implementation Details
**Question**: What are the specific technical details of the BVH implementation that need documentation?
**Sources**: Source code (`src/bvh/`, `includes/bvh.h`), CHANGELOG.md, Spec 017
**Required for**: Technical accuracy in 레이-트레이싱-알고리즘.md and 프로그램-개요.md

### RT-002: Debounced Rendering State Machine
**Question**: What are the exact state transitions, triggers, and timing of the 4-state rendering machine?
**Sources**: Source code (`src/render/debounce.c`, `src/render/state.c`), CHANGELOG.md, Spec 018
**Required for**: Technical documentation in 인터랙티브-컨트롤.md

### RT-003: Performance Benchmark Data
**Question**: What specific performance metrics are available from testing? Which test scenes were used?
**Sources**: CHANGELOG.md, test logs if available, Spec 017
**Required for**: Accurate performance claims in 프로그램-개요.md and FAQ.md

### RT-004: Korean Technical Terminology Mapping
**Question**: What are the established Korean translations for new technical terms (BVH, debouncing, preview mode)?
**Sources**: Existing wiki pages, Korean technical glossaries
**Required for**: Consistent terminology across all updated pages

### RT-005: Common Error Scenarios
**Question**: What are typical user errors or issues with BVH, preview mode, and debounced rendering?
**Sources**: Issue reports, testing feedback, code error handling
**Required for**: Troubleshooting section in FAQ.md

## Research Deliverable

Output file: `specs/019-wiki-update/research.md`

Expected structure:
- BVH technical details (tree structure, AABB implementation, traversal algorithm)
- Debounced rendering mechanics (state machine diagram, timing specs)
- Performance benchmark summary (best-effort data with test scene details)
- Korean terminology guide (new terms with translations)
- Troubleshooting scenarios (common errors and solutions)

---

# Phase 1: Design & Contracts

## Data Model: Wiki Content Structure

### Entity: WikiPage
**Attributes**:
- `file_name` (string): Markdown filename (e.g., "Home.md")
- `title` (string): Korean page title
- `version` (string): "2.1.0"
- `last_updated` (date): "2026-01"
- `sections` (list): Ordered list of content sections
- `update_type` (enum): CREATE | MINOR_UPDATE | MAJOR_UPDATE

**Relationships**:
- WikiPage HAS_MANY ContentSection
- WikiPage LINKS_TO other WikiPages

### Entity: ContentSection
**Attributes**:
- `heading` (string): Section title in Korean
- `content_type` (enum): FEATURE | PERFORMANCE | USAGE | TROUBLESHOOTING
- `technical_depth` (enum): HIGH | MEDIUM | LOW
- `markdown_content` (string): Section body

**Validation Rules**:
- All version references MUST be "2.1.0"
- All dates MUST be "2026-01" or "2026-01-15"
- Korean terminology MUST be consistent across pages
- Internal links MUST point to existing wiki pages
- Code blocks MUST use proper markdown syntax

### Entity: FeatureDocumentation
**Attributes**:
- `feature_name` (string): e.g., "BVH Acceleration"
- `korean_name` (string): e.g., "BVH 가속 구조"
- `spec_reference` (string): e.g., "Spec 017"
- `technical_details` (object): algorithms, data structures, parameters
- `performance_impact` (string): Benchmark data
- `usage_examples` (list): Code or configuration examples
- `troubleshooting` (list): Common issues and solutions

**Features to Document**:
1. BVH Acceleration
2. Debounced Rendering
3. Preview Mode
4. Render State Tracking (is_rendering flag)
5. Unified Object Identifiers
6. 100% Norminette Compliance

## Contracts: Content Templates

### Contract 1: Version Information Block
**Template** (Korean):
```markdown
---

**Last Updated**: 2026-01  
**Version**: 2.1.0
```

**Usage**: Footer of every wiki page

### Contract 2: Feature Description Pattern
**Template** (Korean):
```markdown
### [기능명] ([Feature Name])

**개요**: [1-2 sentence description in Korean]

**기술 세부사항**:
- **[Technical aspect 1]**: [Details]
- **[Technical aspect 2]**: [Details]

**성능**: [Performance metrics if applicable]

**사용법**: [Usage examples or configuration]
```

### Contract 3: Troubleshooting Entry
**Template** (Korean):
```markdown
#### 문제: [Problem description]

**증상**: [What user sees]

**원인**: [Why it happens]

**해결 방법**:
1. [Step 1]
2. [Step 2]
```

### Contract 4: Version History Entry
**Template** (Korean):
```markdown
## [버전 번호] - YYYY-MM-DD

### 주요 변경사항
- [Highlight 1]
- [Highlight 2]

### 세부사항
상세한 변경 내역은 [CHANGELOG.md](../docs/project/CHANGELOG.md)를 참조하세요.
```

## Design Artifacts

### Output: data-model.md
Complete data model specification including:
- Entity definitions (WikiPage, ContentSection, FeatureDocumentation)
- Validation rules
- State transitions (N/A for static documentation)
- Relationships between entities

### Output: contracts/
Directory containing:
- `content-templates.md`: All content patterns and examples
- `korean-terminology.md`: Technical term translations and usage
- `style-guide.md`: Markdown formatting rules, heading hierarchy
- `link-reference.md`: Standard internal link patterns

### Output: quickstart.md
Wiki update workflow guide:
1. Research phase completion verification
2. Content preparation (gather version info, technical details)
3. Page-by-page update sequence (order matters for link validation)
4. Quality checks (terminology, links, formatting)
5. Final review checklist

---

# Phase 2: Task Breakdown

**Note**: Task breakdown will be generated by `/speckit.tasks` command (not part of `/speckit.plan`).

Task structure will organize work by:
- **Stream 1**: Content research and preparation
- **Stream 2**: Existing page updates (Home, 프로그램-개요, FAQ, etc.)
- **Stream 3**: New page creation (버전-히스토리)
- **Stream 4**: Quality assurance (link checking, terminology review)

---

# Implementation Notes

## Update Sequence

Recommended order to avoid broken links:

1. **Create new page first**: 버전-히스토리.md (no dependencies)
2. **Update Home.md**: Add link to version history
3. **Update 프로그램-개요.md**: Major technical content
4. **Update 레이-트레이싱-알고리즘.md**: BVH algorithm details
5. **Update 인터랙티브-컨트롤.md**: Debouncing and preview mode
6. **Update FAQ.md**: Performance and troubleshooting
7. **Update remaining pages**: Version numbers and minor references

## Quality Gates

Before considering any page complete:
- ✅ Version is "2.1.0"
- ✅ Date is "2026-01" or later
- ✅ All internal links resolve correctly
- ✅ Korean terminology matches existing style
- ✅ Markdown renders without errors
- ✅ Technical details are accurate (verified against source)

## Risk Mitigation

**Risk**: Incomplete performance data in CHANGELOG
- **Mitigation**: Use best-effort approach, note missing metrics explicitly

**Risk**: Inconsistent Korean terminology
- **Mitigation**: Create terminology guide in Phase 1, review all changes

**Risk**: Broken links after updates
- **Mitigation**: Validate all links before and after each update

---

# Agent Context Update

After Phase 1 design completion, run:
```bash
.specify/scripts/bash/update-agent-context.sh copilot
```

This will update agent-specific context with:
- Wiki documentation patterns
- Korean technical terminology
- Content structure models
- Quality validation requirements

---

**Plan Status**: Ready for Phase 0 (Research)  
**Next Command**: Generate research.md to resolve all technical details
