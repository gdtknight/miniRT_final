# Architecture & Implementation Summary

**Feature**: 016-compliance-refactoring  
**Date**: 2026-01-14  
**Purpose**: Executive summary of architecture decisions, implementation strategy, and risk management

## Executive Summary

This document provides the high-level architecture and implementation strategy for achieving full 42 School compliance in the miniRT project. The refactoring targets three critical areas with zero tolerance for failure:

1. **Norminette Compliance** (100% pass rate across 75 files)
2. **Forbidden Function Elimination** (0 violations)
3. **Code Deduplication** (100% hash-based exact match detection)

**Strategy**: Incremental, phase-based refactoring with automated validation at each step, ensuring zero functional regressions while achieving compliance.

## Architecture Decisions

### AD-1: Incremental Refactoring with Continuous Validation

**Decision**: Refactor in 11 discrete phases with automated validation after each module change.

**Rationale**: 
- Minimizes risk of introducing regressions
- Allows for quick rollback to last known good state
- Provides clear progress tracking
- Enables parallel work where dependencies allow

**Alternatives Rejected**:
- Big-bang refactoring → Too risky, hard to debug
- Manual validation → Too slow, error-prone
- File-by-file approach → Doesn't respect architectural boundaries

### AD-2: Custom Memory Utilities Module

**Decision**: Implement `ft_memcpy` and `ft_memset` in dedicated `src/utils/memory.c` module.

**Rationale**:
- Centralizes forbidden function replacements
- Provides single source of truth for memory operations
- Reusable across all modules
- Easy to test in isolation
- Complies with 42 School function constraints

**Implementation**:
- Simple loop-based implementations (no optimization tricks)
- Identical behavior to standard library functions
- Comprehensive testing before deployment
- Well-documented with edge cases

**Alternatives Rejected**:
- Inline loops at call sites → Code duplication
- Use different allowed function → None available that fit
- Aggregate initialization → Not applicable for dynamic buffers

### AD-3: Pixel-Diff Validation with <0.1% Threshold

**Decision**: Automated pixel-diff tool comparing rendered images before/after refactoring.

**Rationale**:
- Catches visual regressions automatically
- Tolerates minor floating-point differences
- Provides quantitative pass/fail criteria
- Faster than manual visual inspection
- Creates audit trail of validation results

**Implementation**:
- Baseline renders generated before refactoring starts (Phase 0)
- Compare using image difference metrics (RMSE or similar)
- Threshold set to 0.1% to allow for acceptable rounding differences
- Automated script runs on all 38 test scenes

**Alternatives Rejected**:
- Checksum comparison → Too strict, fails on floating-point differences
- Manual inspection → Too slow, not reproducible
- Perceptual hash → Too lenient, might miss subtle bugs

### AD-4: Hash-Based Exact Duplicate Detection

**Decision**: Use SHA-256 hashes to identify files with 100% identical content.

**Rationale**:
- As clarified in spec, only exact duplicates qualify
- SHA-256 collision probability negligible
- Deterministic and reproducible
- Fast to compute
- Prevents accidental consolidation of similar-but-different files

**Implementation**:
- Generate hash for each source file
- Group files with identical hashes
- Choose canonical version based on naming and location
- Update build system and references
- Verify build succeeds after consolidation

**Alternatives Rejected**:
- Similarity-based (>90%) → Rejected per spec clarification
- AST comparison → Overkill for exact duplicate detection
- Manual review only → Too error-prone

### AD-5: Static Helper Functions by Default

**Decision**: All helper functions extracted during function splitting are static unless needed by multiple files.

**Rationale**:
- Minimizes namespace pollution
- Prevents unintended coupling between modules
- Makes dependencies explicit
- Follows best practices for C code organization
- Easier to refactor later if needed

**Implementation**:
- Extract helpers as static initially
- Promote to non-static only when:
  - Multiple files need the function
  - Function becomes part of module's public API
- Document why non-static functions are public

**Alternatives Rejected**:
- All helpers non-static → Unnecessary coupling
- Mix of static/non-static randomly → Inconsistent, hard to maintain

## Implementation Strategy

### Phase Grouping

**Preparation Phases** (Days 1-2):
- Phase 0: Baseline Capture
- Phase 1: Duplicate Detection
- Phase 2: Duplicate Consolidation
- Phase 3: Forbidden Functions Audit
- Phase 4: Function Replacement

**Refactoring Phases** (Days 3-7):
- Phase 5: Norminette First Pass (audit)
- Phase 6: Function Splitting (25-line limit)
- Phase 7: Parameter Reduction (4-param limit)
- Phase 8: Style Fixes (remaining violations)

