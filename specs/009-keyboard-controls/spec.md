# Feature Specification: Keyboard Control Reorganization

**Feature Branch**: `009-keyboard-controls`  
**Created**: 2026-01-01  
**Status**: Draft  
**Input**: User description: "키보드 컨트롤 재구성 - 카메라 및 오브젝트 조작 개선"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Camera Navigation with Vertical Movement (Priority: P1)

Users need to navigate the 3D scene freely in all six directions (forward/backward, left/right, up/down) to position the camera for optimal viewing angles. The current system lacks dedicated vertical movement keys, forcing users to rotate the camera to move vertically, which is unintuitive.

**Why this priority**: Core navigation functionality that directly impacts usability. Without vertical movement, users cannot efficiently position cameras for overhead or underside views of objects.

**Independent Test**: Can be fully tested by loading any scene, pressing the six directional keys (W/X for forward/back, A/D for left/right, Q/Z for up/down), and verifying the camera moves in the expected world-space directions without needing to rotate first.

**Acceptance Scenarios**:

1. **Given** a scene is loaded with the camera at default position, **When** user presses W key, **Then** camera moves forward in the direction it is facing by 1.0 unit
2. **Given** camera is facing any direction, **When** user presses X key, **Then** camera moves backward (opposite to facing direction) by 1.0 unit
3. **Given** camera is at any position, **When** user presses A key, **Then** camera strafes left (perpendicular to facing direction) by 1.0 unit
4. **Given** camera is at any position, **When** user presses D key, **Then** camera strafes right (perpendicular to facing direction) by 1.0 unit
5. **Given** camera is at any position and any orientation, **When** user presses Q key, **Then** camera moves upward in world space (absolute Y+ direction) by 1.0 unit regardless of camera orientation
6. **Given** camera is at any position and any orientation, **When** user presses Z key, **Then** camera moves downward in world space (absolute Y- direction) by 1.0 unit regardless of camera orientation

---

### User Story 2 - Camera Rotation and Reset (Priority: P1)

Users need to rotate the camera view (pitch) and return to the initial viewpoint when they lose orientation or want to start fresh from the default scene view.

**Why this priority**: Essential for camera control completeness. The reset function prevents users from getting lost in 3D space and provides a quick way to return to the original scene composition.

**Independent Test**: Can be fully tested by rotating the camera with E/C keys to verify pitch changes, then pressing S to confirm camera returns to exact initial position and orientation stored when the scene was loaded.

**Acceptance Scenarios**:

1. **Given** camera is at any position, **When** user presses E key, **Then** camera pitches upward (looks up) by 5 degrees
2. **Given** camera is at any position, **When** user presses C key, **Then** camera pitches downward (looks down) by 5 degrees
3. **Given** scene is initially loaded, **When** system loads scene, **Then** initial camera position and direction are stored in memory
4. **Given** camera has been moved and rotated from initial position, **When** user presses S key, **Then** camera position and direction are restored to exact initial values
5. **Given** camera is already at initial position, **When** user presses S key, **Then** nothing changes (idempotent operation)

---

### User Story 3 - Object Movement with Main Keyboard (Priority: P2)

Users need to manipulate selected objects in 3D space using the main keyboard instead of numpad, as many laptops lack a physical numpad and the numpad-only design limits usability.

**Why this priority**: Critical for laptop users but not blocking basic scene viewing. Users can work around by using external keyboards, but it significantly degrades user experience for a large user segment.

**Independent Test**: Can be fully tested by selecting an object with bracket keys, then using R/F/T/G/Y/N keys to move the object along all six axes and verifying position changes in the scene.

**Acceptance Scenarios**:

1. **Given** an object is selected, **When** user presses R key, **Then** object moves left (X- direction) by 1.0 unit
2. **Given** an object is selected, **When** user presses F key, **Then** object moves right (X+ direction) by 1.0 unit
3. **Given** an object is selected, **When** user presses T key, **Then** object moves down (Y- direction) by 1.0 unit
4. **Given** an object is selected, **When** user presses G key, **Then** object moves up (Y+ direction) by 1.0 unit
5. **Given** an object is selected, **When** user presses Y key, **Then** object moves backward (Z- direction) by 1.0 unit
6. **Given** an object is selected, **When** user presses N key, **Then** object moves forward (Z+ direction) by 1.0 unit
7. **Given** no object is selected, **When** user presses any object movement key, **Then** no action occurs and no error is shown

---

### User Story 4 - Documentation and On-Screen Guidance (Priority: P3)

Users need clear documentation and on-screen key guides to discover and remember the new keyboard controls without trial-and-error or memorization.

**Why this priority**: Enhances discoverability but users can still use the feature without it. Can be delivered after core functionality is proven stable.

**Independent Test**: Can be fully tested by reviewing CONTROLS.md documentation, toggling the HUD display with H key, and verifying all new key bindings are accurately documented and displayed.

**Acceptance Scenarios**:

1. **Given** user opens CONTROLS.md file, **When** user reads camera movement section, **Then** all six movement keys (W/X/A/D/Q/Z) are documented with their directional functions
2. **Given** user opens CONTROLS.md file, **When** user reads camera rotation section, **Then** E/C pitch keys and S reset key are documented
3. **Given** user opens CONTROLS.md file, **When** user reads object movement section, **Then** R/F/T/G/Y/N keys are documented with their axis movements
4. **Given** application is running, **When** user presses H key, **Then** on-screen key guide displays all camera and object control keys with current mappings
5. **Given** HUD is showing key guide, **When** user presses H key again, **Then** key guide is hidden (toggle behavior)

