/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_selection.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:31:27 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window/window_internal.h"
#include "hud.h"

/*
** Cycle to previous object type when index goes negative.
*/
/**
 * @brief cycle type backward 함수
 *
 * @param render 파라미터
 */
static void	cycle_type_backward(t_render *render)
{
	if (render->selection.type == OBJ_SPHERE)
		cycle_backward_sphere(render);
	else if (render->selection.type == OBJ_PLANE)
		cycle_backward_plane(render);
}

/*
** Handle selection increment (right bracket key).
*/
/**
 * @brief select next object 함수
 *
 * @param render 파라미터
 */
static void	select_next_object(t_render *render)
{
	if (render->selection.type == OBJ_NONE)
		render->selection.type = OBJ_SPHERE;
	else
	{
		render->selection.index++;
		if ((render->selection.type == OBJ_SPHERE
				&& render->selection.index >= render->scene->sphere_count)
			|| (render->selection.type == OBJ_PLANE
				&& render->selection.index >= render->scene->plane_count)
			|| (render->selection.type == OBJ_CYLINDER
				&& render->selection.index >= render->scene->cylinder_count))
			cycle_type_forward(render);
	}
}

/*
** Handle selection decrement (left bracket key).
*/
/**
 * @brief select prev object 함수
 *
 * @param render 파라미터
 */
static void	select_prev_object(t_render *render)
{
	if (render->selection.type == OBJ_NONE)
		render->selection.type = OBJ_SPHERE;
	else
	{
		render->selection.index--;
		if (render->selection.index < 0)
		{
			if (render->selection.type == OBJ_CYLINDER)
				cycle_backward_cylinder(render);
			else
				cycle_type_backward(render);
		}
	}
}

/*
** Handle object selection with bracket keys.
** Right bracket: next object, Left bracket: previous object
*/
/**
 * @brief handle object selection 함수
 *
 * @param render 파라미터
 * @param keycode 파라미터
 */
void	handle_object_selection(t_render *render, int keycode)
{
	int	total;

	total = render->scene->sphere_count + render->scene->plane_count
		+ render->scene->cylinder_count;
	if (total == 0)
		return ;
	if (keycode == KEY_BRACKET_RIGHT)
		select_next_object(render);
	else if (keycode == KEY_BRACKET_LEFT)
		select_prev_object(render);
	hud_mark_dirty(render);
}
