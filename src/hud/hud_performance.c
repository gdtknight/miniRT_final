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

int	mlx_string_put(void *mlx_ptr, void *win_ptr, int x, int y,
		int color, char *string);

static void	render_perf_header(t_render *render, int *y)
{
	mlx_string_put(render->mlx, render->win,
		HUD_MARGIN_X + 10, *y, HUD_COLOR_HIGHLIGHT, "=== Performance ===");
	*y += HUD_LINE_HEIGHT;
}

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

static void	concat_and_print(t_render *render, int *y, char *prefix,
		char *value, char *suffix)
{
	char	line[128];
	int		len;

	len = copy_str(line, prefix, 128);
	len += copy_str(line + len, value, 128 - len);
	if (suffix)
		len += copy_str(line + len, suffix, 128 - len);
	line[len] = '\0';
	mlx_string_put(render->mlx, render->win,
		HUD_MARGIN_X + 10, *y, HUD_COLOR_TEXT, line);
	*y += HUD_LINE_HEIGHT;
}

static void	render_perf_basic(t_render *render, int *y)
{
	t_metrics	*m;
	char		buf[64];

	m = &render->scene->render_state.metrics;
	hud_format_fps(buf, m->fps);
	concat_and_print(render, y, "FPS: ", buf, NULL);
	hud_format_time_ms(buf, m->render_time_us);
	concat_and_print(render, y, "Frame: ", buf, "ms");
	hud_format_bvh_status(buf, render->scene->render_state.bvh_enabled);
	concat_and_print(render, y, "BVH: ", buf, NULL);
}

void	hud_render_performance(t_render *render, int *y)
{
	render_perf_header(render, y);
	render_perf_basic(render, y);
}
