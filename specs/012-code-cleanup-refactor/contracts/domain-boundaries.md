# Domain Boundaries Contract

**Feature**: 012-code-cleanup-refactor  
**Date**: 2026-01-03  
**Version**: 1.0

## Purpose

This contract defines the architectural boundaries between functional domains, establishing clear dependency rules and organizational principles to maintain a clean, maintainable codebase structure.

## Scope

All functional domains in the miniRT project, their relationships, and the rules governing cross-domain interactions.

## Domain Definitions

### 1. Foundation Layer (Tier 0)

**Math Domain**
- **Header**: `vec3.h`
- **Source**: `src/math/`
- **Responsibility**: Vector mathematics, geometric calculations
- **Dependencies**: None (foundational)
- **Exports**:
  - Vector operations (add, subtract, multiply, divide)
  - Dot product, cross product
  - Length, normalize, distance
  - Reflection, projection
- **Boundary Rule**: MUST NOT depend on any other domain

---

### 2. Data Structure Layer (Tier 1)

**Object Domain**
- **Header**: `objects.h`
- **Source**: Object definitions embedded in various modules
- **Responsibility**: Scene object definitions (sphere, plane, cylinder)
- **Dependencies**: Math Domain
- **Exports**:
  - Object type definitions (`t_sphere`, `t_plane`, `t_cylinder`)
  - Object union/container structures
  - Object property accessors
- **Boundary Rule**: May depend only on Math Domain

**Ray Domain**
- **Header**: `ray.h`
- **Source**: `src/ray/`
- **Responsibility**: Ray structure and basic operations
- **Dependencies**: Math Domain
- **Exports**:
  - Ray structure (`t_ray`)
  - Ray generation functions
  - Ray-point calculations
- **Boundary Rule**: May depend only on Math Domain

---

### 3. Algorithm Layer (Tier 2)

**Spatial Domain**
- **Header**: `spatial.h`
- **Source**: `src/spatial/`
- **Responsibility**: BVH spatial acceleration
- **Dependencies**: Math, Object, Ray
- **Exports**:
  - BVH tree structures
  - BVH construction functions
  - BVH traversal/intersection
- **Boundary Rule**: May depend on Tier 0-1 domains only

**Lighting Domain**
- **Header**: `shadow.h`
- **Source**: `src/lighting/`
- **Responsibility**: Lighting calculations, shadows
- **Dependencies**: Math, Object, Ray
- **Exports**:
  - Shadow calculation functions
  - Light attenuation
  - Shadow ray generation
- **Boundary Rule**: May depend on Tier 0-1 domains only

**Parser Domain**
- **Header**: `parser.h`
- **Source**: `src/parser/`
- **Responsibility**: Scene file parsing
- **Dependencies**: Math, Object
- **Exports**:
  - Scene parsing functions
  - Element parsing (camera, light, objects)
  - Validation functions
- **Boundary Rule**: May depend on Tier 0-1 domains only

---

### 4. Rendering Layer (Tier 3)

**Rendering Domain**
- **Header**: `render_state.h`
- **Source**: `src/render/`
- **Responsibility**: Main rendering loop, integration
- **Dependencies**: Math, Object, Ray, Spatial, Lighting
- **Exports**:
  - Render state structures
  - Main rendering functions
  - Camera functions
  - Ray tracing logic
- **Boundary Rule**: May depend on Tier 0-2 domains

**Metrics Domain**
- **Header**: `metrics.h`
- **Source**: `src/render/metrics*.c`
- **Responsibility**: Performance tracking
- **Dependencies**: Rendering
- **Exports**:
  - Metrics structures
  - Performance calculation functions
  - Statistics tracking
- **Boundary Rule**: May depend on Rendering Domain

---

### 5. UI Layer (Tier 4)

