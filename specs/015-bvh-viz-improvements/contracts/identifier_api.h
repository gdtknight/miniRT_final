/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   identifier_api.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IDENTIFIER_API_H
# define IDENTIFIER_API_H

/**
 * API Contract: Object Identifier Formatting
 * Feature: 015-bvh-viz-improvements
 * Version: 1.0.0
 *
 * Purpose:
 *   Provide consistent lowercase object identifier formatting (sp-1, pl-2, cy-3)
 *   across HUD and BVH visualization displays.
 *
 * Contract Guarantees:
 *   - Format: [type][sep][num] where type is 2 lowercase letters
 *   - Separator: hyphen character '-'
 *   - Numbering: 1-based sequential (first object is #1)
 *   - Stability: Identifiers remain constant during session
 *   - Independence: Each object type has separate numbering
 *
 * 42 School Constraints:
 *   - Uses only allowed functions: snprintf (stdio.h)
 *   - No dynamic memory allocation (buffer provided by caller)
 *   - No external libraries required
 */

/**
 * @brief Format object identifier into buffer
 *
 * Formats object identifier using consistent lowercase prefix format.
 * Output format: [type]-[num] (e.g., "sp-1", "pl-2", "cy-3")
 *
 * @param buffer Output buffer (must be at least 16 bytes)
 * @param type Object type enum (OBJ_SPHERE, OBJ_PLANE, OBJ_CYLINDER)
 * @param index Array index (0-based, will be converted to 1-based display)
 *
 * @return Number of characters written (excluding null terminator)
 *         Returns -1 on error (invalid type or NULL buffer)
 *
 * @pre buffer must be non-NULL and at least 16 bytes
 * @pre type must be valid enum value (0=sphere, 1=plane, 2=cylinder)
 * @pre index must be >= 0 and < 100 (implementation limit)
 *
 * @post buffer contains null-terminated identifier string
 * @post return value matches strlen(buffer) on success
 *
 * Example:
 *   char id[16];
 *   int len = format_object_id(id, OBJ_SPHERE, 0);
 *   // id = "sp-1", len = 4
 *
 * Thread Safety: Safe (no shared state)
 * Complexity: O(1) time, O(1) space
 */
int	format_object_id(char *buffer, int type, int index);

/**
 * @brief Get type prefix string for object type
 *
 * Returns lowercase 2-letter prefix for object type.
 *
 * @param type Object type enum
 *
 * @return Pointer to static string ("sp", "pl", "cy")
 *         Returns "??" for invalid type
 *
 * @pre type is valid enum value
 * @post Return value points to static constant string
 *
 * Example:
 *   const char *prefix = get_type_prefix(OBJ_SPHERE);
 *   // prefix = "sp"
 *
 * Thread Safety: Safe (returns pointer to static constant)
 * Complexity: O(1) time, O(1) space
 */
const char	*get_type_prefix(int type);

/**
 * @brief Convert 0-based array index to 1-based display number
 *
 * Helper function for converting internal array indices to user-visible
 * object numbers (first object is #1, not #0).
 *
 * @param index Array index (0-based)
 *
 * @return Display number (1-based)
 *
 * @pre index >= 0
 * @post return value = index + 1
 *
 * Example:
 *   int display_num = index_to_display_number(0);
 *   // display_num = 1
 *
 * Thread Safety: Safe (no state)
 * Complexity: O(1) time, O(1) space
 */
int	index_to_display_number(int index);

/**
 * Type Constants
 *
 * Object type enum values used throughout miniRT codebase.
 * These must match the definitions in objects.h or selection system.
 */
# define OBJ_SPHERE		0
# define OBJ_PLANE		1
# define OBJ_CYLINDER	2

/**
 * Buffer Size Requirements
 *
 * Minimum buffer sizes required for identifier operations.
 */
# define ID_BUFFER_SIZE	16	/* Minimum buffer for format_object_id */
# define PREFIX_LEN		2	/* Length of type prefix (sp, pl, cy) */
# define MAX_DISPLAY_NUM	999	/* Maximum supported display number */

#endif
