/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_obj_render.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.h"
#include "hud.h"

void	render_sphere_obj(t_render *render, int idx, int *y, int color)
{
	char	buf[128];
	int		i;

	i = 0;
	buf[i++] = 'S';
	buf[i++] = 'p';
	buf[i++] = 'h';
	buf[i++] = 'e';
	buf[i++] = 'r';
	buf[i++] = 'e';
	buf[i++] = ' ';
	buf[i++] = '#';
	buf[i++] = '0' + idx + 1;
	buf[i++] = ':';
	buf[i++] = ' ';
	buf[i++] = 'p';
	buf[i++] = 'o';
	buf[i++] = 's';
	buf[i] = '\0';
	hud_format_vec3(buf + i, render->scene->spheres[idx].center);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, color, buf);
	*y += HUD_LINE_HEIGHT;
}

void	render_plane_obj(t_render *render, int idx, int *y, int color)
{
	char	buf[128];
	int		i;

	i = 0;
	buf[i++] = 'P';
	buf[i++] = 'l';
	buf[i++] = 'a';
	buf[i++] = 'n';
	buf[i++] = 'e';
	buf[i++] = ' ';
	buf[i++] = '#';
	buf[i++] = '0' + idx + 1;
	buf[i++] = ':';
	buf[i++] = ' ';
	buf[i++] = 'p';
	buf[i++] = 'o';
	buf[i++] = 's';
	buf[i] = '\0';
	hud_format_vec3(buf + i, render->scene->planes[idx].point);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, color, buf);
	*y += HUD_LINE_HEIGHT;
}

void	render_cylinder_obj(t_render *render, int idx, int *y, int color)
{
	char	buf[128];
	int		i;

	i = 0;
	buf[i++] = 'C';
	buf[i++] = 'y';
	buf[i++] = 'l';
	buf[i++] = 'i';
	buf[i++] = 'n';
	buf[i++] = 'd';
	buf[i++] = 'e';
	buf[i++] = 'r';
	buf[i++] = ' ';
	buf[i++] = '#';
	buf[i++] = '0' + idx + 1;
	buf[i++] = ':';
	buf[i++] = ' ';
	buf[i++] = 'p';
	buf[i++] = 'o';
	buf[i++] = 's';
	buf[i] = '\0';
	hud_format_vec3(buf + i, render->scene->cylinders[idx].center);
	mlx_string_put(render->mlx, render->win, HUD_MARGIN_X + 10,
		*y, color, buf);
	*y += HUD_LINE_HEIGHT;
}
