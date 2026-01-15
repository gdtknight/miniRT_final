/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_state.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:15:26 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_state.h"
#include "render_quality.h"
#include <stdlib.h>

/**
 * @brief render state init 함수 - 초기화 수행
 *
 * @param state 파라미터
 */
void	render_state_init(t_render_state *state)
{
	state->quality = QUALITY_HIGH;
	state->target_quality = QUALITY_HIGH;
	state->interaction.is_interacting = 0;
	state->interaction.interaction_count = 0;
	timer_start(&state->interaction.last_interaction);
	state->progressive.enabled = 0;
	state->progressive.current_tile = 0;
	state->progressive.total_tiles = 0;
	state->progressive.tiles_completed = 0;
	state->progressive.tile_size = 32;
	metrics_init(&state->metrics);
	state->bvh = NULL;
	state->adaptive_enabled = 0;
	state->bvh_enabled = 1;
	state->show_info = 1;
}

/**
 * @brief render state cleanup 함수 - 정리 수행
 *
 * @param state 파라미터
 */
void	render_state_cleanup(t_render_state *state)
{
	if (state->bvh)
	{
		bvh_destroy(state->bvh);
		state->bvh = NULL;
	}
}

/**
 * @brief render state update 함수 - 렌더링 수행
 *
 * @param state 파라미터
 */
void	render_state_update(t_render_state *state)
{
	if (state->adaptive_enabled && quality_should_upgrade(state))
	{
		state->quality = state->target_quality;
	}
}
