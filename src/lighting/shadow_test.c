/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow_test.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miniRT team <miniRT@42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 00:00:00 by miniRT           #+#    #+#             */
/*   Updated: 2025/12/17 00:00:00 by miniRT          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shadow.h"
#include "minirt.h"
#include "vec3.h"
#include "ray.h"

static int	check_sphere_shadow(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int	i;

	i = 0;
	while (i < scene->sphere_count)
	{
		if (intersect_sphere(ray, &scene->spheres[i], hit))
			return (1);
		i++;
	}
	return (0);
}

static int	check_plane_shadow(t_scene *scene, t_ray *ray, t_hit *hit)
{
	int	i;

	i = 0;
	while (i < scene->plane_count)
	{
		if (intersect_plane(ray, &scene->planes[i], hit))
			return (1);
		i++;
	}
	return (0);
}

/**
 * @brief Test if point is in shadow (single ray)
 * 
 * @param scene Scene data
 * @param point Point to test
 * @param light_pos Light source position
 * @param bias Shadow bias offset
 * @return 1 if in shadow, 0 if lit
 */
int	is_in_shadow(t_scene *scene, t_vec3 point, t_vec3 light_pos, double bias)
{
	t_ray	shadow_ray;
	t_hit	shadow_hit;
	t_vec3	to_light;
	t_vec3	light_dir;

	to_light = vec3_subtract(light_pos, point);
	shadow_hit.distance = vec3_magnitude(to_light);
	light_dir = vec3_normalize(to_light);
	shadow_ray.origin = vec3_add(point, vec3_multiply(light_dir, bias));
	shadow_ray.direction = light_dir;
	if (check_sphere_shadow(scene, &shadow_ray, &shadow_hit))
		return (1);
	if (check_plane_shadow(scene, &shadow_ray, &shadow_hit))
		return (1);
	return (0);
}
