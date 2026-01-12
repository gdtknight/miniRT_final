# API Contract: Object Identifier Formatting

**Module**: Object Identifier Utilities  
**Header**: `includes/format_object_id.h`  
**Implementation**: `src/utils/format_object_id.c`

---

## Function: format_object_id

### Signature

```c
int format_object_id(char *buffer, t_obj_type type, int index);
```

### Purpose

Format a scene object identifier into a consistent lowercase string representation (e.g., "sp-1", "pl-2", "cy-3") for display in HUD and BVH visualization.

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| buffer | char* | out | Destination buffer for formatted identifier (minimum 8 bytes) |
| type | t_obj_type | in | Object type enum (OBJ_SPHERE, OBJ_PLANE, OBJ_CYLINDER) |
| index | int | in | Zero-based array index (will be converted to 1-based display number) |

### Return Value

| Value | Meaning |
|-------|---------|
| n > 0 | Number of characters written to buffer (excluding null terminator) |
| 0 | Error (invalid type or buffer is NULL) |

### Preconditions

- `buffer` must not be NULL
- `buffer` must be at least 8 bytes to accommodate "cy-100\0"
- `type` must be valid enum value (OBJ_SPHERE, OBJ_PLANE, OBJ_CYLINDER)
- `index` must be non-negative (0 to 99)

### Postconditions

- Buffer contains null-terminated identifier string in format: `[prefix]-[number]`
- Number is 1-based (index 0 → "sp-1", index 1 → "sp-2")
- Return value matches strlen(buffer)

### Examples

```c
char id[16];
int len;

/* Sphere at array index 0 */
len = format_object_id(id, OBJ_SPHERE, 0);
// Result: id = "sp-1", len = 4

/* Plane at array index 5 */
len = format_object_id(id, OBJ_PLANE, 5);
// Result: id = "pl-6", len = 4

/* Cylinder at array index 99 */
len = format_object_id(id, OBJ_CYLINDER, 99);
// Result: id = "cy-100", len = 6

/* Invalid type */
len = format_object_id(id, 999, 0);
// Result: len = 0, id unchanged
```

### Error Handling

| Error Condition | Behavior |
|-----------------|----------|
| buffer is NULL | Return 0, no crash |
| type is invalid | Return 0, buffer unchanged |
| index < 0 | Return 0, buffer unchanged |
| index > 99 | Proceed anyway (future-proofing for larger scenes) |

### Performance

- **Time Complexity**: O(1) - fixed number of operations
- **Space Complexity**: O(1) - no dynamic allocation
- **Typical Execution**: <10 CPU cycles (sprintf/snprintf with fixed format)

### Thread Safety

- Function is thread-safe (no global state)
- Caller must ensure buffer is not shared between threads without synchronization

### Norminette Compliance

- Function body < 25 lines
- Variable names follow norm (no camelCase)
- No forbidden constructs

---

## Function: get_object_type_prefix

### Signature

```c
const char *get_object_type_prefix(t_obj_type type);
```

### Purpose

Return the two-letter lowercase prefix for a given object type (internal helper).

### Parameters

| Parameter | Type | Direction | Description |
|-----------|------|-----------|-------------|
| type | t_obj_type | in | Object type enum |

### Return Value

| Value | Meaning |
|-------|---------|
| "sp" | Sphere prefix |
| "pl" | Plane prefix |
| "cy" | Cylinder prefix |
| NULL | Invalid type |

### Examples

```c
const char *prefix;

prefix = get_object_type_prefix(OBJ_SPHERE);   // "sp"
prefix = get_object_type_prefix(OBJ_PLANE);    // "pl"
prefix = get_object_type_prefix(OBJ_CYLINDER); // "cy"
prefix = get_object_type_prefix(999);          // NULL
```

---

## Integration Points

### HUD Rendering

**Location**: `src/hud/hud_obj_render.c`

**Before** (Current):
```c
void render_sphere_obj(t_render *render, int idx, int *y, int color)
{
    char buf[128];
    int i = 0;
    
    buf[i++] = 'S';
    buf[i++] = 'p';
    buf[i++] = 'h';
    buf[i++] = 'e';
    buf[i++] = 'r';
    buf[i++] = 'e';
    buf[i++] = ' ';
    buf[i++] = '#';
    buf[i++] = '0' + idx + 1;  /* Limited to 9 objects */
    // ...
}
```

