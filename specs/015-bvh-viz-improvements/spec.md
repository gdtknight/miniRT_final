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

### User Story 2 - Smooth Keyboard Navigation (Priority: P2)

A developer is navigating through BVH tree levels using keyboard controls (arrow keys or similar). They want to quickly move through multiple levels but experience stuttering and lag because each key press triggers an immediate re-render. With debounced rendering, rapid keyboard inputs feel smooth and responsive, with the final state rendered only after they stop pressing keys.

**Why this priority**: While P1 addresses information clarity, P2 focuses on interaction quality. A user can still use the system with stuttering, but it creates frustration and slows down exploration. This enhancement significantly improves user experience during intensive navigation sessions.

**Independent Test**: Can be fully tested by rapidly pressing navigation keys (e.g., holding down arrow key for 3 seconds) and observing that rendering occurs only once after key input stops, not on every keypress. Delivers value by eliminating visual stuttering and improving perceived performance.

**Acceptance Scenarios**:

1. **Given** BVH tree visualization is displayed, **When** user presses a navigation key once, **Then** system waits 1 second and renders the new state
2. **Given** user pressed a navigation key 0.5 seconds ago (timer running), **When** user presses another navigation key, **Then** timer resets to 1 second from the new keypress
3. **Given** user rapidly presses navigation keys 5 times over 2 seconds, **When** user stops pressing keys, **Then** system renders only once after 1 second of inactivity, showing the final cumulative state
4. **Given** user is holding down an arrow key, **When** continuous key events are generated, **Then** each event resets the 1-second timer, and rendering occurs only 1 second after key release

---

### Edge Cases

- What happens when user switches between HUD and tree visualization displays rapidly during the debounce period?
- How does system handle object identification when objects are dynamically added or removed from the scene?
- **Keyboard input during active render**: If keyboard input occurs while rendering is already in progress, the system MUST queue the input and restart the debounce timer after the current render operation completes. This ensures no inputs are lost and prevents race conditions between rendering and input handling.
- How does system display objects when there are more than 99 of the same type (e.g., "sp-100", "sp-101")?
- What happens when debounce timer is active and user performs a non-keyboard action (e.g., mouse click)?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST display object identifiers in HUD using consistent format: two-letter lowercase type prefix followed by hyphen and sequential number (e.g., "sp-1", "pl-2", "cy-3")
- **FR-002**: System MUST display object identifiers in BVH tree leaf nodes using the exact same format as HUD (matching FR-001)
- **FR-003**: System MUST support object type prefixes in lowercase including: "sp" (sphere), "pl" (plane), "cy" (cylinder), and any other object types present in the scene
- **FR-004**: System MUST number objects of the same type sequentially starting from 1 (e.g., first sphere is "sp-1", second is "sp-2")
- **FR-005**: System MUST implement a debounce mechanism for keyboard input events that delays rendering actions
- **FR-006**: System MUST set debounce delay to 1 second after initial keyboard input
- **FR-007**: System MUST reset the debounce timer to 1 second whenever a new keyboard input occurs before the timer expires
- **FR-008**: System MUST execute only one render operation after the debounce timer expires without interruption
- **FR-009**: System MUST accumulate all keyboard inputs during the debounce period and apply them cumulatively to the final render
- **FR-010**: System MUST apply debouncing to navigation keyboard controls (arrow keys and similar navigation commands)
- **FR-011**: System MUST maintain consistent object numbering across HUD and tree visualization throughout the session
- **FR-012**: System MUST queue keyboard inputs that arrive during an active render operation and restart the debounce timer after render completion
- **FR-013**: System MUST NOT discard or lose keyboard inputs when they occur during rendering

### Key Entities

- **Object Identifier**: Represents a unique reference to a scene object, consisting of type prefix (2 letters), separator (hyphen), and sequential number (integer starting from 1)
- **Keyboard Input Event**: Represents a user keyboard action (key press/hold) that triggers navigation or control actions, with timestamp for debounce tracking
- **Debounce Timer**: Represents a countdown mechanism (1 second duration) that determines when to execute the accumulated rendering action, with state tracking (IDLE/PENDING/RENDERING) to handle concurrent events and microsecond timestamp precision for accurate timing
- **Timer Pool**: Pre-allocated fixed array of timer slots, with one slot per view/component that can trigger rendering, avoiding runtime memory allocation overhead during debounce operations
- **Input Queue**: Buffer that stores keyboard inputs received during active rendering operations, ensuring no inputs are lost when render is in progress

## Clarifications

### Session 2026-01-12

- Q: Should object type prefixes be uppercase (SP, PL, CY) or lowercase (sp, pl, cy)? → A: lowercase
- Q: Should debouncing apply to ALL keyboard events globally or ONLY navigation/view control keys? → A: Only navigation keys (arrow keys, view controls)
- Q: How should the debounce timer state be managed when multiple views/components can render? → A: Pre-allocated fixed pool (one timer slot per view/component, avoiding dynamic allocation)
- Q: What happens if a keyboard input occurs while rendering is already in progress? → A: Queue the input and restart debounce timer after render completes
- Q: How should the timer implementation be structured? → A: Use a dedicated timer structure with state tracking (IDLE/PENDING/RENDERING) and microsecond timestamp precision for accurate 1-second delays

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Users can identify the same object in both HUD and BVH tree visualization without mental conversion or reference lookup (100% format consistency)
- **SC-002**: Rapid keyboard navigation (10+ key presses within 2 seconds) results in only one rendering operation after input stops
- **SC-003**: Visual stuttering during keyboard navigation is eliminated, with smooth perceived performance (rendering occurs only after 1-second pause)
- **SC-004**: Users can navigate through BVH tree levels using keyboard controls with responsive feedback and final state accurately reflecting all accumulated inputs
- **SC-005**: Debugging time for correlating objects between HUD and tree visualization reduces by at least 50% through consistent identifier format
