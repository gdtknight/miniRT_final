# Feature 016: Complete Compliance Refactoring

**Status**: Planning Complete ✅  
**Branch**: `016-compliance-refactoring`  
**Priority**: P1 (Critical - Required for evaluation)  
**Timeline**: 8-11 days  
**Generated**: 2026-01-14

## Quick Links

- **[Feature Specification](spec.md)** - Complete requirements and user stories
- **[Implementation Plan](plan.md)** - Technical context and structure decisions
- **[Research Document](research.md)** - Phase 0 output with all technical unknowns resolved
- **[Architecture Document](architecture.md)** - High-level decisions and implementation strategy
- **[Data Model](data-model.md)** - Compliance metadata structures
- **[Contracts](contracts/)** - Behavioral specifications and interfaces
- **[Quickstart Guide](quickstart.md)** - Developer onboarding and phase-by-phase guide

## Overview

This refactoring achieves full 42 School compliance for the miniRT project across three critical areas:

### 1. Norminette Compliance (FR-001 to FR-008)
- **Target**: 100% of 75 source files pass norminette 3.3.50
- **Constraints**: 
  - Functions ≤25 lines
  - Functions ≤4 parameters
  - Files ≤5 functions per file
  - Variables declared at function start

### 2. Forbidden Function Elimination (FR-006 to FR-008a)
- **Current**: 3 violations (2x memcpy, 1x memset)
- **Target**: 0 violations
- **Solution**: Custom utilities in `src/utils/memory.c`

### 3. Code Deduplication (FR-039 to FR-042)
- **Method**: SHA-256 hash-based exact duplicate detection
- **Criterion**: 100% byte-for-byte identical content only
- **Action**: Consolidate duplicates, update build system

## Implementation Phases

| Phase | Duration | Goal | Validation |
|-------|----------|------|------------|
| 0 | 2h | Baseline capture | 38 renders generated |
| 1-2 | 5h | Duplicate detection & consolidation | 0 duplicates, build passes |
| 3-4 | 6h | Forbidden function replacement | 0 violations, pixel-diff <0.1% |
| 5 | 3h | Norminette audit | Violations categorized |
| 6 | 3d | Function splitting | All ≤25 lines |
| 7 | 1.5d | Parameter reduction | All ≤4 params |
| 8 | 1d | Style fixes | 100% norminette pass |
| 9 | 1d | Full validation | All criteria met |
| 10 | 0.5d | Documentation | Complete & ready |

**Total**: 9 days + 0.5 day buffer

## Success Criteria (from spec.md SC-001 to SC-012)

| ID | Criterion | Target | Measurement |
|----|-----------|--------|-------------|
| SC-001 | Norminette compliance | 100% | 0 errors on 75 files |
| SC-002 | Forbidden functions | 0 | grep check returns nothing |
| SC-003 | Build warnings | 0 | -Wall -Wextra -Werror |
| SC-004 | Memory leaks | 0 bytes | valgrind on all scenes |
| SC-005 | Pixel difference | <0.1% | Automated tool on 38 scenes |
| SC-006 | Makefile targets | All pass | make, re, clean, fclean |
| SC-007 | Mandatory features | 100% | miniRT.pdf checklist |
| SC-008 | Evaluation items | 100% | miniRT_eval.pdf checklist |
| SC-009 | Error handling | 0 crashes | 20+ error scenarios |
| SC-010 | Compilation time | <120% | Time comparison |
| SC-011 | Functionality | No degradation | All features work |
| SC-012 | Peer review | Pass | 42 School evaluator approval |

## Key Design Decisions

### AD-1: Incremental Refactoring
- **Why**: Minimizes risk, enables quick rollback
- **How**: 11 discrete phases with validation after each

### AD-2: Custom Memory Utilities
- **Why**: Replace forbidden functions (memcpy, memset)
- **How**: Simple loop-based implementations in `src/utils/memory.c`

### AD-3: Pixel-Diff Validation
- **Why**: Catch visual regressions automatically
- **How**: <0.1% threshold comparing baseline vs. current renders

### AD-4: Hash-Based Duplicate Detection
- **Why**: 100% exact match only (per spec clarification)
- **How**: SHA-256 hashes, consolidate identical files

### AD-5: Static Helpers by Default
- **Why**: Minimize coupling, clear dependencies
- **How**: All helpers static unless needed by multiple files

## Risk Management