**After** (Updated):
```c
void render_sphere_obj(t_render *render, int idx, int *y, int color)
{
    char buf[128];
    char id[16];
    int len;
    
    format_object_id(id, OBJ_SPHERE, idx);
    len = 0;
    len += copy_str_to_buf(buf + len, id);
    buf[len++] = ':';
    buf[len++] = ' ';
    // ... continue with position data
}
```

### BVH Leaf Node Rendering

**Location**: `src/bvh_vis/bvh_vis_node.c`

**Before** (Current):
```c
void format_object_list(t_object_ref *objects, int count, char *buffer)
{
    int i;
    char temp[16];
    
    strcpy(buffer, "Objects: [");
    i = 0;
    while (i < count)
    {
        snprintf(temp, sizeof(temp), "%d", objects[i].index);  /* Raw index */
        strcat(buffer, temp);
        if (i < count - 1)
            strcat(buffer, ", ");
        i++;
    }
    strcat(buffer, "]");
}
```

**After** (Updated):
```c
void format_object_list(t_object_ref *objects, int count, char *buffer)
{
    int i;
    char id[16];
    
    strcpy(buffer, "Objects: [");
    i = 0;
    while (i < count)
    {
        format_object_id(id, objects[i].type, objects[i].index);
        strcat(buffer, id);
        if (i < count - 1)
            strcat(buffer, ", ");
        i++;
    }
    strcat(buffer, "]");
}
```

---

## Testing Contract

### Unit Tests

```c
/* Test basic formatting */
void test_format_sphere_id(void)
{
    char buf[16];
    int len;
    
    len = format_object_id(buf, OBJ_SPHERE, 0);
    assert(len == 4);
    assert(strcmp(buf, "sp-1") == 0);
}

/* Test plane formatting */
void test_format_plane_id(void)
{
    char buf[16];
    int len;
    
    len = format_object_id(buf, OBJ_PLANE, 5);
    assert(len == 4);
    assert(strcmp(buf, "pl-6") == 0);
}

/* Test cylinder formatting */
void test_format_cylinder_id(void)
{
    char buf[16];
    int len;
    
    len = format_object_id(buf, OBJ_CYLINDER, 99);
    assert(len == 6);
    assert(strcmp(buf, "cy-100") == 0);
}

/* Test error handling */
void test_format_invalid_type(void)
{
    char buf[16] = "unchanged";
    int len;
    
    len = format_object_id(buf, 999, 0);
    assert(len == 0);
}

/* Test NULL buffer */
void test_format_null_buffer(void)
{
    int len;
    
    len = format_object_id(NULL, OBJ_SPHERE, 0);
    assert(len == 0);
}
```

### Integration Tests

```c
/* Test HUD-BVH consistency */
void test_identifier_consistency(void)
{
    t_scene scene;
    t_render render;
    char hud_id[16];
    char bvh_id[16];
    
    /* Setup scene with 3 spheres */
    scene.sphere_count = 3;
    
    /* Get identifier from HUD rendering logic */
    format_object_id(hud_id, OBJ_SPHERE, 1);
    
    /* Get identifier from BVH leaf node logic */
    format_object_id(bvh_id, OBJ_SPHERE, 1);
    
    /* Verify consistency */
    assert(strcmp(hud_id, bvh_id) == 0);
    assert(strcmp(hud_id, "sp-2") == 0);
}
```

---

## Dependencies

### Required Headers
- `includes/objects.h` (for t_obj_type enum)
- `<stdio.h>` (for snprintf)
- `<string.h>` (for strlen)

### Allowed Functions (42 School)
- `snprintf` (standard C library - formatting)
- `strlen` (standard C library - string length)

### No Dependencies On
- pthread (timer-related code separate)
- malloc/free (stack-allocated buffers only)
- External libraries

---

## Versioning

- **Version**: 1.0.0
- **Date**: 2026-01-12
- **Stability**: Stable (no breaking changes planned)

---

## Change History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2026-01-12 | Initial API definition |

---

## Notes

- This API is **internal** to miniRT; not exposed to external consumers
- Identifier format may be extended in future (e.g., "lt-1" for lights) without breaking existing code
- Buffer size requirement (8 bytes) accounts for 3-digit numbers (up to 999 objects per type)
