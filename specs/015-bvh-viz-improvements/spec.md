# Feature Specification: BVH Tree Visualization Improvements

**Feature Branch**: `001-bvh-viz-improvements`  
**Created**: 2026-01-12  
**Status**: Draft  
**Input**: User description: "Create a feature specification for improving BVH Tree Visualization with two main enhancements: 1. Consistent Object Identification: Align the object type and number format displayed in the HUD with the information shown in BVH leaf nodes. 2. Debounced Rendering for Keyboard Input: Implement a debounce mechanism that waits 1 second before rendering after keyboard input, resetting the timer if another input occurs within that 1 second."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Unified Object Identification Format (Priority: P1)

A developer is debugging ray intersection issues by examining the BVH tree structure. They notice object "sp-1" in the HUD but cannot quickly identify which leaf node in the ASCII tree visualization corresponds to this object because the formats don't match. With unified formatting, they can immediately correlate HUD information with tree nodes, speeding up debugging by eliminating the need to mentally map between different naming conventions.

**Why this priority**: This is the foundational improvement that enables efficient debugging workflows. Without consistent identification, users waste time cross-referencing objects between displays, reducing the utility of both the HUD and tree visualization.

**Independent Test**: Can be fully tested by loading a scene with multiple objects, viewing both HUD and BVH tree visualization, and verifying that object identifiers use identical format (e.g., "sp-1" in HUD matches "sp-1" in leaf nodes). Delivers immediate value by making object correlation straightforward.

**Acceptance Scenarios**:

1. **Given** a scene with 3 spheres, 2 planes, and 1 cylinder, **When** user views the HUD, **Then** objects are displayed as "sp-1, sp-2, sp-3, pl-1, pl-2, cy-1"
2. **Given** the same scene with HUD showing "sp-1, sp-2, sp-3, pl-1, pl-2, cy-1", **When** user views BVH tree visualization, **Then** leaf nodes display objects with matching format "sp-1", "sp-2", "sp-3", "pl-1", "pl-2", "cy-1"
3. **Given** BVH tree showing a leaf node with "sp-2", **When** user looks at HUD, **Then** they can immediately identify "sp-2" without mental conversion

---

### User Story 2 - Smooth Keyboard Interaction (Priority: P2)

A developer is interacting with the BVH visualization using any keyboard controls. They want to perform rapid keyboard actions but experience stuttering and lag because each key press triggers an immediate re-render. With debounced rendering, all keyboard inputs feel smooth and responsive, with the final state rendered only after they stop pressing keys.

**Why this priority**: While P1 addresses information clarity, P2 focuses on interaction quality. A user can still use the system with stuttering, but it creates frustration and slows down exploration. This enhancement significantly improves user experience during intensive keyboard interaction sessions.

**Independent Test**: Can be fully tested by rapidly pressing any keyboard keys (e.g., holding down any key for 3 seconds) and observing that rendering occurs only once after key input stops, not on every keypress. Delivers value by eliminating visual stuttering and improving perceived performance.

**Acceptance Scenarios**:

1. **Given** BVH tree visualization is displayed, **When** user presses any keyboard key once, **Then** system waits 1 second and renders the new state
2. **Given** user pressed a keyboard key 0.5 seconds ago (timer running), **When** user presses another keyboard key, **Then** timer resets to 1 second from the new keypress
3. **Given** user rapidly presses any keyboard keys 5 times over 2 seconds, **When** user stops pressing keys, **Then** system renders only once after 1 second of inactivity, showing the final cumulative state
4. **Given** user is holding down any key, **When** continuous key events are generated, **Then** each event resets the 1-second timer, and rendering occurs only 1 second after key release

---

### Edge Cases

- **Keyboard input during active render**: If keyboard input occurs while rendering is already in progress, the system MUST queue the input and restart the debounce timer after the current render operation completes. This ensures no inputs are lost and prevents race conditions between rendering and input handling.
- **More than 99 objects of same type**: When a .rt scene file contains more than 99 objects of the same type, the system MUST fail the scene load operation and display a clear error message indicating the object type that exceeded the 99-object limit, preventing identifier overflow and undefined display behavior
- **Mouse action during debounce**: When debounce timer is active and user performs a non-keyboard action (e.g., mouse click), the system MUST discard all queued keyboard inputs, cancel the pending timer, and handle the mouse action immediately without waiting for the debounce delay to expire
- **Window/view switch during debounce**: When user attempts to toggle between HUD and tree visualization displays during the debounce period, the system MUST prevent the toggle action and keep the current view active until the debounce timer expires and pending render completes
- **Timer pool overflow**: When more than 4 components attempt to register timers simultaneously, the system returns an error code to components attempting to register beyond the limit, allowing the calling component to handle the failure appropriately (e.g., log warning, fall back to immediate rendering)
- **Invalid object identifiers in scene file**: If a .rt scene file contains objects with invalid or missing type identifiers, the system MUST fail the scene load operation and display a clear error message indicating which object(s) have invalid identifiers, preventing undefined behavior from malformed scene data
- **BVH construction failure after parsing**: When scene parsing completes successfully with assigned object IDs but BVH construction subsequently fails, the system MUST preserve all assigned object IDs, alert the user of the BVH failure, and allow fallback rendering (without BVH acceleration) to proceed using the preserved IDs