**Validation Phases** (Days 8-9):
- Phase 9: Full Validation
- Phase 10: Documentation Update

### Critical Path

```
Phase 0 → Phase 5 → Phase 6 → Phase 8 → Phase 9 → Phase 10
```

**Parallelization Opportunities**:
- Phases 1 (Duplicates) and 3 (Forbidden Functions) can run concurrently
- Phase 6 (Splitting) and 7 (Parameters) can partially overlap (different modules)

**Dependencies**:
- Phase 2 depends on Phase 1 (need duplicate list)
- Phase 4 depends on Phase 3 (need violation list)
- Phase 5 must complete before 6, 7, 8 (need violation report)
- Phases 6, 7, 8 must complete before Phase 9 (validation needs all refactoring done)

### Validation Strategy

**Continuous Validation** (after each change):
- Norminette check on modified files
- Build verification
- Quick test with 1-2 scenes

**Phase Validation** (after each phase):
- Full norminette scan
- Build all targets
- Memory check on representative scenes
- Pixel-diff on subset of scenes

**Final Validation** (Phase 9):
- Complete norminette scan (all files)
- Build system verification (all targets)
- Memory checks on all 38 scenes
- Pixel-diff on all 38 scenes
- Manual functional testing
- Evaluation checklist verification

### Rollback Strategy

**Granularity Levels**:
1. **File-level**: Revert specific file to last good version
2. **Module-level**: Revert entire module (e.g., all parser/ changes)
3. **Phase-level**: Revert to tagged phase completion commit
4. **Nuclear**: Reset to pre-refactoring state (baseline tag)

**Triggers**:
- Pixel-diff >0.1% → Immediate rollback of last change
- Memory leak detected → Rollback module, investigate
- Build failure → Quick fix or revert
- Norminette regression → Fix immediately or revert

**Procedure**:
```bash
# Identify problem scope
git log --oneline --decorate

# Option 1: Revert last commit
git revert HEAD

# Option 2: Reset to specific commit
git reset --hard <commit-hash>

# Option 3: Revert specific files
git checkout <commit-hash> -- <file>

# Always validate after rollback
make clean && make
./scripts/regression_test.sh
```

## Risk Management

### Risk Priority Matrix

| Priority | Risk | Mitigation |
|----------|------|------------|
| **P0** | Breaking functionality | Pixel-diff, frequent testing, atomic commits |
| **P0** | Missing norminette violations | Automated checks, pre-commit hooks |
| **P1** | Memory leaks | Valgrind after each module, focus on new code |
| **P1** | Timeline overrun | Buffer time, prioritize critical path |
| **P2** | Performance degradation | Benchmark critical paths, profile if needed |
| **P3** | Incomplete duplicate detection | Hash-based reliable, manual review supplement |

### Risk Mitigation Details

**Breaking Functionality** (P0):
- **Prevention**: Automated pixel-diff with <0.1% threshold, test after every change
- **Detection**: Continuous testing, visual inspection of key scenes
- **Response**: Immediate rollback, investigate root cause
- **Recovery**: Git history allows instant revert to last good state

**Missing Norminette Violations** (P0):
- **Prevention**: Run norminette after every file modification
- **Detection**: Full scan at phase boundaries
- **Response**: Fix violations immediately following refactoring patterns
- **Recovery**: Norminette violations don't break functionality, safe to fix iteratively

**Memory Leaks** (P1):
- **Prevention**: Careful code review of malloc/free pairs, valgrind after modules
- **Detection**: Automated valgrind checks on all scenes
- **Response**: Debug with valgrind detailed output, review error paths
- **Recovery**: Rollback leaking module if fix unclear

**Timeline Overrun** (P1):
- **Prevention**: 2-3 day buffer in 8-11 day estimate
- **Detection**: Daily progress tracking against plan
- **Response**: Prioritize critical path, defer non-essential tasks
- **Recovery**: Extend timeline if necessary, communicate delays

## Testing Strategy

### Test Layers

**Layer 1: Static Analysis** (Continuous)
- Norminette compliance
- Forbidden function detection
- Build warnings
- Frequency: After every file edit

**Layer 2: Unit Testing** (Per Module)
- Custom memory utilities (ft_memcpy, ft_memset)
- Individual refactored functions
- Frequency: When implementing new code

**Layer 3: Integration Testing** (Per Phase)
- Pixel-diff validation
- Memory leak detection
- Build system verification
- Frequency: After each phase completion

