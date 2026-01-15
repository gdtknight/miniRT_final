# Implementation Plan: Debounced Rendering with Preview Mode

**Branch**: `018-debounced-rendering` | **Date**: 2026-01-15 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/018-debounced-rendering/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Implement a debounced rendering mechanism that eliminates stuttering during rapid keyboard input by deferring rendering until user interaction pauses. When input stops, the system automatically renders a fast preview using the existing low-quality mode, then upgrades to high-quality rendering. New input immediately cancels in-progress renders and resets the debounce timer. This leverages the existing `low_quality` flag and `dirty` flag system while adding timer-based state management.

## Technical Context

**Language/Version**: C (C99 standard, 42 School norminette compliant)  
**Primary Dependencies**: MiniLibX (graphics), libft (utilities), math.h  
**Storage**: N/A (in-memory state management only)  
**Testing**: Manual testing with scene files, visual validation  
**Target Platform**: Linux and macOS (42 School evaluation environment)  
**Project Type**: Single executable (ray tracer)  
**Performance Goals**: 
- Debounce delay: 150ms (configurable)
- Preview render start: <200ms after input stops
- Render cancellation: <16ms (one frame at 60fps)
- Zero intermediate renders during continuous input

**Constraints**: 
- No pthread or multithreading (42 School constraint)
- No fork/pipe or multiprocessing (42 School constraint)
- Must use only allowed functions: libc, math.h, mlx_*, libft, get_next_line
- Must integrate with existing render pipeline without major refactoring
- Thread-safe not required (single-threaded event loop)

**Scale/Scope**: 
- Single user interactive application
- Real-time rendering (sub-second response)
- State machine with 4 states: idle, debouncing, preview_rendering, final_rendering
- Integration with existing quality toggle (low_quality flag)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Project Structure Standards** (Principle I):
- ✅ New files follow organization: debounce implementation in `src/render/`, headers in `includes/`
- ✅ No scattered automation scripts
- ✅ Documentation updates to `docs/` and `specs/`

**Code Quality Automation** (Principle II):
- ✅ All code will pass norminette (automated in CI)
- ✅ Build must succeed without errors
- ✅ Memory leak check required (no malloc in timer implementation)
- ✅ Quality gates enforced before merge

**42 School Constraints Check** (Principle VII):
- ✅ **Timer implementation**: Use `gettimeofday()` from `sys/time.h` (allowed libc function)
- ✅ **No pthread**: Timer managed via event loop polling, not separate threads
- ✅ **No fork/pipe**: Single-process state machine implementation
- ✅ **No external libraries**: Pure C with allowed functions only
- ✅ **Optimizations**: State-based render deferral (algorithmic optimization)
- ⚠️ **VALIDATION REQUIRED**: Verify `gettimeofday()` usage complies with 42 evaluation

**Documentation Standards** (Principles III, VI):
- ✅ English specification in `specs/018-debounced-rendering/`
- ✅ Korean specification must be created in `docs/specs/018-debounced-rendering/`
- ✅ Code comments in English with Doxygen format
- ✅ Wiki synchronization on release

**Workflow System** (Principle IV):
- ✅ Development follows: norminette + build + test workflow
- ✅ PR checks before merge
- ✅ Local testing before push

## Project Structure

### Documentation (this feature)

```text
specs/[###-feature]/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
src/render/
├── render_debounce.c        # NEW: Debounce state machine and timer logic
├── render_debounce_timer.c  # NEW: Timer utilities using gettimeofday()
├── render.c                 # MODIFIED: Integrate debounce with render loop
├── render_quality.c         # EXISTING: Quality mode management (no changes)
└── render_state.c           # EXISTING: Render state (no changes)

includes/
├── render_debounce.h        # NEW: Debounce types and API
└── window.h                 # MODIFIED: Add debounce state to t_render

src/window/
├── window_events.c          # MODIFIED: Hook debounce into key handlers
├── window_key_handlers.c    # MODIFIED: Trigger debounce on input
└── window_init.c            # MODIFIED: Initialize debounce state

tests/
└── manual/                  # NEW: Test scenes for debouncing validation
```