**Window Domain**
- **Header**: `window.h`
- **Source**: `src/window/`
- **Responsibility**: MLX window management, events
- **Dependencies**: Rendering
- **Exports**:
  - Window structures
  - Event handlers
  - Window lifecycle functions
- **Boundary Rule**: May depend on Tier 0-3 domains

**HUD Domain**
- **Header**: `hud.h`
- **Source**: `src/hud/`
- **Responsibility**: Performance overlay display
- **Dependencies**: Window, Metrics
- **Exports**:
  - HUD structures
  - HUD rendering functions
  - HUD state management
- **Boundary Rule**: May depend on Window and Metrics domains

**KeyGuide Domain**
- **Header**: `keyguide.h`
- **Source**: `src/keyguide/`
- **Responsibility**: Interactive key guide
- **Dependencies**: Window
- **Exports**:
  - KeyGuide structures
  - KeyGuide rendering
  - KeyGuide lifecycle
- **Boundary Rule**: May depend on Window Domain

---

### 6. Application Layer (Tier 5)

**Core Domain**
- **Header**: `minirt.h`
- **Source**: `main.c`
- **Responsibility**: Application entry, main data structures
- **Dependencies**: All domains
- **Exports**:
  - Main program structure
  - Global scene structure
  - Application lifecycle
- **Boundary Rule**: Top-level integrator, may use all domains

---

## Dependency Rules

### Hierarchical Dependency Rule

**Rule**: A domain MAY depend on domains in lower tiers, but MUST NOT depend on domains in the same or higher tiers (except within-tier peer dependencies explicitly allowed).

**Tier Hierarchy**:
```
Tier 5: Core (Application)
  ↓ may depend on all below
Tier 4: Window, HUD, KeyGuide (UI)
  ↓ may depend on all below
Tier 3: Rendering, Metrics
  ↓ may depend on all below
Tier 2: Spatial, Lighting, Parser (Algorithms)
  ↓ may depend on all below
Tier 1: Object, Ray (Data Structures)
  ↓ may depend on all below
Tier 0: Math (Foundation)
  ↓ no dependencies
```

**Exception**: Metrics may depend on Rendering (same tier, explicit peer dependency).

---

### Acyclic Dependency Rule

**Rule**: Dependencies MUST form a Directed Acyclic Graph (DAG). No circular dependencies allowed.

**Examples**:
- ✅ Allowed: Math → Object → Spatial (forward only)
- ❌ Forbidden: Spatial → Math → Spatial (cycle)
- ❌ Forbidden: Rendering → Window → Rendering (cycle)

**Validation**:
```bash
# Check for circular includes (simplified)
# For each header, trace include chain and detect cycles
```

---

### Minimal Dependency Rule

**Rule**: Each domain MUST depend only on domains strictly necessary for its functionality.

**Examples**:
- ✅ Ray Domain depends on Math (needs vectors)
- ❌ Ray Domain depending on Parser (unnecessary coupling)
- ✅ HUD depends on Window and Metrics (needs both)
- ❌ HUD depending on Rendering (should use Metrics as intermediary)

---

## Boundary Violations

### What Constitutes a Violation?

1. **Upward Dependency**: Lower-tier domain depending on higher-tier domain
   - Example: Math including `render_state.h` ❌

2. **Circular Dependency**: Domain A depends on B, B depends on A
   - Example: Parser including Rendering, Rendering including Parser ❌

3. **Unnecessary Coupling**: Domain depending on another without clear need
   - Example: Parser including Window ❌

4. **Header Leakage**: Private headers from one domain exposed in another's public header
   - Example: `vec3.h` including implementation-specific headers ❌

---

### Detecting Violations

**Method 1: Include Analysis**
```bash
#!/bin/bash
# For each header, check included headers are from allowed dependencies

check_header() {
    local header=$1
    local domain=$2
    
    # Extract includes
    includes=$(grep "^#include \"" "$header" | sed 's/#include "\(.*\)"/\1/')
    
    # Check each include against allowed dependencies for this domain
    for inc in $includes; do
        if ! is_allowed_dependency "$domain" "$inc"; then
            echo "❌ Violation: $header includes $inc (not in allowed dependencies)"
        fi
    done
}
```