**Layer 4: Functional Testing** (Major Milestones)
- All 38 scenes render
- Window management
- Error handling
- Frequency: After Phases 4, 8, 9

**Layer 5: Manual Testing** (Final Validation)
- Visual quality inspection
- Evaluation checklist
- Peer review
- Frequency: Phase 9 only

### Test Coverage

**Mandatory Scenes** (must pass):
- sphere.rt, plane.rt, cylinder.rt
- lighting.rt, camera.rt
- Combined features

**Edge Cases**:
- FOV extremes (0, 180)
- Large/small coordinates
- Multiple objects
- Complex scenes

**Error Cases**:
- Missing required elements
- Invalid values
- Malformed syntax
- File I/O errors

**Total**: 38 scenes covering all requirements and edge cases

## Success Metrics

### Quantitative Metrics

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| Norminette Pass Rate | 100% | `norminette src/ includes/ \| grep Error` = 0 |
| Forbidden Functions | 0 | `grep -r "memset\|memcpy" src/` = 0 |
| Build Warnings | 0 | `make 2>&1 \| grep warning` = 0 |
| Memory Leaks | 0 bytes | `valgrind` on all 38 scenes |
| Pixel Difference | <0.1% | Automated pixel-diff tool |
| Test Pass Rate | 100% | All 38 scenes render successfully |
| Compilation Time | <120% | `time make clean && make` |

### Qualitative Metrics

- Code more readable (better function organization)
- Code more maintainable (no duplicates, clear helpers)
- Evaluation-ready (checklist verified)
- Peer review approved (42 School standards)

## Timeline & Milestones

### Week 1: Core Refactoring (Days 1-7)

**Day 1**: Preparation
- AM: Phase 0 (Baseline)
- PM: Phases 1-2 (Duplicates)

**Day 2**: Forbidden Functions
- AM: Phase 3 (Audit)
- PM: Phase 4 (Replacement)

**Days 3-5**: Function Splitting
- Phase 6 (Splitting to 25 lines)
- Continuous validation

**Days 6-7**: Parameter Reduction & Style
- Day 6: Phase 7 (4-parameter limit)
- Day 7: Phase 8 (Style fixes)

### Week 2: Validation & Completion (Days 8-9)

**Day 8**: Full Validation
- Phase 9 (Complete testing)
- Bug fixes if needed

**Day 9**: Documentation & Review
- Phase 10 (Docs update)
- Peer review preparation
- Buffer for issues

### Milestones

| Milestone | Date | Deliverable | Validation |
|-----------|------|-------------|------------|
| M1: Baseline | Day 1 AM | 38 baseline renders | All scenes render |
| M2: Clean Codebase | Day 1 PM | No duplicates | Build passes |
| M3: Compliance | Day 2 PM | No forbidden funcs | Grep check = 0 |
| M4: Function Size | Day 5 | All ≤25 lines | Norminette pass |
| M5: Parameters | Day 6 | All ≤4 params | Norminette pass |
| M6: Style Complete | Day 7 | 100% norminette | Full scan passes |
| M7: Validated | Day 8 | All tests pass | Final validation |
| M8: Complete | Day 9 | Ready for merge | Docs complete |

## Post-Refactoring

### Immediate Actions

1. **Pull Request**: Create PR with summary, link to docs, validation results
2. **Code Review**: Request review, provide checklist, address feedback
3. **CI/CD**: Verify automated checks pass
4. **Merge**: Squash/rebase as needed, merge to main

### Release

1. **Tag**: `v1.0.0-compliant` - Full 42 School compliance achieved
2. **Documentation**: Update README, wiki, changelog
3. **Announcement**: Notify team/stakeholders of completion

### Evaluation Preparation

1. **Practice**: Run through evaluation checklist
2. **Demo**: Prepare demo scenarios
3. **Questions**: Prepare answers for common evaluator questions
4. **Cleanup**: Ensure clean repository state

## Conclusion

This architecture and implementation strategy provides:

✅ **Clear Path**: 11 phases with defined deliverables  
✅ **Risk Mitigation**: Multiple validation layers, rollback procedures  
✅ **Quality Assurance**: Automated testing, continuous validation  
✅ **Timeline**: 8-11 days with buffer for unexpected issues  
✅ **Success Criteria**: Measurable, verifiable metrics  
✅ **Evaluation Ready**: Complies with all 42 School requirements  

**Status**: Ready to begin Phase 0 (Baseline Capture)

**Next Step**: Review this document with team, then execute Phase 0

---

**Document Version**: 1.0  
**Last Updated**: 2026-01-14  
**Author**: Compliance Refactoring Team
