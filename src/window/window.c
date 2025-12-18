/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:31 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:20:31 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include <stdlib.h>

/*
** Handle window close event (X button clicked).
** Cleans up resources and exits program.
*/
int	close_window(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	cleanup_all(render->scene, render);
	exit(0);
	return (0);
}

/*
** Handle keyboard input events.
** ESC key (keycode 65307) closes the window.
*/
int	handle_key(int keycode, void *param)
{
	if (keycode == 65307)
		close_window(param);
	return (0);
}

/*
** Initialize MLX window and set up event handlers.
** Creates 800x600 window and registers close/keyboard handlers.
** Returns NULL on initialization failure.
*/
t_render	*init_window(t_scene *scene)
{
	t_render	*render;

	render = malloc(sizeof(t_render));
	if (!render)
		return (NULL);
	render->mlx = mlx_init();
	if (!render->mlx)
	{
		free(render);
		return (NULL);
	}
	render->win = mlx_new_window(render->mlx, 800, 600, "miniRT");
	if (!render->win)
	{
		free(render);
		return (NULL);
	}
	render->scene = scene;
	mlx_hook(render->win, 17, 0, close_window, render);
	mlx_key_hook(render->win, handle_key, render);
	return (render);
}
