/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_debounce.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 16:21:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 16:21:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_debounce.h"
#include "window.h"

/*
** debounce_init - Initialize debounce state with default configuration
*/
void	debounce_init(t_debounce_state *state)
{
	state->state = DEBOUNCE_IDLE;
	state->timer.is_active = 0;
	state->timer.delay_ms = DEBOUNCE_DEFAULT_DELAY_MS;
	state->preview_enabled = DEBOUNCE_DEFAULT_PREVIEW;
	state->auto_upgrade = DEBOUNCE_DEFAULT_AUTO_UPGRADE;
	state->cancel_requested = 0;
}

/*
** debounce_on_input - Handle keyboard input event
** Transitions state machine based on current state and starts/resets timer
** Only cancels render if actually in progress (render->dirty == 1)
*/
void	debounce_on_input(t_debounce_state *state, t_render *render)
{
	if (state->state == DEBOUNCE_IDLE)
	{
		state->state = DEBOUNCE_ACTIVE;
		debounce_timer_start(&state->timer);
	}
	else if (state->state == DEBOUNCE_ACTIVE)
	{
		debounce_timer_reset(&state->timer);
	}
	else if (state->state == DEBOUNCE_PREVIEW
		|| state->state == DEBOUNCE_FINAL)
	{
		if (render->dirty)
			state->cancel_requested = 1;
		state->state = DEBOUNCE_ACTIVE;
		debounce_timer_start(&state->timer);
	}
}

/*
** debounce_update - Update state machine on each frame
** Checks timer expiration and transitions between states
*/
void	debounce_update(t_debounce_state *state, t_render *render)
{
	if (state->state == DEBOUNCE_ACTIVE
		&& debounce_timer_expired(&state->timer))
	{
		if (state->preview_enabled)
			state->state = DEBOUNCE_PREVIEW;
		else
			state->state = DEBOUNCE_FINAL;
		render->low_quality = state->preview_enabled;
		render->dirty = 1;
		debounce_timer_stop(&state->timer);
	}
	else if (state->state == DEBOUNCE_PREVIEW && !render->dirty)
	{
		if (state->auto_upgrade)
		{
			state->state = DEBOUNCE_FINAL;
			render->low_quality = 0;
			render->dirty = 1;
		}
		else
			state->state = DEBOUNCE_IDLE;
	}
	else if (state->state == DEBOUNCE_FINAL && !render->dirty)
		state->state = DEBOUNCE_IDLE;
}

/*
** debounce_cancel - Cancel in-progress render
** Clears the cancel_requested flag after processing
*/
void	debounce_cancel(t_debounce_state *state)
{
	state->cancel_requested = 0;
}
