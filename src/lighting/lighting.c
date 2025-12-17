/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lighting.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/17 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "vec3.h"
#include "ray.h"
#include "shadow.h"
#include <math.h>

static void	clamp_color(t_color *result)
{
	if (result->r > 255)
		result->r = 255;
	if (result->g > 255)
		result->g = 255;
	if (result->b > 255)
		result->b = 255;
}

t_color	apply_lighting(t_scene *scene, t_hit *hit)
{
	t_vec3	light_dir;
	double	diffuse;
	t_color	result;
	double	ambient;
	double	shadow_factor;

	ambient = scene->ambient.ratio;
	light_dir = vec3_normalize(vec3_subtract(scene->light.position, \
		hit->point));
	diffuse = vec3_dot(hit->normal, light_dir);
	if (diffuse < 0)
		diffuse = 0;
	shadow_factor = calculate_shadow_factor(scene, hit->point, \
		scene->light.position, &scene->shadow_config);
	diffuse = diffuse * (1.0 - shadow_factor);
	result.r = hit->color.r * (ambient + diffuse * scene->light.brightness);
	result.g = hit->color.g * (ambient + diffuse * scene->light.brightness);
	result.b = hit->color.b * (ambient + diffuse * scene->light.brightness);
	clamp_color(&result);
	return (result);
}
