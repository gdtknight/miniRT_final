/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_navigation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"

int	hud_get_global_index(t_selection sel, t_scene *scene)
{
	if (sel.type == OBJ_SPHERE)
		return (sel.index);
	else if (sel.type == OBJ_PLANE)
		return (scene->sphere_count + sel.index);
	else if (sel.type == OBJ_CYLINDER)
		return (scene->sphere_count + scene->plane_count + sel.index);
	return (0);
}

void	hud_get_selection_from_global(t_selection *sel, int idx,
	t_scene *scene)
{
	if (idx < scene->sphere_count)
	{
		sel->type = OBJ_SPHERE;
		sel->index = idx;
	}
	else if (idx < scene->sphere_count + scene->plane_count)
	{
		sel->type = OBJ_PLANE;
		sel->index = idx - scene->sphere_count;
	}
	else
	{
		sel->type = OBJ_CYLINDER;
		sel->index = idx - scene->sphere_count - scene->plane_count;
	}
}

void	hud_select_next(t_render *render)
{
	int	total_objects;
	int	idx;

	total_objects = render->scene->sphere_count
		+ render->scene->plane_count + render->scene->cylinder_count;
	if (total_objects == 0)
		return ;
	idx = hud_get_global_index(render->selection, render->scene);
	idx = (idx + 1) % total_objects;
	hud_get_selection_from_global(&render->selection, idx,
		render->scene);
	render->hud.dirty = 1;
}

void	hud_select_prev(t_render *render)
{
	int	total_objects;
	int	idx;

	total_objects = render->scene->sphere_count
		+ render->scene->plane_count + render->scene->cylinder_count;
	if (total_objects == 0)
		return ;
	idx = hud_get_global_index(render->selection, render->scene);
	idx = (idx - 1 + total_objects) % total_objects;
	hud_get_selection_from_global(&render->selection, idx,
		render->scene);
	render->hud.dirty = 1;
}

void	hud_change_page(t_render *render, int direction)
{
	if (render->hud.total_pages <= 1)
		return ;
	if (direction > 0)
		render->hud.current_page = (render->hud.current_page + 1)
			% render->hud.total_pages;
	else
		render->hud.current_page = (render->hud.current_page - 1
				+ render->hud.total_pages) % render->hud.total_pages;
	render->hud.dirty = 1;
	render->dirty = 1;
}
