/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_lifecycle.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:45:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/15 14:15:06 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "bvh_internal.h"
#include <stdlib.h>

t_bvh	*bvh_create(void)
{
	t_bvh	*bvh;

	bvh = malloc(sizeof(t_bvh));
	if (!bvh)
		return (NULL);
	bvh->root = NULL;
	bvh->enabled = 1;
	bvh->total_nodes = 0;
	bvh->max_depth = 0;
	bvh->visualize = 0;
	return (bvh);
}

/**
 * @brief bvh node destroy 함수 - 제거 및 메모리 해제 수행
 *
 * @param node 파라미터
 */
void	bvh_node_destroy(t_bvh_node *node)
{
	if (!node)
		return ;
	bvh_node_destroy(node->left);
	bvh_node_destroy(node->right);
	if (node->objects)
		free(node->objects);
	free(node);
}

/**
 * @brief bvh destroy 함수 - 제거 및 메모리 해제 수행
 *
 * @param bvh 파라미터
 */
void	bvh_destroy(t_bvh *bvh)
{
	if (!bvh)
		return ;
	bvh_node_destroy(bvh->root);
	free(bvh);
}