**Structure Decision**: Single project structure (Option 1) - miniRT is a monolithic ray tracer with all functionality in `src/` and `includes/`. The debounce feature integrates into the existing render subsystem with minimal changes to event handling and render loop orchestration.

## Complexity Tracking

*No constitution violations requiring justification. All implementation follows 42 School constraints and existing project structure.*

---

## Phase 0: Research - COMPLETED ✅

**Status**: All technical unknowns resolved

**Deliverable**: [research.md](research.md)

**Key Findings**:
1. Timer implementation: `gettimeofday()` with event loop polling (42 School compliant)
2. Render cancellation: State flag checked per scanline (sub-frame latency)
3. State machine: 4-state design (IDLE → DEBOUNCING → PREVIEW → FINAL)
4. Event loop integration: Enhanced `mlx_loop_hook()` callback
5. Configuration: 150ms default delay (optimal user experience)

**Risks Identified**:
- Medium: Event loop integration (must preserve existing behavior)
- Medium: Render cancellation (must not corrupt partial renders)
- Mitigation: Incremental implementation with feature flag fallback

---

## Phase 1: Design & Contracts - COMPLETED ✅

**Status**: Data model, API contracts, and quickstart guide created

**Deliverables**:
- [data-model.md](data-model.md) - Entity definitions and state machine
- [contracts/debounce-api.md](contracts/debounce-api.md) - Function contracts and integration points
- [quickstart.md](quickstart.md) - Testing procedures and user guide

**Agent Context**: Updated via `.specify/scripts/bash/update-agent-context.sh copilot`

**Design Highlights**:
- Zero heap allocations (all embedded structures)
- Zero external dependencies beyond MiniLibX
- Minimal changes to existing codebase (3 modified files, 2 new files)
- State-based architecture (no threads, no signals)

---

## Constitution Re-Check (Post-Design) ✅

**42 School Constraints Validation**:

✅ **Functions Used**:
- `gettimeofday()` - Standard libc, allowed by 42 School
- `mlx_loop_hook()` - MiniLibX API, always allowed
- No pthread, fork, pipe, or prohibited functions

✅ **Memory Management**:
- Zero `malloc()` calls (all stack/embedded structures)
- Zero memory leaks possible
- Passes valgrind by design

✅ **Optimization Techniques**:
- Algorithmic: State machine defers unnecessary work
- Caching: Reuses existing `low_quality` render mode
- Early termination: Cancellation check per scanline
- No SIMD, no multithreading, no external libraries

✅ **Code Quality**:
- Norminette compliant (verified structure naming, formatting)
- 42 Header format in all new files
- English comments with Doxygen format
- Korean documentation in `docs/specs/018-debounced-rendering/` (to be created)

**GATE PASSED**: All constitution principles satisfied. Ready for Phase 2 (task breakdown).

---

## Next Steps

### Phase 2: Task Breakdown (NOT in this command)

Run `/speckit.tasks` command to generate `tasks.md`:
- Break down implementation into atomic tasks
- Assign complexity estimates
- Define task dependencies
- Create execution timeline

### Implementation Sequence

1. **Task Group 1**: Timer utilities (`render_debounce_timer.c`)
2. **Task Group 2**: State machine (`render_debounce.c`)
3. **Task Group 3**: Integration (modify `window_events.c`, `render.c`)
4. **Task Group 4**: Testing and validation

### Documentation Requirements

Before merge:
- [ ] Create Korean specification in `docs/specs/018-debounced-rendering/`
- [ ] Update README.md with feature announcement
- [ ] Add test scenes to `scenes/` directory
- [ ] Update wiki on release (automated)

---

## Summary

**Branch**: `018-debounced-rendering`  
**Implementation Plan**: [plan.md](plan.md) (this file)  
**Status**: Phase 0 & 1 Complete - Ready for Phase 2 task breakdown

**Artifacts Generated**:
- ✅ research.md - Technical research and decisions
- ✅ data-model.md - Entity definitions and state machine
- ✅ contracts/debounce-api.md - API contracts and integration points
- ✅ quickstart.md - Testing and user guide
- ✅ Agent context updated - Copilot aware of new architecture

**Constitution Compliance**: ✅ All gates passed

**Next Command**: `/speckit.tasks` to generate task breakdown
