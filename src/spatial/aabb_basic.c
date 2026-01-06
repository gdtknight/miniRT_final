/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb_basic.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"

double	min_double(double a, double b)
{
	if (a < b)
		return (a);
	return (b);
}

double	max_double(double a, double b)
{
	if (a > b)
		return (a);
	return (b);
}

t_aabb	aabb_create(t_vec3 min, t_vec3 max)
{
	t_aabb	box;

	box.min = min;
	box.max = max;
	return (box);
}

t_aabb	aabb_merge(t_aabb a, t_aabb b)
{
	t_aabb	result;

	result.min.x = min_double(a.min.x, b.min.x);
	result.min.y = min_double(a.min.y, b.min.y);
	result.min.z = min_double(a.min.z, b.min.z);
	result.max.x = max_double(a.max.x, b.max.x);
	result.max.y = max_double(a.max.y, b.max.y);
	result.max.z = max_double(a.max.z, b.max.z);
	return (result);
}

double	aabb_surface_area(t_aabb box)
{
	double	dx;
	double	dy;
	double	dz;

	dx = box.max.x - box.min.x;
	dy = box.max.y - box.min.y;
	dz = box.max.z - box.min.z;
	return (2.0 * (dx * dy + dy * dz + dz * dx));
}
