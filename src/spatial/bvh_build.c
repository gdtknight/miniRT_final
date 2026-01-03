/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_build.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/19 21:30:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spatial.h"
#include "minirt.h"
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
	return (bvh);
}

static void	bvh_node_destroy(t_bvh_node *node)
{
	if (!node)
		return ;
	bvh_node_destroy(node->left);
	bvh_node_destroy(node->right);
	if (node->objects)
		free(node->objects);
	free(node);
}

void	bvh_destroy(t_bvh *bvh)
{
	if (!bvh)
		return ;
	bvh_node_destroy(bvh->root);
	free(bvh);
}

static t_aabb	compute_bounds(t_object_ref *objects, int count, void *scene)
{
	t_aabb	bounds;
	t_aabb	obj_bounds;
	t_vec3	zero;
	int		i;

	zero.x = 0;
	zero.y = 0;
	zero.z = 0;
	if (count == 0)
		return (aabb_create(zero, zero));
	bounds = get_object_bounds(objects[0], scene);
	i = 1;
	while (i < count)
	{
		obj_bounds = get_object_bounds(objects[i], scene);
		bounds = aabb_merge(bounds, obj_bounds);
		i++;
	}
	return (bounds);
}

static int	partition_objects(t_partition_params *params)
{
	int				i;
	int				left_count;
	t_object_ref	temp;
	t_vec3			center;

	left_count = 0;
	i = 0;
	while (i < params->count)
	{
		center = get_object_center(params->objects[i], params->scene);
		if ((params->axis == 0 && center.x < params->split)
			|| (params->axis == 1 && center.y < params->split)
			|| (params->axis == 2 && center.z < params->split))
		{
			temp = params->objects[left_count];
			params->objects[left_count] = params->objects[i];
			params->objects[i] = temp;
			left_count++;
		}
		i++;
	}
	if (left_count == 0 || left_count == params->count)
		return (params->count / 2);
	return (left_count);
}

static t_bvh_node	*create_leaf_node(t_object_ref *objects, int count,
		void *scene)
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

static int	choose_split_axis(t_aabb bounds)
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

static double	calculate_split_position(t_aabb bounds, int axis)
{
	if (axis == 0)
		return ((bounds.min.x + bounds.max.x) / 2.0);
	else if (axis == 1)
		return ((bounds.min.y + bounds.max.y) / 2.0);
	else
		return ((bounds.min.z + bounds.max.z) / 2.0);
}

static t_bvh_node	*create_split_node(t_split_params *sp)
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

void	bvh_build(t_bvh *bvh, t_object_ref *objects, int count, void *scene)
{
	if (!bvh || count == 0)
		return ;
	if (bvh->root)
		bvh_node_destroy(bvh->root);
	bvh->root = bvh_build_recursive(objects, count, scene, 0);
	bvh->total_nodes = 1;
}
