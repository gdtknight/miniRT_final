/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metrics_frame.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "metrics.h"
#include <stdlib.h>

/**
 * @brief metrics init 함수 - 초기화 수행
 *
 * @param metrics 파라미터
 */
void	metrics_init(t_metrics *metrics)
{
	int	i;

	metrics->render_time_us = 0;
	metrics->frame_count = 0;
	metrics->fps = 0.0;
	metrics->rays_traced = 0;
	metrics->intersect_tests = 0;
	metrics->bvh_nodes_visited = 0;
	metrics->bvh_tests_skipped = 0;
	metrics->quality_mode = 0;
	i = 0;
	while (i < 60)
	{
		metrics->frame_times_us[i] = 0;
		i++;
	}
	metrics->frame_index = 0;
}

/**
 * @brief metrics start frame 함수
 *
 * @param metrics 파라미터
 */
void	metrics_start_frame(t_metrics *metrics)
{
	metrics->rays_traced = 0;
	metrics->intersect_tests = 0;
	metrics->bvh_nodes_visited = 0;
	metrics->bvh_tests_skipped = 0;
	metrics->bvh_box_tests = 0;
	timer_start(&metrics->start_time);
}

/**
 * @brief calculate fps internal 함수 - 계산 수행
 *
 * @param m 파라미터
 *
 * @return double 반환값
 */
static double	calculate_fps_internal(t_metrics *m)
{
	long	sum;
	int		i;
	int		valid_frames;

	sum = 0;
	i = 0;
	valid_frames = 0;
	while (i < 60)
	{
		if (m->frame_times_us[i] > 0)
		{
			sum += m->frame_times_us[i];
			valid_frames++;
		}
		i++;
	}
	if (valid_frames == 0 || sum == 0)
		return (0.0);
	return (1000000.0 / (sum / (double)valid_frames));
}

/**
 * @brief metrics end frame 함수
 *
 * @param metrics 파라미터
 */
void	metrics_end_frame(t_metrics *metrics)
{
	metrics->render_time_us = timer_elapsed_us(&metrics->start_time);
	metrics->frame_times_us[metrics->frame_index] = metrics->render_time_us;
	metrics->frame_index = (metrics->frame_index + 1) % 60;
	metrics->frame_count++;
	metrics->fps = calculate_fps_internal(metrics);
}

/**
 * @brief metrics log render 함수 - 렌더링 수행
 *
 * @param metrics 파라미터
 */
void	metrics_log_render(t_metrics *metrics)
{
	(void)metrics;
}
