/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:50 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 15:35:36 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_H
# define WINDOW_H

# include "minirt.h"
# include "mlx.h"
# include "pixel_timing.h"
# include "render_debounce.h"

/* Window resolution constants - MacBook M1 13-inch native resolution */
# define WINDOW_WIDTH 1440
# define WINDOW_HEIGHT 900
# define ASPECT_RATIO_NUM 1440.0
# define ASPECT_RATIO_DEN 900.0

/* Object type enumeration for selection */
typedef enum e_obj_type
{
	OBJ_NONE = 0,
	OBJ_SPHERE,
	OBJ_PLANE,
	OBJ_CYLINDER
}	t_obj_type;

/* Selected object information */
typedef struct s_selection
{
	t_obj_type	type;
	int			index;
}	t_selection;

/* HUD state structure */
typedef struct s_hud_state
{
	int		visible;
	int		current_page;
	int		objects_per_page;
	int		total_pages;
	int		dirty;
	void	*bg_img;
	char	*bg_data;
	int		bpp;
	int		size_line;
	int		endian;
}	t_hud_state;

/* Key guide state structure */
typedef struct s_keyguide_state
{
	void	*bg_img;
	char	*bg_data;
	int		bpp;
	int		size_line;
	int		endian;
	int		visible;
	int		x;
	int		y;
}	t_keyguide_state;

/* Render context containing MLX pointers and scene data */
typedef struct s_render
{
	void				*mlx;
	void				*win;
	void				*img;
	char				*img_data;
	int					bpp;
	int					size_line;
	int					endian;
	t_scene				*scene;
	t_selection			selection;
	int					dirty;
	int					low_quality;
	int					shift_pressed;
	t_hud_state			hud;
	t_keyguide_state	keyguide;
	t_pixel_timing		pixel_timing;
	t_debounce_state	debounce;
}	t_render;

/* Initialize MLX window and set up event handlers */
t_render	*init_window(t_scene *scene);
/* Close window and clean up resources */
int			close_window(void *param);
/* Handle keyboard input (ESC to exit) */
int			handle_key(int keycode, void *param);
/* Handle key release events */
int			handle_key_release(int keycode, void *param);
/* Main rendering loop */
int			render_loop(void *param);

#endif
