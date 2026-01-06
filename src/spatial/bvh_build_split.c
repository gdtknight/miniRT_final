/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_build_split.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:45:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/01/04 18:50:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "spatial/bvh_internal.h"
#include <stdlib.h>

/**
 * @brief choose split axis 함수
 *
 * @param bounds 파라미터
 *
 * @return int 반환값
 */
int	choose_split_axis(t_aabb bounds)
{
	int		axis;
	double	max_extent;

	axis = 0;
	if (bounds.max.y - bounds.min.y > bounds.max.x - bounds.min.x)
		axis = 1;
	max_extent = bounds.max.x - bounds.min.x;
	if (axis == 1)
		max_extent = bounds.max.y - bounds.min.y;
	if (bounds.max.z - bounds.min.z > max_extent)
		axis = 2;
	return (axis);
}

/**
 * @brief calculate split position 함수 - 계산 수행
 *
 * @param bounds 파라미터
 * @param axis 파라미터
 *
 * @return double 반환값
 */
double	calculate_split_position(t_aabb bounds, int axis)
{
	if (axis == 0)
		return ((bounds.min.x + bounds.max.x) / 2.0);
	else if (axis == 1)
		return ((bounds.min.y + bounds.max.y) / 2.0);
	else
		return ((bounds.min.z + bounds.max.z) / 2.0);
}

t_bvh_node	*create_split_node(t_split_params *sp)
{
	t_bvh_node	*node;

	node = malloc(sizeof(t_bvh_node));
	if (!node)
		return (NULL);
	node->bounds = sp->bounds;
	node->objects = NULL;
	node->object_count = 0;
	node->left = bvh_build_recursive(sp->objects, sp->mid,
			sp->scene, sp->depth + 1);
	node->right = bvh_build_recursive(sp->objects + sp->mid,
			sp->count - sp->mid, sp->scene, sp->depth + 1);
	return (node);
}