## Requirements *(mandatory)*

### Format Specification

**Object Identifier Format**: `<type>-<number>`
- **Type prefix**: Exactly 2 lowercase letters representing object type
- **Separator**: Single hyphen character `-`
- **Number**: Sequential integer starting from 1 for each object type, incrementing independently per type based on parse order from .rt scene file (e.g., first sphere encountered becomes sp-1, second sphere becomes sp-2, first plane becomes pl-1)
- **Supported types**: `sp` (sphere), `pl` (plane), `cy` (cylinder)
- **Examples**: `sp-1`, `pl-2`, `cy-3`, `sp-42`
- **Case sensitivity**: Type prefix MUST be lowercase; uppercase not permitted

### Technical Constraints

- **Timer State Machine**: State transitions MUST be implemented using an enum-based state machine with three states:
  - `TIMER_IDLE`: No pending input, timer not active
  - `TIMER_PENDING`: Input received, waiting for 1-second delay to expire
  - `TIMER_RENDERING`: Rendering in progress, queue new inputs
- **State Transition Rules**:
  - `IDLE → PENDING`: On first keyboard navigation input
  - `PENDING → PENDING`: On subsequent navigation input (reset timestamp)
  - `PENDING → RENDERING`: When 1-second delay expires
  - `PENDING → IDLE`: When mouse action occurs (discard queued keyboard inputs and cancel timer immediately)
  - `RENDERING → PENDING`: When render completes AND queued inputs exist
  - `RENDERING → IDLE`: When render completes AND no queued inputs
- **Timestamp Precision**: Use `gettimeofday()` system call to obtain microsecond-precision timestamps for accurate 1-second delay measurement
- **Observability Logging**: Log only state machine errors (invalid transitions, initialization failures) and state transitions (IDLE→PENDING, PENDING→RENDERING, etc.) to minimize performance overhead while maintaining debuggability
- **Performance Constraint**: Debounce timer operations (state checks, timestamp comparisons, queue management) MUST consume less than 5% of CPU time during active keyboard interaction sessions to ensure responsive user experience

### Functional Requirements

- **FR-001**: System MUST display object identifiers in HUD using format specified in Format Specification section
- **FR-002**: System MUST display object identifiers in BVH tree leaf nodes using format specified in Format Specification section (matching FR-001), showing only the identifier without additional metadata
- **FR-003**: System MUST support all object type prefixes defined in Format Specification section
- **FR-004**: System MUST number objects of the same type sequentially as defined in Format Specification section
- **FR-005**: System MUST implement a debounce mechanism for all keyboard input events that delays rendering actions
- **FR-006**: System MUST set debounce delay to 1 second after initial keyboard input
- **FR-007**: System MUST reset the debounce timer to 1 second whenever a new keyboard input occurs before the timer expires
- **FR-008**: System MUST execute only one render operation after the debounce timer expires without interruption
- **FR-009**: System MUST accumulate all keyboard inputs during the debounce period and apply them cumulatively to the final render
- **FR-010**: System MUST apply debouncing to all keyboard controls globally, not limited to navigation keys
- **FR-011**: System MUST maintain consistent object numbering across HUD and tree visualization throughout the session
- **FR-012**: System MUST queue keyboard inputs that arrive during an active render operation and restart the debounce timer after render completion
- **FR-013**: System MUST NOT discard or lose keyboard inputs when they occur during rendering
- **FR-014**: System MUST discard all queued keyboard inputs when a mouse action occurs during the debounce period
- **FR-015**: System MUST handle mouse actions immediately when they occur, bypassing any active debounce timer
- **FR-016**: System MUST return an error code when a component attempts to register a timer beyond the 4-slot pool capacity
- **FR-017**: System MUST allow calling components to handle timer registration failures appropriately (e.g., logging warnings, falling back to immediate rendering)
- **FR-018**: System MUST validate object type identifiers during scene file parsing and fail the scene load operation if any object has an invalid or missing identifier
- **FR-019**: System MUST display a clear error message when scene load fails due to invalid object identifiers, indicating which object(s) have invalid identifiers
- **FR-020**: System MUST prevent HUD/tree visualization toggle actions when the debounce timer is in PENDING or RENDERING state
- **FR-021**: System MUST allow HUD/tree visualization toggle actions only when the debounce timer is in IDLE state (no pending keyboard inputs or active renders)
- **FR-022**: System MUST assign object IDs during scene file parsing phase, immediately after each object is successfully parsed
- **FR-023**: System MUST preserve assigned object IDs even if subsequent BVH construction fails after parsing completes
- **FR-024**: System MUST alert the user when BVH construction fails post-parsing, allowing fallback rendering to proceed with preserved object IDs
- **FR-025**: System MUST validate that no object type exceeds 99 instances during scene file parsing
- **FR-026**: System MUST fail scene load operation when any object type exceeds 99 instances and display a clear error message indicating which object type exceeded the limit

