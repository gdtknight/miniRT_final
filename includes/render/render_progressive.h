/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_progressive.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:15:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:15:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDER_PROGRESSIVE_H
# define RENDER_PROGRESSIVE_H

# include "render_state.h"

/* Progressive rendering functions */
void	progressive_init(t_progressive_state *prog, int width, int height,
			int tile_size);
int		progressive_next_tile(t_progressive_state *prog, t_tile_rect *rect);
void	progressive_reset(t_progressive_state *prog);

#endif
