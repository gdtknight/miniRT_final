/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:18:33 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:18:34 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

# include "vec3.h"
# include "objects.h"
# include <stdbool.h>

/* Ray with origin point and normalized direction vector */
typedef struct s_ray
{
	t_vec3	origin;
	t_vec3	direction;
}	t_ray;

/* Ray-object intersection result with hit point and surface info */
typedef struct s_hit
{
	bool	hit;
	double	distance;
	t_vec3	point;
	t_vec3	normal;
	t_color	color;
}	t_hit;

#endif
