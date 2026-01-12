/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_objects.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:34 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:34 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "vec3.h"
#include <stdio.h>

/*
** Helper function to skip current token and advance to next.
** Skips non-space characters, then skips spaces.
*/
static char	*skip_to_next_token(char *token)
{
	while (*token && *token != ' ')
		token++;
	while (*token == ' ')
		token++;
	return (token);
}

/*
** Parse sphere object from scene file.
** Format: sp <x,y,z> <diameter> <R,G,B>
** Validates diameter is positive.
*/
/**
 * @brief parse sphere 함수 - 파싱 수행
 *
 * @param line 파라미터
 * @param scene 파라미터
 *
 * @return int 반환값
 */
int	parse_sphere(char *line, t_scene *scene)
{
	char		*token;
	t_sphere	*sphere;

	if (scene->sphere_count >= 100)
		return (print_error("Too many spheres"));
	sphere = &scene->spheres[scene->sphere_count];
	token = line + 3;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &sphere->center))
		return (print_error("Invalid sphere center"));
	token = skip_to_next_token(token);
	sphere->diameter = ft_atof(token);
	if (sphere->diameter <= 0)
		return (print_error("Sphere diameter must be positive"));
	sphere->radius = sphere->diameter / 2.0;
	sphere->radius_squared = sphere->radius * sphere->radius;
	snprintf(sphere->id, 8, "sp-%d", scene->sphere_count + 1);
	token = skip_to_next_token(token);
	if (!parse_color(token, &sphere->color))
		return (0);
	scene->sphere_count++;
	return (1);
}

/*
** Parse plane object from scene file.
** Format: pl <x,y,z> <nx,ny,nz> <R,G,B>
** Normalizes the normal vector.
*/
/**
 * @brief parse plane 함수 - 파싱 수행
 *
 * @param line 파라미터
 * @param scene 파라미터
 *
 * @return int 반환값
 */
int	parse_plane(char *line, t_scene *scene)
{
	char		*token;
	t_plane		*plane;

	if (scene->plane_count >= 100)
		return (print_error("Too many planes"));
	plane = &scene->planes[scene->plane_count];
	token = line + 3;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &plane->point))
		return (print_error("Invalid plane point"));
	token = skip_to_next_token(token);
	if (!parse_vector(token, &plane->normal))
		return (print_error("Invalid plane normal"));
	plane->normal = vec3_normalize(plane->normal);
	snprintf(plane->id, 8, "pl-%d", scene->plane_count + 1);
	token = skip_to_next_token(token);
	if (!parse_color(token, &plane->color))
		return (0);
	scene->plane_count++;
	return (1);
}

/*
** Parse cylinder diameter and height parameters.
** Validates both dimensions are positive.
** Computes precomputed cached values for optimization.
*/
/**
 * @brief parse cylinder params 함수 - 파싱 수행
 *
 * @param token 파라미터
 * @param cylinder 파라미터
 *
 * @return int 반환값
 */
static int	parse_cylinder_params(char *token, t_cylinder *cylinder)
{
	token = skip_to_next_token(token);
	cylinder->diameter = ft_atof(token);
	token = skip_to_next_token(token);
	cylinder->height = ft_atof(token);
	if (cylinder->diameter <= 0 || cylinder->height <= 0)
		return (print_error("Cylinder dimensions must be positive"));
	cylinder->radius = cylinder->diameter / 2.0;
	cylinder->radius_squared = cylinder->radius * cylinder->radius;
	cylinder->half_height = cylinder->height / 2.0;
	return (1);
}

/*
** Parse cylinder object from scene file.
** Format: cy <x,y,z> <nx,ny,nz> <diameter> <height> <R,G,B>
** Normalizes the axis vector.
*/
/**
 * @brief parse cylinder 함수 - 파싱 수행
 *
 * @param line 파라미터
 * @param scene 파라미터
 *
 * @return int 반환값
 */
int	parse_cylinder(char *line, t_scene *scene)
{
	char		*token;
	t_cylinder	*cylinder;

	if (scene->cylinder_count >= 100)
		return (print_error("Too many cylinders"));
	cylinder = &scene->cylinders[scene->cylinder_count];
	token = line + 3;
	while (*token == ' ')
		token++;
	if (!parse_vector(token, &cylinder->center))
		return (print_error("Invalid cylinder center"));
	token = skip_to_next_token(token);
	if (!parse_vector(token, &cylinder->axis))
		return (print_error("Invalid cylinder axis"));
	cylinder->axis = vec3_normalize(cylinder->axis);
	snprintf(cylinder->id, 8, "cy-%d", scene->cylinder_count + 1);
	if (!parse_cylinder_params(token, cylinder))
		return (0);
	token = skip_to_next_token(skip_to_next_token(token));
	if (!parse_color(token, &cylinder->color))
		return (0);
	scene->cylinder_count++;
	return (1);
}
