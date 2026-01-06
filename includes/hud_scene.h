/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_scene.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUD_SCENE_H
# define HUD_SCENE_H

# include "minirt.h"

void	hud_render_camera(t_render *render, int *y);
void	render_camera_fov(t_render *render, int *y);
void	hud_render_ambient(t_render *render, int *y);
void	hud_render_light(t_render *render, int *y);
void	render_light_bright(t_render *render, int *y);

#endif
