/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_shadow_calc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/17 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include "vec3.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

void	test_shadow_bias_perpendicular(void)
{
	t_vec3	normal;
	t_vec3	light_dir;
	double	bias;

	printf("Testing shadow bias - perpendicular (90°)...\n");
	normal.x = 0.0;
	normal.y = 1.0;
	normal.z = 0.0;
	light_dir.x = 0.0;
	light_dir.y = 1.0;
	light_dir.z = 0.0;
	bias = calculate_shadow_bias(normal, light_dir, 0.001);
	assert(bias >= 0.001 && bias <= 0.0015);
	printf("✓ Perpendicular angle test passed (bias: %f)\n", bias);
}

void	test_shadow_bias_parallel(void)
{
	t_vec3	normal;
	t_vec3	light_dir;
	double	bias;

	printf("Testing shadow bias - parallel (0°)...\n");
	normal.x = 0.0;
	normal.y = 1.0;
	normal.z = 0.0;
	light_dir.x = 1.0;
	light_dir.y = 0.0;
	light_dir.z = 0.0;
	bias = calculate_shadow_bias(normal, light_dir, 0.001);
	assert(bias >= 0.002);
	printf("✓ Parallel angle test passed (bias: %f)\n", bias);
}

void	test_shadow_bias_45_degree(void)
{
	t_vec3	normal;
	t_vec3	light_dir;
	double	bias;
	double	sqrt2;

	printf("Testing shadow bias - 45° angle...\n");
	sqrt2 = sqrt(2.0);
	normal.x = 0.0;
	normal.y = 1.0;
	normal.z = 0.0;
	light_dir.x = sqrt2 / 2.0;
	light_dir.y = sqrt2 / 2.0;
	light_dir.z = 0.0;
	bias = calculate_shadow_bias(normal, light_dir, 0.001);
	assert(bias > 0.001 && bias < 0.003);
	printf("✓ 45° angle test passed (bias: %f)\n", bias);
}

int	main(void)
{
	printf("\n=== Shadow Calculation Unit Tests ===\n\n");
	test_shadow_bias_perpendicular();
	test_shadow_bias_parallel();
	test_shadow_bias_45_degree();
	printf("\n=== Shadow bias tests passed! ===\n\n");
	return (0);
}
