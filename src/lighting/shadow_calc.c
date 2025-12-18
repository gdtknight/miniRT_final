/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_calc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:04 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:05 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include "minirt.h"
#include "vec3.h"
#include <math.h>

/*
** Calculate adaptive shadow bias based on surface angle.
** Increases bias for grazing angles to prevent shadow acne.
** Returns adjusted bias value scaled by angle factor.
*/
double	calculate_shadow_bias(t_vec3 normal, t_vec3 light_dir, \
		double base_bias)
{
	double	dot_nl;
	double	angle_factor;
	double	bias;

	dot_nl = vec3_dot(normal, light_dir);
	if (dot_nl < 0.0)
		dot_nl = -dot_nl;
	angle_factor = 1.0 - dot_nl;
	bias = base_bias * (1.0 + angle_factor * 2.0);
	return (bias);
}

/*
** Generate offset vector for soft shadow sampling.
** Uses stratified sampling in circular pattern around light.
** Returns zero vector if only one sample requested.
*/
t_vec3	generate_shadow_sample_offset(double radius, int sample_index, \
		int total_samples)
{
	t_vec3	offset;
	double	angle;
	double	r;
	int		grid_size;

	if (total_samples <= 1)
		return ((t_vec3){0.0, 0.0, 0.0});
	grid_size = (int)sqrt((double)total_samples);
	if (grid_size < 1)
		grid_size = 1;
	angle = 2.0 * 3.14159265358979323846 * (sample_index % grid_size) \
		/ (double)grid_size;
	r = radius * (sample_index / (double)grid_size + 0.5) / (double)grid_size;
	offset.x = r * cos(angle);
	offset.y = r * sin(angle);
	offset.z = 0.0;
	return (offset);
}

/*
** Calculate shadow factor using multiple shadow rays.
** Casts multiple rays to determine partial occlusion.
** Returns 0.0 (fully lit) to 1.0 (fully shadowed).
*/
double	calculate_shadow_factor(t_scene *scene, t_vec3 point, \
		t_vec3 light_pos, t_shadow_config *config)
{
	double	shadow_count;
	double	bias;
	t_vec3	normal;
	t_vec3	light_dir;
	int		i;
	t_vec3	offset;
	t_vec3	sample_light_pos;

	shadow_count = 0.0;
	light_dir = vec3_normalize(vec3_subtract(light_pos, point));
	normal = (t_vec3){0.0, 1.0, 0.0};
	bias = calculate_shadow_bias(normal, light_dir, 0.001);
	i = 0;
	while (i < config->samples)
	{
		offset = generate_shadow_sample_offset(config->softness * 2.0, i, \
			config->samples);
		sample_light_pos = vec3_add(light_pos, offset);
		if (is_in_shadow(scene, point, sample_light_pos, bias))
			shadow_count += 1.0;
		i++;
	}
	return (shadow_count / (double)config->samples);
}

/*
** Calculate distance-based shadow attenuation.
** Softens shadows based on distance from light source.
** Uses inverse square falloff with configurable softness.
*/
double	calculate_shadow_attenuation(double distance, double max_distance, \
		double softness)
{
	double	normalized_dist;
	double	attenuation;

	if (max_distance <= 0.0)
		return (1.0);
	normalized_dist = distance / max_distance;
	attenuation = 1.0 / (1.0 + normalized_dist * normalized_dist * softness);
	if (attenuation < 0.0)
		attenuation = 0.0;
	if (attenuation > 1.0)
		attenuation = 1.0;
	return (attenuation);
}
