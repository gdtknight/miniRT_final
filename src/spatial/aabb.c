/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"

static void	update_bounds(double *tmin, double *tmax, double t0, double t1)
{
	if (t0 > t1)
	{
		*tmin = max_double(*tmin, t1);
		*tmax = min_double(*tmax, t0);
	}
	else
	{
		*tmin = max_double(*tmin, t0);
		*tmax = min_double(*tmax, t1);
	}
}

int	aabb_intersect(t_aabb box, t_ray ray, double *t_min, double *t_max)
{
	double	t[2];
	double	tmin;
	double	tmax;

	tmin = *t_min;
	tmax = *t_max;
	t[0] = (box.min.x - ray.origin.x) / ray.direction.x;
	t[1] = (box.max.x - ray.origin.x) / ray.direction.x;
	update_bounds(&tmin, &tmax, t[0], t[1]);
	t[0] = (box.min.y - ray.origin.y) / ray.direction.y;
	t[1] = (box.max.y - ray.origin.y) / ray.direction.y;
	update_bounds(&tmin, &tmax, t[0], t[1]);
	t[0] = (box.min.z - ray.origin.z) / ray.direction.z;
	t[1] = (box.max.z - ray.origin.z) / ray.direction.z;
	update_bounds(&tmin, &tmax, t[0], t[1]);
	*t_min = tmin;
	*t_max = tmax;
	return (tmax >= tmin && tmax > 0);
}