### Key Entities

- **Object Identifier**: Represents a unique reference to a scene object, consisting of type prefix (2 letters), separator (hyphen), and sequential number (integer starting from 1). Storage: Stored directly in each individual object type struct (t_sphere, t_plane, t_cylinder, etc.) as a `char id[8]` field, where each string holds format "<type>-<number>" (e.g., "sp-1", "pl-2").
- **Keyboard Input Event**: Represents any user keyboard action (key press/hold) that triggers actions, with timestamp for debounce tracking (applies to all keyboard inputs globally)
- **Debounce Timer**: Represents a countdown mechanism (1 second duration) that determines when to execute the accumulated rendering action, with state tracking implemented as a state machine using an enum (IDLE/PENDING/RENDERING) to handle concurrent events and timestamp precision obtained via `gettimeofday()` system call for accurate timing
- **Timer Pool**: Pre-allocated fixed array of 4 timer slots, with one slot per view/component that can trigger rendering, avoiding runtime memory allocation overhead during debounce operations. When more than 4 components request timers, return error code to the component attempting registration beyond the limit, allowing the calling component to handle the failure (e.g., log warning, fall back to immediate rendering). Timer pool MUST be reset to initial state on scene reload or reset to prevent stale timer references
- **Input Queue**: Buffer that stores keyboard inputs received during active rendering operations, ensuring no inputs are lost when render is in progress. Fixed capacity of 8 inputs. When queue reaches capacity, incoming inputs are rejected and discarded to maintain queue integrity

## Clarifications

### Session 2026-01-12

- Q: Should object type prefixes be uppercase (SP, PL, CY) or lowercase (sp, pl, cy)? → A: lowercase
- Q: How should the debounce timer state be managed when multiple views/components can render? → A: Pre-allocated fixed pool (one timer slot per view/component, avoiding dynamic allocation)
- Q: What happens if a keyboard input occurs while rendering is already in progress? → A: Queue the input and restart debounce timer after render completes
- Q: How should timer state transitions be implemented to handle concurrent events? → A: State machine with enum (recommended for clarity and maintainability)
- Q: What timestamp precision should be used for the 1-second debounce delay measurement? → A: gettimeofday() (microsecond-precision timing)
- Q: What should be the maximum capacity of the input queue? → A: 8 inputs
- Q: What level of observability logging should the debounce timer state machine produce? → A: Log only errors and state transitions
- Q: What is the maximum acceptable CPU overhead for debounce timer operations during keyboard interaction? → A: <5% CPU time
- Q: What should happen when a keyboard input occurs during the debounce period, but user initiates a mouse action? → A: Discard all queued keyboard inputs and handle mouse action immediately
- Q: What happens when the input queue reaches its capacity of 8 inputs during an active render? → A: Discard newest - reject the incoming input and keep existing queue unchanged
- Q: What happens when objects in a .rt scene file have invalid or missing type identifiers? → A: Fail scene load with clear error message
- Q: How should object IDs be stored and retrieved for display in both HUD and BVH tree? → A: Store directly in each object type struct (t_sphere, t_plane, t_cylinder) as char id[8] field
- Q: What should be the byte size for the identifier storage in each object struct? → A: 8 bytes
- Q: What information should BVH leaf nodes display for each object? → A: Display only the identifier
- Q: What happens to existing timer pool slots when a scene is reloaded or reset? → A: Reset timer pool on scene reload
- Q: What should happen when more than 4 components attempt to register timers simultaneously? → A: Return error on timer registration overflow
- Q: How should system handle objects when there are more than 99 of the same type? → A: Fail scene load with clear error message when >99 detected
- Q: Where should the unified object identifier rendering logic be implemented? → A: Modify existing render_sphere_obj(), render_plane_obj(), render_cylinder_obj() functions in hud_obj_render.c directly
- Q: What determines the sequential numbering order for objects of the same type (e.g., which sphere becomes sp-1, sp-2)? → A: Parse order from .rt file
- Q: When should object IDs be assigned if BVH construction fails after parsing? → A: Assign IDs during parsing; if BVH fails, alert user but preserve IDs
- Q: Should HUD/tree toggle be allowed during the debounce period? → A: Prevent HUD/tree toggle during debounce

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Users can identify the same object in both HUD and BVH tree visualization without mental conversion or reference lookup (100% format consistency)
- **SC-002**: Rapid keyboard input (10+ key presses of any keys within 2 seconds) results in only one rendering operation after input stops
- **SC-003**: Visual stuttering during keyboard interaction is eliminated, with smooth perceived performance (rendering occurs only after 1-second pause)
- **SC-004**: Users can interact with the system using any keyboard controls with responsive feedback and final state accurately reflecting all accumulated inputs
- **SC-005**: Debugging time for correlating objects between HUD and tree visualization reduces by at least 50% through consistent identifier format
