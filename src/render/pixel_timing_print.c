/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pixel_timing_print.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 11:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/14 11:40:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pixel_timing.h"
#include <stdio.h>

static void	print_timing_header_and_limits(t_pixel_timing *timing);
static void	print_timing_percentiles(double avg_ns, double median_ns,
				double p95, double p99);

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
	print_timing_header_and_limits(timing);
	print_timing_percentiles(avg_ns, median_ns, p95, p99);
	printf("================================\n\n");
}

static void	print_timing_header_and_limits(t_pixel_timing *timing)
{
	printf("\n=== Pixel Timing Statistics ===\n");
	printf("Total pixels: %zu\n", timing->count);
	printf("Min time:     %.3f µs (%.6f ms)\n",
		timing->min_time / 1000.0, timing->min_time / 1000000.0);
	printf("Max time:     %.3f µs (%.6f ms)\n",
		timing->max_time / 1000.0, timing->max_time / 1000000.0);
}

static void	print_timing_percentiles(double avg_ns, double median_ns,
	double p95, double p99)
{
	printf("Average:      %.3f µs (%.6f ms)\n",
		avg_ns / 1000.0, avg_ns / 1000000.0);
	printf("Median:       %.3f µs (%.6f ms)\n",
		median_ns / 1000.0, median_ns / 1000000.0);
	printf("95th %%ile:    %.3f µs (%.6f ms)\n",
		p95 / 1000.0, p95 / 1000000.0);
	printf("99th %%ile:    %.3f µs (%.6f ms)\n",
		p99 / 1000.0, p99 / 1000000.0);
}