**Method 2: Dependency Graph Visualization**
- Generate graph of all domain dependencies
- Visually inspect for cycles or upward edges

**Method 3: Compilation Order Test**
- Attempt to compile domains in tier order
- Lower tiers should compile without higher tiers

---

## Cross-Domain Communication

### Allowed Communication Patterns

**1. Direct Function Call** (preferred for same or lower tier)
```c
// In src/lighting/shadow_calc.c
#include "vec3.h"

t_vec3 shadow_direction = vec3_subtract(light_pos, hit_point);
```

**2. Data Structure Passing** (for cross-tier data flow)
```c
// In src/render/render.c
#include "spatial.h"
#include "objects.h"

t_hit_info hit = bvh_intersect(bvh, ray);
```

**3. Callback Registration** (for upward notification)
```c
// In src/window/window.c
mlx_hook(window->mlx_win, EVENT_KEYPRESS, 0, handle_keypress, data);
```

---

### Forbidden Communication Patterns

**1. Upward Function Call** (lower tier calling higher tier directly)
```c
❌ In src/math/vector.c:
#include "render_state.h"  // VIOLATION: Math depending on Rendering
```

**2. Global State Access** (bypassing interfaces)
```c
❌ extern t_scene g_global_scene;  // Tight coupling
✅ Pass scene as parameter instead
```

