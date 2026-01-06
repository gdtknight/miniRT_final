/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics_calc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/31 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "metrics.h"

/**
 * @brief calculate fps 함수 - 계산 수행
 *
 * @param m 파라미터
 *
 * @return double 반환값
 */
double	calculate_fps(t_metrics *m)
{
	return (m->fps);
}

/**
 * @brief calculate bvh efficiency 함수 - 계산 수행
 *
 * @param m 파라미터
 * @param object_count 파라미터
 *
 * @return double 반환값
 */
double	calculate_bvh_efficiency(t_metrics *m, int object_count)
{
	long	naive_tests;
	double	efficiency;

	if (object_count == 0 || m->rays_traced == 0)
		return (0.0);
	naive_tests = m->rays_traced * object_count;
	if (naive_tests == 0)
		return (0.0);
	efficiency = 1.0 - ((double)m->intersect_tests / naive_tests);
	if (efficiency < 0.0)
		return (0.0);
	return (efficiency * 100.0);
}

/**
 * @brief calculate avg tests per ray 함수 - 계산 수행
 *
 * @param m 파라미터
 *
 * @return double 반환값
 */
double	calculate_avg_tests_per_ray(t_metrics *m)
{
	if (m->rays_traced == 0)
		return (0.0);
	return ((double)m->intersect_tests / m->rays_traced);
}
