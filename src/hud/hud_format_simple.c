/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_format_simple.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief hud format fps 함수
 *
 * @param buf 파라미터
 * @param fps 파라미터
 */
void	hud_format_fps(char *buf, double fps)
{
	hud_format_float(buf, fps);
}

/**
 * @brief hud format time ms 함수
 *
 * @param buf 파라미터
 * @param time_us 파라미터
 */
void	hud_format_time_ms(char *buf, long time_us)
{
	double	time_ms;

	time_ms = time_us / 1000.0;
	hud_format_float(buf, time_ms);
}

/**
 * @brief hud format bvh status 함수
 *
 * @param buf 파라미터
 * @param enabled 파라미터
 */
void	hud_format_bvh_status(char *buf, int enabled)
{
	if (enabled)
		ft_strcpy(buf, "ON");
	else
		ft_strcpy(buf, "OFF");
}