**3. Friend Classes / Privileged Access** (C doesn't have, but conceptually)
```c
❌ Exposing internal structures outside domain boundary
✅ Use opaque pointers or accessor functions
```

---

## Refactoring Guidelines

### When Reorganizing Code

**Step 1: Identify Current Domain**
- Determine which domain the code logically belongs to
- Check function's dependencies

**Step 2: Check Dependency Compatibility**
- Does target domain already depend on required domains?
- Would move create new dependencies?
- Would move create cycles?

**Step 3: Move or Extract**
- **Move**: If code fits cleanly in target domain
- **Extract**: If code needs new domain or intermediary

**Step 4: Update Includes**
- Move declarations to appropriate headers
- Update include paths
- Remove unnecessary includes

**Step 5: Validate**
- Compile and test
- Check no new violations introduced

---

### When Creating New Functions

**Decision Tree**:
```
New function needed
  ↓
What domain is it related to?
  ├─ Existing domain → Add to that domain
  └─ Spans multiple domains
      ├─ Belongs to higher tier → Add to integration layer (Rendering/Core)
      └─ New concept → Create new domain (rare, needs justification)
  ↓
What does it depend on?
  ├─ Only lower tiers → OK, proceed
  ├─ Same tier → Needs justification (peer dependency)
  └─ Higher tier → VIOLATION, redesign needed
  ↓
Add declaration to domain's header
Add implementation to domain's source
```

---

## Validation Tests

### Test 1: Include Chain Validation

```bash
#!/bin/bash
echo "Validating include chains..."

# Define allowed dependencies (domain → allowed includes)
declare -A allowed
allowed[vec3.h]=""  # Math: no dependencies
allowed[objects.h]="vec3.h"  # Object: depends on Math
allowed[ray.h]="vec3.h"  # Ray: depends on Math
allowed[spatial.h]="vec3.h objects.h ray.h"  # Spatial: Tier 0-1
allowed[shadow.h]="vec3.h objects.h ray.h"  # Lighting: Tier 0-1
allowed[parser.h]="vec3.h objects.h"  # Parser: Tier 0-1
allowed[render_state.h]="vec3.h objects.h ray.h spatial.h shadow.h"  # Rendering: Tier 0-2
allowed[metrics.h]="render_state.h"  # Metrics: depends on Rendering
allowed[window.h]="render_state.h"  # Window: depends on Rendering
allowed[hud.h]="window.h metrics.h"  # HUD: depends on Window, Metrics
allowed[keyguide.h]="window.h"  # KeyGuide: depends on Window
allowed[minirt.h]="*"  # Core: may include all

VIOLATIONS=0
for header in includes/*.h; do
    basename=$(basename "$header")
    includes=$(grep "^#include \"" "$header" | sed 's/#include "\(.*\)"/\1/')
    
    for inc in $includes; do
        # Check if include is in allowed list for this header
        if [[ "${allowed[$basename]}" != "*" ]] && ! [[ "${allowed[$basename]}" =~ "$inc" ]]; then
            echo "❌ $basename includes $inc (not in allowed: ${allowed[$basename]})"
            VIOLATIONS=$((VIOLATIONS + 1))
        fi
    done
done

if [ "$VIOLATIONS" -eq 0 ]; then
    echo "✅ PASS: No dependency violations"
    exit 0
else
    echo "❌ FAIL: $VIOLATIONS dependency violations"
    exit 1
fi
```

---

### Test 2: Circular Dependency Detection

```bash
#!/bin/bash
echo "Checking for circular dependencies..."

# For each header, build transitive include set
# Check if any header appears in its own transitive includes

# (Simplified version - real implementation would use graph algorithm)
detect_cycles() {
    local header=$1
    local path=$2
    
    if [[ "$path" =~ "$header" ]]; then
        echo "❌ Circular dependency detected: $path → $header"
        return 1
    fi
    
    includes=$(grep "^#include \"" "includes/$header" | sed 's/#include "\(.*\)"/\1/')
    for inc in $includes; do
        detect_cycles "$inc" "$path → $inc"
    done
}

for header in includes/*.h; do
    detect_cycles "$(basename $header)" ""
done

echo "✅ No circular dependencies detected"
```

---

## Success Criteria (from Spec)

| Criterion | Requirement | Validation Method |
|-----------|-------------|-------------------|
| **SC-005** | Zero mixed-domain headers | Manual review against domain definitions |
| **SC-006** | ≤2 files per domain | Directory structure audit |
| **FR-006** | Related code grouped together | Verify functions match domain responsibility |
| **FR-007** | Separated by functional domain | Audit against 12 defined domains |
| **FR-008** | Single domain per header | Check header includes match tier rules |

---

## Contract Guarantees

Upon successful completion:

1. ✅ **All domains have clear boundaries** with documented responsibilities
2. ✅ **Dependencies form a DAG** (no circular dependencies)
3. ✅ **Tier rules are enforced** (no upward dependencies)
4. ✅ **Each header represents one domain** (no mixed concerns) (FR-008)
5. ✅ **Related code is grouped** within domain boundaries (FR-006)
6. ✅ **Domains are separated** by functional concern (FR-007)
7. ✅ **Include chains are validated** and compliant

---

## Maintenance Guidelines

### For Future Development

When modifying code:

1. ✅ Identify which domain the change affects
2. ✅ Verify change doesn't introduce new cross-domain dependencies
3. ✅ Keep code within domain boundaries
4. ✅ If new dependency needed, verify tier compatibility
5. ✅ Run validation tests after changes

When adding features:

1. ✅ Determine appropriate domain or if new domain needed
2. ✅ Document domain relationships if creating new domain
3. ✅ Update this contract with new domain definitions
4. ✅ Ensure new domain follows tier rules

---

## References

- Feature spec: `specs/012-code-cleanup-refactor/spec.md` (FR-006, FR-007, FR-008)
- Success criteria: SC-005, SC-006
- Data model: `specs/012-code-cleanup-refactor/data-model.md` (Domain Dependency Model)
- Research: `specs/012-code-cleanup-refactor/research.md` (Functional Domain Identification)

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-03 | Initial contract defining 12 domains across 6 tiers |
