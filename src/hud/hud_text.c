/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_text.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"

/**
 * @brief hud mark dirty 함수
 *
 * @param render 파라미터
 */
void	hud_mark_dirty(t_render *render)
{
	render->hud.dirty = 1;
}

/**
 * @brief hud render background row 함수 - 렌더링 수행
 *
 * @param render 파라미터
 * @param y 파라미터
 */
void	hud_render_background_row(t_render *render, int y)
{
	int				x;
	int				scene_color;
	int				blended;
	t_pixel_params	params;

	x = HUD_MARGIN_X;
	params.img_data = render->img_data;
	params.y = y;
	params.size_line = render->size_line;
	params.bpp = render->bpp;
	while (x < HUD_MARGIN_X + HUD_WIDTH)
	{
		params.x = x;
		scene_color = get_pixel(&params);
		blended = blend_colors(scene_color, HUD_COLOR_BG, HUD_BG_ALPHA);
		set_pixel(&params, blended);
		x++;
	}
}

/**
 * @brief hud render background 함수 - 렌더링 수행
 *
 * @param render 파라미터
 */
void	hud_render_background(t_render *render)
{
	int	y;

	y = HUD_MARGIN_Y;
	while (y < HUD_MARGIN_Y + HUD_HEIGHT)
	{
		hud_render_background_row(render, y);
		y++;
	}
}

void	format_and_print_vec3(t_render *render, int *y,
		char *label, t_vec3 vec)
{
	char	buf[128];
	int		i;

	i = 0;
	while (i < 8)
		buf[i++] = ' ';
	while (*label)
		buf[i++] = *label++;
	buf[i] = '\0';
	hud_format_vec3(buf + i, vec);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, HUD_COLOR_TEXT, buf);
	*y += HUD_LINE_HEIGHT;
}

/**
 * @brief copy str to buf 함수
 *
 * @param dst 파라미터
 * @param src 파라미터
 *
 * @return int 반환값
 */
int	copy_str_to_buf(char *dst, char *src)
{
	int	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	return (i);
}
