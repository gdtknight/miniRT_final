/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_color.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/03 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

int	get_red(int color)
{
	return ((color >> 16) & 0xFF);
}

int	get_green(int color)
{
	return ((color >> 8) & 0xFF);
}

int	get_blue(int color)
{
	return (color & 0xFF);
}

int	make_color(int r, int g, int b)
{
	return ((r << 16) | (g << 8) | b);
}

int	blend_colors(int bg_color, int fg_color, double alpha)
{
	int	bg_r;
	int	bg_g;
	int	bg_b;
	int	fg_r;
	int	fg_g;
	int	fg_b;

	bg_r = get_red(bg_color);
	bg_g = get_green(bg_color);
	bg_b = get_blue(bg_color);
	fg_r = get_red(fg_color);
	fg_g = get_green(fg_color);
	fg_b = get_blue(fg_color);
	bg_r = (int)(bg_r * (1.0 - alpha) + fg_r * alpha);
	bg_g = (int)(bg_g * (1.0 - alpha) + fg_g * alpha);
	bg_b = (int)(bg_b * (1.0 - alpha) + fg_b * alpha);
	return (make_color(bg_r, bg_g, bg_b));
}
