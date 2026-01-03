/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_transparency.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/03 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

int	get_pixel(t_pixel_params *params)
{
	char	*pixel;

	pixel = params->img_data + (params->y * params->size_line
			+ params->x * (params->bpp / 8));
	return (*(int *)pixel);
}

void	set_pixel(t_pixel_params *params, int color)
{
	char	*pixel;

	pixel = params->img_data + (params->y * params->size_line
			+ params->x * (params->bpp / 8));
	*(int *)pixel = color;
}
