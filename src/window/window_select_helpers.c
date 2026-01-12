/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_select_helpers.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:44:57 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window/window_internal.h"

/*
** Cycle to next object type when index wraps around.
*/
/**
 * @brief cycle type forward 함수
 *
 * @param render 파라미터
 */
void	cycle_type_forward(t_render *render)
{
	render->selection.index = 0;
	if (render->selection.type == OBJ_SPHERE)
		render->selection.type = next_type_from_sphere(render->scene);
	else if (render->selection.type == OBJ_PLANE)
		render->selection.type = next_type_from_plane(render->scene);
	else
		render->selection.type = next_type_from_cylinder(render->scene);
}

/*
** Set index to last element of current type.
*/
/**
 * @brief set last index 함수 - 설정 수행
 *
 * @param render 파라미터
 */
void	set_last_index(t_render *render)
{
	if (render->selection.type == OBJ_CYLINDER)
		render->selection.index = render->scene->cylinder_count - 1;
	else if (render->selection.type == OBJ_PLANE)
		render->selection.index = render->scene->plane_count - 1;
	else
		render->selection.index = render->scene->sphere_count - 1;
}

/*
** Cycle backward from sphere type.
*/
/**
 * @brief cycle backward sphere 함수
 *
 * @param render 파라미터
 */
void	cycle_backward_sphere(t_render *render)
{
	if (render->scene->cylinder_count > 0)
		render->selection.type = OBJ_CYLINDER;
	else if (render->scene->plane_count > 0)
		render->selection.type = OBJ_PLANE;
	else
		render->selection.type = OBJ_SPHERE;
	set_last_index(render);
}

/*
** Cycle backward from plane type.
*/
/**
 * @brief cycle backward plane 함수
 *
 * @param render 파라미터
 */
void	cycle_backward_plane(t_render *render)
{
	if (render->scene->sphere_count > 0)
		render->selection.type = OBJ_SPHERE;
	else if (render->scene->cylinder_count > 0)
		render->selection.type = OBJ_CYLINDER;
	else
		render->selection.type = OBJ_PLANE;
	if (render->selection.type == OBJ_SPHERE)
		render->selection.index = render->scene->sphere_count - 1;
	else
		render->selection.index = render->scene->plane_count - 1;
}

/*
** Continue backward cycling for cylinder type.
*/
/**
 * @brief cycle backward cylinder 함수
 *
 * @param render 파라미터
 */
void	cycle_backward_cylinder(t_render *render)
{
	if (render->scene->plane_count > 0)
		render->selection.type = OBJ_PLANE;
	else if (render->scene->sphere_count > 0)
		render->selection.type = OBJ_SPHERE;
	else
		render->selection.type = OBJ_CYLINDER;
	if (render->selection.type == OBJ_PLANE)
		render->selection.index = render->scene->plane_count - 1;
	else
		render->selection.index = render->scene->cylinder_count - 1;
}
