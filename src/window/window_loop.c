/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_loop.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:40:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window/window_internal.h"
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

	render = (t_render *)param;
	if (render->dirty)
	{
		metrics_start_frame(&render->scene->render_state.metrics);
		render_scene_to_buffer(render->scene, render);
		metrics_end_frame(&render->scene->render_state.metrics);
		mlx_put_image_to_window(render->mlx, render->win, render->img, 0, 0);
		render->dirty = 0;
	}
	if (render->hud.visible && render->hud.dirty)
	{
		hud_render(render);
		keyguide_render(render);
	}
	return (0);
}
