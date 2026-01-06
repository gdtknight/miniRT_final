/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_lifecycle.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:40:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:40:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "window.h"
#include "window/window_internal.h"
#include "hud.h"
#include "keyguide.h"
#include <stdlib.h>

/*
** Handle window close event (X button clicked).
** Cleans up resources and exits program.
*/
int	close_window(void *param)
{
	t_render	*render;

	render = (t_render *)param;
	keyguide_cleanup(&render->keyguide, render->mlx);
	hud_cleanup(&render->hud, render->mlx);
	cleanup_all(render->scene, render);
	exit(0);
	return (0);
}