| Risk | Priority | Mitigation |
|------|----------|------------|
| Breaking functionality | P0 | Pixel-diff, frequent testing, atomic commits |
| Missing norminette violations | P0 | Automated checks, pre-commit hooks |
| Memory leaks | P1 | Valgrind after each module |
| Timeline overrun | P1 | Buffer time, prioritize critical path |
| Performance degradation | P2 | Benchmark, profile if needed |

## Rollback Procedures

**Granularity**:
1. File-level: `git checkout <commit> -- <file>`
2. Module-level: Revert entire module changes
3. Phase-level: `git reset --hard <phase-tag>`
4. Nuclear: Reset to pre-refactoring baseline

**Triggers**:
- Pixel-diff >0.1% → Immediate rollback
- Memory leak → Rollback module, debug
- Build failure → Fix or revert
- Norminette regression → Fix immediately

## Getting Started

### Prerequisites
```bash
# Verify tools
norminette --version        # 3.3.50+
make --version             # GNU Make 3.81+
valgrind --version         # 3.15.0+ (Linux) or leaks (macOS)
git --version              # 2.17+
```

### Start Refactoring

1. **Read Documentation**:
   - Start with [quickstart.md](quickstart.md) for detailed phase guide
   - Review [research.md](research.md) for technical decisions
   - Check [contracts/](contracts/) for behavioral specifications

2. **Create Branch**:
   ```bash
   git checkout -b 016-compliance-refactoring
   ```

3. **Begin Phase 0**:
   ```bash
   # Follow quickstart.md Phase 0 instructions
   make clean && make
   mkdir -p tests/baselines
   # Generate baseline renders...
   ```

4. **Execute Phases Sequentially**:
   - Complete each phase fully before moving to next
   - Validate after each phase
   - Commit frequently with clear messages
   - Tag validation milestones

## Documentation Structure

```
specs/016-compliance-refactoring/
├── README.md                        # This file (navigation hub)
├── spec.md                          # Feature specification (input)
├── plan.md                          # Implementation plan (structure)
├── architecture.md                  # High-level decisions & strategy
├── research.md                      # Phase 0 output (technical research)
├── data-model.md                    # Compliance metadata model
├── quickstart.md                    # Developer guide (start here!)
├── contracts/
│   └── refactoring-contracts.md     # Behavior specifications
└── tracking/                        # Runtime tracking (generated)
    ├── file_compliance.json
    ├── function_compliance.json
    ├── duplicates.json
    ├── forbidden_usage.json
    ├── validation_results.json
    └── phase_state.json
```

## Validation Commands

```bash
# Norminette (expect 0 errors)
norminette src/ includes/ | grep -c "Error"

# Forbidden functions (expect 0)
grep -rn "memset\|memcpy\|memmove" src/ includes/ | wc -l

# Build (expect success, 0 warnings)
make fclean && make 2>&1 | grep -i warning | wc -l

# Memory leaks (expect 0 bytes)
valgrind --leak-check=full ./miniRT scenes/mandatory/sphere.rt

# Pixel-diff (expect <0.1%)
./scripts/pixel_diff.sh

# Full regression test
./scripts/regression_test.sh
```

## Milestones

- [x] Planning complete (this document)
- [ ] Phase 0: Baseline captured
- [ ] Phase 2: Duplicates removed
- [ ] Phase 4: Forbidden functions eliminated
- [ ] Phase 6: Functions ≤25 lines
- [ ] Phase 7: Parameters ≤4
- [ ] Phase 8: 100% norminette pass
- [ ] Phase 9: Full validation passed
- [ ] Phase 10: Documentation complete
- [ ] Merged to main
- [ ] Ready for evaluation

## References

**42 School Documentation**:
- miniRT.pdf - Project subject and requirements
- miniRT_eval.pdf - Evaluation checklist
- norminette - https://github.com/42School/norminette

**Project Documentation**:
- README.md - Project overview
- CHANGELOG.md - Project history
- docs/ - Korean documentation

**External Resources**:
- Valgrind Manual - https://valgrind.org/docs/manual/
- C99 Standard - ISO/IEC 9899:1999
- Ray Tracing in One Weekend - https://raytracing.github.io/

## Status Tracking

**Current Phase**: Planning Complete  
**Next Action**: Review documentation, then begin Phase 0 (Baseline Capture)  
**Blocked On**: N/A  
**Issues**: None  
**Last Updated**: 2026-01-14

---

**For detailed phase-by-phase instructions, see [quickstart.md](quickstart.md)**
