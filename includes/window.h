/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:50 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:18:51 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_H
# define WINDOW_H

# include "minirt.h"

/* Render context containing MLX pointers and scene data */
typedef struct s_render
{
	void		*mlx;
	void		*win;
	t_scene		*scene;
}	t_render;

/* Initialize MLX window and set up event handlers */
t_render	*init_window(t_scene *scene);
/* Close window and clean up resources */
int			close_window(void *param);
/* Handle keyboard input (ESC to exit) */
int			handle_key(int keycode, void *param);

/* Initialize MLX connection */
void		*mlx_init(void);
/* Create new window with specified dimensions and title */
void		*mlx_new_window(void *mlx_ptr, int width, int height,
				char *title);
/* Start MLX event loop */
int			mlx_loop(void *mlx_ptr);
/* Register event handler for window events */
int			mlx_hook(void *win_ptr, int event, int mask,
				int (*f)(), void *param);
/* Register keyboard event handler */
int			mlx_key_hook(void *win_ptr, int (*f)(), void *param);

#endif
