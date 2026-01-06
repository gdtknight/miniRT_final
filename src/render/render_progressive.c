/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_progressive.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:15:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:15:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render/render_progressive.h"
#include "window.h"

void	progressive_init(t_progressive_state *prog, int width, int height,
		int tile_size)
{
	int	tiles_x;
	int	tiles_y;

	prog->tile_size = tile_size;
	tiles_x = (width + tile_size - 1) / tile_size;
	tiles_y = (height + tile_size - 1) / tile_size;
	prog->total_tiles = tiles_x * tiles_y;
	prog->current_tile = 0;
	prog->tiles_completed = 0;
	prog->enabled = 1;
}

/**
 * @brief progressive next tile 함수
 *
 * @param prog 파라미터
 * @param rect 파라미터
 *
 * @return int 반환값
 */
int	progressive_next_tile(t_progressive_state *prog, t_tile_rect *rect)
{
	int	tiles_x;
	int	tile_x;
	int	tile_y;

	if (!prog->enabled || prog->current_tile >= prog->total_tiles)
		return (0);
	tiles_x = (800 + prog->tile_size - 1) / prog->tile_size;
	tile_x = prog->current_tile % tiles_x;
	tile_y = prog->current_tile / tiles_x;
	rect->x = tile_x * prog->tile_size;
	rect->y = tile_y * prog->tile_size;
	rect->w = prog->tile_size;
	rect->h = prog->tile_size;
	if (rect->x + rect->w > WINDOW_WIDTH)
		rect->w = WINDOW_WIDTH - rect->x;
	if (rect->y + rect->h > WINDOW_HEIGHT)
		rect->h = WINDOW_HEIGHT - rect->y;
	prog->current_tile++;
	return (1);
}

/**
 * @brief progressive reset 함수 - 설정 수행
 *
 * @param prog 파라미터
 */
void	progressive_reset(t_progressive_state *prog)
{
	prog->current_tile = 0;
	prog->tiles_completed = 0;
}
