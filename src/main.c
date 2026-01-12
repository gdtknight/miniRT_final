/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:38 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:21:10 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parser.h"
#include "window.h"
#include "spatial.h"
#include "bvh_vis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** Initialize scene structure with default values.
** Allocates memory and sets all counters to zero.
** Returns NULL on allocation failure.
*/
static t_scene	*init_scene(void)
{
	t_scene	*scene;

	scene = malloc(sizeof(t_scene));
	if (!scene)
		return (NULL);
	memset(scene, 0, sizeof(t_scene));
	scene->sphere_count = 0;
	scene->plane_count = 0;
	scene->cylinder_count = 0;
	scene->has_ambient = 0;
	scene->has_camera = 0;
	scene->has_light = 0;
	scene->shadow_config = init_shadow_config();
	render_state_init(&scene->render_state);
	scene->render_state.bvh_enabled = 1;
	return (scene);
}

static int	parse_args(int argc, char **argv, char **filename, int *bvh_vis)
{
	int	i;

	*filename = NULL;
	*bvh_vis = 0;
	i = 1;
	while (i < argc)
	{
		if (argv[i][0] == '-' && argv[i][1] == '-')
		{
			if (strcmp(argv[i], "--bvh-vis") == 0)
				*bvh_vis = 1;
			else
			{
				printf("Unknown option: %s\n", argv[i]);
				return (0);
			}
		}
		else if (*filename == NULL)
			*filename = argv[i];
		else
		{
			printf("Multiple scene files provided\n");
			return (0);
		}
		i++;
	}
	return (*filename != NULL);
}

static int	initialize_scene_and_render(char *filename, t_scene **scene,
		t_render **render, int bvh_vis)
{
	*scene = init_scene();
	if (!*scene)
		return (print_error("Failed to initialize scene"));
	if (!parse_scene(filename, *scene))
	{
		cleanup_scene(*scene);
		return (1);
	}
	scene_build_bvh(*scene);
	if (bvh_vis && (*scene)->render_state.bvh)
	{
		(*scene)->render_state.bvh->visualize = 1;
		bvh_visualize((*scene)->render_state.bvh, NULL, *scene);
	}
	*render = init_window(*scene);
	if (!*render)
	{
		cleanup_scene(*scene);
		return (print_error("Failed to initialize window"));
	}
	return (0);
}

/*
** Main program entry point.
** Validates arguments, initializes scene, parses input file,
** creates window, and starts rendering loop.
*/
/**
 * @brief miniRT 프로그램 메인 함수
 *
 * @param argc 명령행 인자 개수
 * @param argv 명령행 인자 배열
 *
 * @return int 프로그램 종료 코드 (0: 성공, 1: 실패)
 */
int	main(int argc, char **argv)
{
	t_scene		*scene;
	t_render	*render;
	char		*filename;
	int			bvh_vis;

	if (argc < 2)
	{
		printf("Usage: %s <scene_file.rt> [--bvh-vis]\n", argv[0]);
		return (1);
	}
	if (!parse_args(argc, argv, &filename, &bvh_vis))
	{
		printf("Usage: %s <scene_file.rt> [--bvh-vis]\n", argv[0]);
		return (1);
	}
	if (initialize_scene_and_render(filename, &scene, &render, bvh_vis) != 0)
		return (1);
	mlx_loop(render->mlx);
	return (0);
}
