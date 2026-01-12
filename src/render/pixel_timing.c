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
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>

long	get_time_ns(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000000000L + ts.tv_nsec);
}

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

void	pixel_timing_print_stats(t_pixel_timing *timing)
{
	double	avg_ns;
	double	median_ns;
	double	p95;
	double	p99;

	if (timing->count == 0)
	{
		printf("\n=== Pixel Timing Statistics ===\n");
		printf("No samples collected.\n");
		return ;
	}
	avg_ns = (double)timing->total_time / (double)timing->count;
	if (timing->count % 2 == 0)
		median_ns = (timing->samples[timing->count / 2 - 1]
				+ timing->samples[timing->count / 2]) / 2.0;
	else
		median_ns = timing->samples[timing->count / 2];
	p95 = timing->samples[(size_t)(timing->count * 0.95)];
	p99 = timing->samples[(size_t)(timing->count * 0.99)];
	printf("\n=== Pixel Timing Statistics ===\n");
	printf("Total pixels: %zu\n", timing->count);
	printf("Min time:     %.3f µs (%.6f ms)\n",
		timing->min_time / 1000.0, timing->min_time / 1000000.0);
	printf("Max time:     %.3f µs (%.6f ms)\n",
		timing->max_time / 1000.0, timing->max_time / 1000000.0);
	printf("Average:      %.3f µs (%.6f ms)\n",
		avg_ns / 1000.0, avg_ns / 1000000.0);
	printf("Median:       %.3f µs (%.6f ms)\n",
		median_ns / 1000.0, median_ns / 1000000.0);
	printf("95th %%ile:    %.3f µs (%.6f ms)\n",
		p95 / 1000.0, p95 / 1000000.0);
	printf("99th %%ile:    %.3f µs (%.6f ms)\n",
		p99 / 1000.0, p99 / 1000000.0);
	printf("================================\n\n");
}
