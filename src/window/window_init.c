/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:31:05 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "hud.h"
#include "keyguide.h"
#include "pixel_timing.h"
#include <stdlib.h>

/*
** Initialize render structure fields with default values.
*/
/**
 * @brief init render state 함수 - 초기화 수행
 *
 * @param render 파라미터
 * @param scene 파라미터
 */
static void	init_render_state(t_render *render, t_scene *scene)
{
	render->scene = scene;
	render->selection.type = OBJ_NONE;
	render->selection.index = 0;
	render->dirty = 1;
	render->low_quality = 0;
	render->shift_pressed = 0;
	pixel_timing_init(&render->pixel_timing);
}

/*
** Initialize HUD and keyguide components.
** Returns -1 on failure.
*/
/**
 * @brief init ui components 함수 - 초기화 수행
 *
 * @param render 파라미터
 * @param scene 파라미터
 *
 * @return int 반환값
 */
static int	init_ui_components(t_render *render, t_scene *scene)
{
	if (hud_init(&render->hud, render->mlx, render->win) == -1)
	{
		free(render);
		return (-1);
	}
	if (keyguide_init(&render->keyguide, render->mlx, render->win) == -1)
	{
		hud_cleanup(&render->hud, render->mlx);
		free(render);
		return (-1);
	}
	render->hud.total_pages = hud_calculate_total_pages(scene);
	return (0);
}

/*
** Register MLX event hooks for window events.
*/
/**
 * @brief register hooks 함수
 *
 * @param render 파라미터
 */
static void	register_hooks(t_render *render)
{
	mlx_hook(render->win, 17, 0, close_window, render);
	mlx_hook(render->win, 2, 1L << 0, handle_key, render);
	mlx_hook(render->win, 3, 1L << 1, handle_key_release, render);
	mlx_loop_hook(render->mlx, render_loop, render);
}

/*
** Initialize MLX connection and image buffer.
** Returns 0 on success, -1 on failure.
*/
/**
 * @brief init mlx 함수 - 초기화 수행
 *
 * @param render 파라미터
 *
 * @return int 반환값
 */
static int	init_mlx(t_render *render)
{
	render->mlx = mlx_init();
	if (!render->mlx)
		return (-1);
	render->win = mlx_new_window(render->mlx, WINDOW_WIDTH,
			WINDOW_HEIGHT, "miniRT");
	render->img = mlx_new_image(render->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (!render->win || !render->img)
		return (-1);
	render->img_data = mlx_get_data_addr(render->img, &render->bpp,
			&render->size_line, &render->endian);
	return (0);
}

/*
** Initialize MLX window and set up event handlers.
** Creates 800x600 window with image buffer for fast rendering.
** Registers close/keyboard handlers and rendering loop.
** Returns NULL on initialization failure.
*/
t_render	*init_window(t_scene *scene)
{
	t_render	*render;

	render = malloc(sizeof(t_render));
	if (!render)
		return (NULL);
	if (init_mlx(render) == -1)
	{
		free(render);
		return (NULL);
	}
	init_render_state(render, scene);
	if (init_ui_components(render, scene) == -1)
		return (NULL);
	register_hooks(render);
	return (render);
}
