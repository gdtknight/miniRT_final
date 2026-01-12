/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:32:29 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"

/**
 * @brief update bounds 함수 - 업데이트 수행
 *
 * @param tmin 파라미터
 * @param tmax 파라미터
 * @param t0 파라미터
 * @param t1 파라미터
 */
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

/**
 * @brief aabb intersect 함수 - 교차 검사 수행
 *
 * @param box 파라미터
 * @param ray 파라미터
 * @param t_min 파라미터
 * @param t_max 파라미터
 *
 * @return int 반환값
 */
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
