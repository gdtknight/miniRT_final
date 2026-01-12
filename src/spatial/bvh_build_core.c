/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_build_core.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 18:45:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:32:14 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "spatial/bvh_internal.h"
#include <stdlib.h>

t_bvh_node	*create_leaf_node(t_object_ref *objects, int count, void *scene)
{
	t_bvh_node	*node;
	int			i;

	node = malloc(sizeof(t_bvh_node));
	if (!node)
		return (NULL);
	node->bounds = compute_bounds(objects, count, scene);
	node->left = NULL;
	node->right = NULL;
	node->object_count = count;
	node->depth = 0;
	node->objects = malloc(sizeof(t_object_ref) * count);
	if (!node->objects)
	{
		free(node);
		return (NULL);
	}
	i = 0;
	while (i < count)
	{
		node->objects[i] = objects[i];
		i++;
	}
	return (node);
}

t_bvh_node	*bvh_build_recursive(t_object_ref *objects, int count,
		void *scene, int depth)
{
	t_aabb				bounds;
	int					axis;
	t_partition_params	params;
	t_split_params		sp;

	if (count <= 2 || depth > 20)
		return (create_leaf_node(objects, count, scene));
	bounds = compute_bounds(objects, count, scene);
	axis = choose_split_axis(bounds);
	params.objects = objects;
	params.count = count;
	params.axis = axis;
	params.split = calculate_split_position(bounds, axis);
	params.scene = scene;
	sp.mid = partition_objects(&params);
	sp.bounds = bounds;
	sp.objects = objects;
	sp.count = count;
	sp.scene = scene;
	sp.depth = depth;
	return (create_split_node(&sp));
}

/**
 * @brief bvh build 함수 - 빌드 수행
 *
 * @param bvh 파라미터
 * @param objects 파라미터
 * @param count 파라미터
 * @param scene 파라미터
 */
void	bvh_build(t_bvh *bvh, t_object_ref *objects, int count, void *scene)
{
	if (!bvh || count == 0)
		return ;
	if (bvh->root)
		bvh_node_destroy(bvh->root);
	bvh->root = bvh_build_recursive(objects, count, scene, 0);
	if (bvh->root)
		bvh->root->depth = 0;
	bvh->total_nodes = 1;
}
