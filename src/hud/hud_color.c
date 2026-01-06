/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_color.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief make color 함수
 *
 * @param r 파라미터
 * @param g 파라미터
 * @param b 파라미터
 *
 * @return int 반환값
 */
int	make_color(int r, int g, int b)
{
	return ((r << 16) | (g << 8) | b);
}

/**
 * @brief blend component 함수
 *
 * @param bg 파라미터
 * @param fg 파라미터
 * @param alpha 파라미터
 *
 * @return int 반환값
 */
static int	blend_component(int bg, int fg, double alpha)
{
	return ((int)(bg * (1.0 - alpha) + fg * alpha));
}

/**
 * @brief blend colors 함수
 *
 * @param bg_color 파라미터
 * @param fg_color 파라미터
 * @param alpha 파라미터
 *
 * @return int 반환값
 */
int	blend_colors(int bg_color, int fg_color, double alpha)
{
	int	r;
	int	g;
	int	b;

	r = blend_component(get_red(bg_color), get_red(fg_color), alpha);
	g = blend_component(get_green(bg_color), get_green(fg_color), alpha);
	b = blend_component(get_blue(bg_color), get_blue(fg_color), alpha);
	return (make_color(r, g, b));
}
