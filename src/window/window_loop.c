/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_loop.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 15:32:41 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window_internal.h"
#include "hud.h"
#include "keyguide.h"
#include "metrics.h"

/*
** Main rendering loop hook.
** Only re-renders when dirty flag is set.
*/
/*
** Main rendering loop hook.
** Only re-renders when dirty flag is set.
*/
/**
 * @brief render loop 함수 - 렌더링 수행
 *
 * @param param 파라미터
 *
 * @return int 반환값
 */
int	render_loop(void *param)
{
	t_render	*render;
	int			rendered;

	render = (t_render *)param;
	rendered = 0;
	debounce_update(&render->debounce, render);
	if (render->dirty)
	{
		render->is_rendering = 1;
		metrics_start_frame(&render->scene->render_state.metrics);
		render_scene_to_buffer(render->scene, render);
		render->is_rendering = 0;
		if (render->debounce.cancel_requested)
		{
			debounce_cancel(&render->debounce);
			render->dirty = 1;
		}
		else
		{
			metrics_end_frame(&render->scene->render_state.metrics);
			mlx_put_image_to_window(
				render->mlx,
				render->win,
				render->img, 0, 0);
			render->dirty = 0;
			rendered = 1;
		}
	}
	if (render->hud.visible && (render->hud.dirty || rendered))
	{
		hud_render(render);
		keyguide_render(render);
	}
	return (0);
}
