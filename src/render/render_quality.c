/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_quality.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:15:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:32:39 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render/render_quality.h"

/**
 * @brief quality set mode 함수 - 설정 수행
 *
 * @param state 파라미터
 * @param mode 파라미터
 */
void	quality_set_mode(t_render_state *state, t_quality_mode mode)
{
	state->quality = mode;
	state->target_quality = mode;
	if (mode == QUALITY_HIGH)
		state->metrics.quality_mode = 1;
	else
		state->metrics.quality_mode = 0;
}

/**
 * @brief quality should upgrade 함수
 *
 * @param state 파라미터
 *
 * @return int 반환값
 */
int	quality_should_upgrade(t_render_state *state)
{
	long	elapsed;

	if (state->interaction.is_interacting)
		return (0);
	if (state->quality == state->target_quality)
		return (0);
	elapsed = timer_elapsed_us(&state->interaction.last_interaction);
	return (elapsed > 1000000);
}

/**
 * @brief quality handle interaction 함수
 *
 * @param state 파라미터
 */
void	quality_handle_interaction(t_render_state *state)
{
	state->interaction.is_interacting = 1;
	state->interaction.interaction_count++;
	timer_start(&state->interaction.last_interaction);
	if (state->adaptive_enabled && state->target_quality == QUALITY_HIGH)
		state->quality = QUALITY_LOW;
}