---

### Edge Cases

- What happens when camera reaches scene boundaries or coordinate system limits during movement?
- How does system handle rapid repeated key presses (key repeat rate)?
- What happens if user tries to pitch camera beyond vertical limits (straight up/down)?
- How does camera reset behave if scene is dynamically modified after initial load?
- What happens when user presses both conflicting movement keys simultaneously (e.g., W and X)?
- How does system handle platform-specific key code differences between macOS and Linux?
- What happens if initial camera state fails to save due to memory constraints?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST map camera forward movement to W key (existing behavior maintained)
- **FR-002**: System MUST map camera backward movement to X key (changed from S key)
- **FR-003**: System MUST map camera left strafe to A key (existing behavior maintained)
- **FR-004**: System MUST map camera right strafe to D key (existing behavior maintained)
- **FR-005**: System MUST map camera upward world-space movement to Q key (new functionality)
- **FR-006**: System MUST map camera downward world-space movement to Z key (new functionality)
- **FR-007**: System MUST implement Q/Z vertical movement as absolute world Y-axis translation regardless of camera orientation
- **FR-008**: System MUST map camera pitch up (look upward) to E key (changed from R key)
- **FR-009**: System MUST map camera pitch down (look downward) to C key (changed from F key)
- **FR-010**: System MUST map camera reset to S key (new functionality replacing backward movement)
- **FR-011**: System MUST store initial camera position and direction when scene is loaded
- **FR-012**: System MUST restore camera to exact initial position and direction when S key is pressed
- **FR-013**: System MUST remove adaptive quality toggle functionality from Q key
- **FR-014**: System MUST map selected object X- movement to R key (changed from Numpad 4)
- **FR-015**: System MUST map selected object X+ movement to F key (changed from Numpad 6)
- **FR-016**: System MUST map selected object Y- movement to T key (changed from Numpad 2)
- **FR-017**: System MUST map selected object Y+ movement to G key (changed from Numpad 8)
- **FR-018**: System MUST map selected object Z- movement to Y key (changed from Numpad 1)
- **FR-019**: System MUST map selected object Z+ movement to N key (changed from Numpad 3)
- **FR-020**: System MUST maintain H key for HUD toggle functionality (no change)
- **FR-021**: System MUST maintain I key for detailed info toggle functionality (no change)
- **FR-022**: System MUST maintain B key for BVH toggle functionality (no change)
- **FR-023**: System MUST maintain bracket keys [ and ] for object selection (no change)
- **FR-024**: System MUST maintain ESC key for program exit (no change)
- **FR-025**: System MUST maintain TAB key for next object selection (no change)
- **FR-026**: System MUST support all key mappings on both macOS and Linux platforms
- **FR-027**: System MUST define platform-specific key codes for new keys: X, E, C, Z, N, T, G, Y (where not already defined)
- **FR-028**: System MUST update on-screen key guide display to reflect all new key mappings
- **FR-029**: System MUST update CONTROLS.md documentation file with all new key mappings
- **FR-030**: System MUST update KEYBOARD_LAYOUT.txt file (if exists) with all new key mappings
- **FR-031**: System MUST use movement step of 1.0 unit for all camera translations (consistent with existing)
- **FR-032**: System MUST use rotation angle of 5 degrees for camera pitch operations (consistent with existing)
- **FR-033**: System MUST use movement step of 1.0 unit for all object translations (consistent with existing)
- **FR-034**: System MUST trigger scene re-rendering after any camera movement, rotation, or reset operation
- **FR-035**: System MUST trigger scene re-rendering after any object movement operation

### Key Entities *(include if feature involves data)*

- **Camera State**: Represents current camera position (3D vector) and direction (3D normalized vector) in world space
- **Initial Camera State**: Stored snapshot of camera position and direction at scene load time for reset functionality
- **Selected Object**: Reference to currently selected object (type: sphere/plane/cylinder, index in array) that receives movement commands
- **Key Binding**: Mapping between platform-specific key codes and application functions (e.g., KEY_Q → camera_up_movement)

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Users can navigate camera in all six world-space directions (forward/back, left/right, up/down) using single key presses without requiring camera rotation first
- **SC-002**: Camera returns to exact initial position and orientation within 0.001 units of precision when reset key is pressed
- **SC-003**: Users can perform all object manipulation tasks (6-axis movement) using only the main keyboard area without requiring numpad access
- **SC-004**: All 11 new/modified key bindings (W, X, A, D, Q, Z, E, C, S for camera; R, F, T, G, Y, N for objects) work identically on both macOS and Linux platforms
- **SC-005**: On-screen HUD displays correct key mappings for all camera and object controls without showing outdated bindings
- **SC-006**: Documentation (CONTROLS.md) accurately reflects all new key mappings and can be understood by users without technical knowledge
- **SC-007**: Users can complete a typical workflow (navigate scene, select object, move object, reset camera) using only the redesigned keyboard controls within 30 seconds
- **SC-008**: No memory leaks occur during repeated camera reset operations (tested over 1000 iterations)
- **SC-009**: Key conflicts are eliminated - no single key triggers multiple unintended actions simultaneously
- **SC-010**: Users spend less than 10 seconds locating the correct key for each common operation after reading the documentation once
