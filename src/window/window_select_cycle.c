/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_select_cycle.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:40:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"

/*
** Find next object type after sphere.
*/
int	next_type_from_sphere(t_scene *scene)
{
	if (scene->plane_count > 0)
		return (OBJ_PLANE);
	else if (scene->cylinder_count > 0)
		return (OBJ_CYLINDER);
	else
		return (OBJ_SPHERE);
}

/*
** Find next object type after plane.
*/
int	next_type_from_plane(t_scene *scene)
{
	if (scene->cylinder_count > 0)
		return (OBJ_CYLINDER);
	else if (scene->sphere_count > 0)
		return (OBJ_SPHERE);
	else
		return (OBJ_PLANE);
}

/*
** Find next object type after cylinder.
*/
int	next_type_from_cylinder(t_scene *scene)
{
	if (scene->sphere_count > 0)
		return (OBJ_SPHERE);
	else if (scene->plane_count > 0)
		return (OBJ_PLANE);
	else
		return (OBJ_CYLINDER);
}
