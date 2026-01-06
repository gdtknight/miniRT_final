/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_quality.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:15:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:15:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDER_QUALITY_H
# define RENDER_QUALITY_H

# include "render_state.h"

/* Quality management functions */
void	quality_set_mode(t_render_state *state, t_quality_mode mode);
int		quality_should_upgrade(t_render_state *state);
void	quality_handle_interaction(t_render_state *state);

#endif
