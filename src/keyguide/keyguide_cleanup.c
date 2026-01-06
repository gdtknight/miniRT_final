/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyguide_cleanup.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/30 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "keyguide.h"
#include "window.h"

/**
 * @brief keyguide cleanup 함수 - 정리 수행
 *
 * @param keyguide 파라미터
 * @param mlx 파라미터
 */
void	keyguide_cleanup(t_keyguide_state *keyguide, void *mlx)
{
	if (keyguide->bg_img)
	{
		mlx_destroy_image(mlx, keyguide->bg_img);
		keyguide->bg_img = NULL;
		keyguide->bg_data = NULL;
	}
}
