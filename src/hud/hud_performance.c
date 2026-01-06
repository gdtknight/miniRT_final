/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_performance.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/31 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"
#include "metrics.h"

/**
 * @brief render perf header 함수 - 렌더링 수행
 *
 * @param render 파라미터
 * @param y 파라미터
 */
static void	render_perf_header(t_render *render, int *y)
{
	mlx_string_put(render->mlx, render->win,
		HUD_MARGIN_X + 10, *y, HUD_COLOR_HIGHLIGHT, "=== Performance ===");
	*y += HUD_LINE_HEIGHT;
}

/**
 * @brief copy str 함수
 *
 * @param dst 파라미터
 * @param src 파라미터
 * @param max_len 파라미터
 *
 * @return int 반환값
 */
static int	copy_str(char *dst, char *src, int max_len)
{
	int	i;

	i = 0;
	while (src[i] && i < max_len - 1)
	{
		dst[i] = src[i];
		i++;
	}
	return (i);
}

/**
 * @brief concat and print 함수
 *
 * @param render 파라미터
 * @param params 파라미터
 */
static void	concat_and_print(t_render *render, t_perf_text *params)
{
	char	line[128];
	int		len;

	len = copy_str(line, params->prefix, 128);
	len += copy_str(line + len, params->value, 128 - len);
	if (params->suffix)
		len += copy_str(line + len, params->suffix, 128 - len);
	line[len] = '\0';
	mlx_string_put(render->mlx, render->win,
		HUD_MARGIN_X + 10, *params->y, HUD_COLOR_TEXT, line);
	*params->y += HUD_LINE_HEIGHT;
}

/**
 * @brief render perf basic 함수 - 렌더링 수행
 *
 * @param render 파라미터
 * @param y 파라미터
 */
static void	render_perf_basic(t_render *render, int *y)
{
	t_metrics	*m;
	char		buf[64];
	t_perf_text	params;

	m = &render->scene->render_state.metrics;
	params.y = y;
	hud_format_fps(buf, m->fps);
	params.prefix = "FPS: ";
	params.value = buf;
	params.suffix = NULL;
	concat_and_print(render, &params);
	hud_format_time_ms(buf, m->render_time_us);
	params.prefix = "Frame: ";
	params.value = buf;
	params.suffix = "ms";
	concat_and_print(render, &params);
	hud_format_bvh_status(buf, render->scene->render_state.bvh_enabled);
	params.prefix = "BVH: ";
	params.value = buf;
	params.suffix = NULL;
	concat_and_print(render, &params);
}

/**
 * @brief hud render performance 함수 - 렌더링 수행
 *
 * @param render 파라미터
 * @param y 파라미터
 */
void	hud_render_performance(t_render *render, int *y)
{
	render_perf_header(render, y);
	render_perf_basic(render, y);
}
