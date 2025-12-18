/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:11 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:20:12 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include <stdlib.h>

/*
** Free memory allocated for scene structure.
** Scene contains all objects, lights, and configuration.
*/
void	cleanup_scene(t_scene *scene)
{
	if (!scene)
		return ;
	free(scene);
}

/*
** Free memory allocated for render context.
** Render context contains MLX pointers and scene reference.
*/
void	cleanup_render(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	if (!render)
		return ;
	free(render);
}

/*
** Clean up all allocated resources (scene and render context).
** Should be called before program exit.
*/
void	cleanup_all(t_scene *scene, void *param)
{
	cleanup_render(param);
	cleanup_scene(scene);
}
