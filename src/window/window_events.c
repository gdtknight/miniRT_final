/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_events.c                                    :+:      :+:    :+:   */
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

/* Forward declarations from window_key_handlers.c */
void	handle_camera_keys(t_render *render, int keycode);
void	handle_transform_keys(t_render *render, int keycode);
void	handle_hud_keys(t_render *render, int keycode);

/*
** Check if key is a movement or transformation key.
*/
/**
 * @brief is movement key 함수
 *
 * @param keycode 파라미터
 *
 * @return int 반환값
 */
static int	is_movement_key(int keycode)
{
	return (keycode == KEY_W || keycode == KEY_X || keycode == KEY_A
		|| keycode == KEY_D || keycode == KEY_Q || keycode == KEY_Z
		|| keycode == KEY_E || keycode == KEY_C
		|| keycode == KEY_R || keycode == KEY_T || keycode == KEY_F
		|| keycode == KEY_G || keycode == KEY_V || keycode == KEY_B
		|| keycode == KEY_INSERT || keycode == KEY_HOME || keycode == KEY_PGUP
		|| keycode == KEY_DELETE || keycode == KEY_END || keycode == KEY_PGDN);
}

/*
** Handle keyboard input events with interactive controls.
** Uses dirty flag and low quality rendering for smooth interaction.
*/
/**
 * @brief handle key 함수
 *
 * @param keycode 파라미터
 * @param param 파라미터
 *
 * @return int 반환값
 */
int	handle_key(int keycode, void *param)
{
	t_render	*render;

	render = (t_render *)param;
	if (keycode == KEY_ESC)
		close_window(param);
	handle_hud_keys(render, keycode);
	if (keycode == KEY_BRACKET_LEFT || keycode == KEY_BRACKET_RIGHT)
		handle_object_selection(render, keycode);
/**
 * @brief if 함수
 *
 * @param KEY_I 파라미터
 *
 * @return else 반환값
 */
	else if (keycode == KEY_I)
	{
		render->scene->render_state.show_info
			= !render->scene->render_state.show_info;
		render->dirty = 1;
	}
	else if (keycode == KEY_SHIFT_L || keycode == KEY_SHIFT_R)
		render->shift_pressed = 1;
	handle_camera_keys(render, keycode);
	handle_transform_keys(render, keycode);
	return (0);
}

/*
** Handle key release events.
** Switches to high quality rendering when interaction stops.
*/
/**
 * @brief handle key release 함수
 *
 * @param keycode 파라미터
 * @param param 파라미터
 *
 * @return int 반환값
 */
int	handle_key_release(int keycode, void *param)
{
	t_render	*render;

	render = (t_render *)param;
	if (is_movement_key(keycode))
	{
		render->low_quality = 0;
		render->dirty = 1;
	}
	else if (keycode == KEY_SHIFT_L || keycode == KEY_SHIFT_R)
		render->shift_pressed = 0;
	return (0);
}
