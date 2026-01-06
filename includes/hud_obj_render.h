/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_obj_render.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUD_OBJ_RENDER_H
# define HUD_OBJ_RENDER_H

# include "minirt.h"

void	render_sphere_obj(t_render *render, int idx, int *y, int color);
void	render_plane_obj(t_render *render, int idx, int *y, int color);
void	render_cylinder_obj(t_render *render, int idx, int *y, int color);

#endif
