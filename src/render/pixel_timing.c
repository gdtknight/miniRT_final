/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pixel_timing.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 01:28:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 01:28:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pixel_timing.h"
#include "metrics.h"
#include <stdlib.h>
#include <limits.h>

void	pixel_timing_init(t_pixel_timing *timing)
{
	timing->capacity = MAX_PIXEL_SAMPLES;
	timing->samples = malloc(sizeof(long) * timing->capacity);
	timing->count = 0;
	timing->min_time = LONG_MAX;
	timing->max_time = 0;
	timing->total_time = 0;
	timing->enabled = 1;
}

void	pixel_timing_cleanup(t_pixel_timing *timing)
{
	if (timing->samples)
	{
		free(timing->samples);
		timing->samples = NULL;
	}
	timing->count = 0;
	timing->enabled = 0;
}

void	pixel_timing_add_sample(t_pixel_timing *timing, long time_ns)
{
	if (!timing->enabled || !timing->samples)
		return ;
	if (timing->count < timing->capacity)
	{
		timing->samples[timing->count] = time_ns;
		timing->count++;
	}
	if (time_ns < timing->min_time)
		timing->min_time = time_ns;
	if (time_ns > timing->max_time)
		timing->max_time = time_ns;
	timing->total_time += time_ns;
}

static int	compare_long(const void *a, const void *b)
{
	long	la;
	long	lb;

	la = *(const long *)a;
	lb = *(const long *)b;
	if (la < lb)
		return (-1);
	if (la > lb)
		return (1);
	return (0);
}

void	pixel_timing_calculate_stats(t_pixel_timing *timing)
{
	if (timing->count > 0 && timing->samples)
		qsort(timing->samples, timing->count, sizeof(long), compare_long);
}
