/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb_shapes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:32:20 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include <math.h>

/**
 * @brief aabb for sphere 함수
 *
 * @param center 파라미터
 * @param radius 파라미터
 *
 * @return t_aabb 반환값
 */
t_aabb	aabb_for_sphere(t_vec3 center, double radius)
{
	t_aabb	box;
	t_vec3	offset;

	offset.x = radius;
	offset.y = radius;
	offset.z = radius;
	box.min = vec3_subtract(center, offset);
	box.max = vec3_add(center, offset);
	return (box);
}

t_aabb	aabb_for_cylinder(t_vec3 center, t_vec3 axis, double radius,
		double height)
{
	t_aabb	box;
	t_vec3	half_axis;
	double	extent;

	half_axis = vec3_multiply(axis, height / 2.0);
	extent = radius + fabs(half_axis.x);
	box.min.x = center.x - extent;
	box.max.x = center.x + extent;
	extent = radius + fabs(half_axis.y);
	box.min.y = center.y - extent;
	box.max.y = center.y + extent;
	extent = radius + fabs(half_axis.z);
	box.min.z = center.z - extent;
	box.max.z = center.z + extent;
	return (box);
}

/**
 * @brief aabb for plane 함수
 *
 * @param point 파라미터
 * @param normal 파라미터
 *
 * @return t_aabb 반환값
 */
t_aabb	aabb_for_plane(t_vec3 point, t_vec3 normal)
{
	t_aabb	box;
	double	large;

	large = 1000000.0;
	(void)normal;
	box.min.x = point.x - large;
	box.min.y = point.y - large;
	box.min.z = point.z - large;
	box.max.x = point.x + large;
	box.max.y = point.y + large;
	box.max.z = point.z + large;
	return (box);
}
