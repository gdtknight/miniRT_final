/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_color_extract.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief get red 함수 - 조회 수행
 *
 * @param color 파라미터
 *
 * @return int 반환값
 */
int	get_red(int color)
{
	return ((color >> 16) & 0xFF);
}

/**
 * @brief get green 함수 - 조회 수행
 *
 * @param color 파라미터
 *
 * @return int 반환값
 */
int	get_green(int color)
{
	return ((color >> 8) & 0xFF);
}

/**
 * @brief get blue 함수 - 조회 수행
 *
 * @param color 파라미터
 *
 * @return int 반환값
 */
int	get_blue(int color)
{
	return (color & 0xFF);
}
