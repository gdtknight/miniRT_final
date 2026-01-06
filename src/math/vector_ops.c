/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector_ops.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:23 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:24 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <math.h>
#include "vec3.h"

/*
** Calculate dot product of two vectors.
** Returns scalar: a.x * b.x + a.y * b.y + a.z * b.z
** Used for projections and angle calculations.
*/
/**
 * @brief vec3 dot 함수 - 내적 수행
 *
 * @param a 파라미터
 * @param b 파라미터
 *
 * @return double 반환값
 */
double	vec3_dot(t_vec3 a, t_vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

/*
** Calculate cross product of two vectors.
** Returns vector perpendicular to both input vectors.
** Right-hand rule determines direction.
*/
/**
 * @brief vec3 cross 함수 - 외적 수행
 *
 * @param a 파라미터
 * @param b 파라미터
 *
 * @return t_vec3 반환값
 */
t_vec3	vec3_cross(t_vec3 a, t_vec3 b)
{
	t_vec3	result;

	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	return (result);
}

/*
** Calculate magnitude (length) of vector.
** Returns sqrt(x² + y² + z²)
*/
/**
 * @brief vec3 magnitude 함수
 *
 * @param v 파라미터
 *
 * @return double 반환값
 */
double	vec3_magnitude(t_vec3 v)
{
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

/*
** Normalize vector to unit length (magnitude = 1).
** Returns zero vector if input magnitude is zero.
*/
/**
 * @brief vec3 normalize 함수 - 정규화 수행
 *
 * @param v 파라미터
 *
 * @return t_vec3 반환값
 */
t_vec3	vec3_normalize(t_vec3 v)
{
	double	mag;
	t_vec3	result;

	mag = vec3_magnitude(v);
	if (mag > 0.0)
	{
		result.x = v.x / mag;
		result.y = v.y / mag;
		result.z = v.z / mag;
	}
	else
	{
		result.x = 0.0;
		result.y = 0.0;
		result.z = 0.0;
	}
	return (result);
}
