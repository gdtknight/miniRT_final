/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersections.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:19:51 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:19:52 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "ray.h"
#include "vec3.h"
#include <math.h>

/*
** Calculate ray-sphere intersection distance using quadratic formula.
** Solves: ||ray.origin + t*ray.direction - sphere.center||² = radius²
** Returns closest positive t value, or -1 if no intersection.
*/
static double	calculate_sphere_t(t_vec3 oc, t_ray *ray, double radius)
{
	double	a;
	double	b;
	double	c;
	double	discriminant;
	double	t;

	a = vec3_dot(ray->direction, ray->direction);
	b = 2.0 * vec3_dot(oc, ray->direction);
	c = vec3_dot(oc, oc) - radius * radius;
	discriminant = b * b - 4 * a * c;
	if (discriminant < 0)
		return (-1);
	t = (-b - sqrt(discriminant)) / (2.0 * a);
	if (t < 0.001)
		t = (-b + sqrt(discriminant)) / (2.0 * a);
	return (t);
}

/*
** Test ray-sphere intersection and update hit info if closer.
** Uses quadratic equation to find intersection points.
** Returns 1 if hit and closer than current hit->distance, 0 otherwise.
*/
int	intersect_sphere(t_ray *ray, t_sphere *sphere, t_hit *hit)
{
	t_vec3	oc;
	double	t;
	double	radius;

	radius = sphere->diameter / 2.0;
	oc = vec3_subtract(ray->origin, sphere->center);
	t = calculate_sphere_t(oc, ray, radius);
	if (t < 0.001 || t > hit->distance)
		return (0);
	hit->distance = t;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	hit->normal = vec3_normalize(vec3_subtract(hit->point, sphere->center));
	hit->color = sphere->color;
	return (1);
}

/*
** Test ray-plane intersection using plane equation.
** Plane equation: dot(point - plane.point, plane.normal) = 0
** Returns 1 if hit and closer than current hit->distance, 0 otherwise.
*/
int	intersect_plane(t_ray *ray, t_plane *plane, t_hit *hit)
{
	double	denom;
	double	t;
	t_vec3	p0l0;

	denom = vec3_dot(plane->normal, ray->direction);
	if (fabs(denom) < 0.0001)
		return (0);
	p0l0 = vec3_subtract(plane->point, ray->origin);
	t = vec3_dot(p0l0, plane->normal) / denom;
	if (t < 0.001 || t > hit->distance)
		return (0);
	hit->distance = t;
	hit->point = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	hit->normal = plane->normal;
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1);
	hit->color = plane->color;
	return (1);
}

/* Helper structure for cylinder intersection calculations */
typedef struct s_cyl_calc
{
	double	a;
	double	b;
	double	c;
	double	discriminant;
	double	t;
	double	m;
}	t_cyl_calc;

/*
** Calculate ray-cylinder infinite surface intersection.
** Uses quadratic equation excluding cylinder caps.
** Returns 1 if intersection found within cylinder height, 0 otherwise.
*/
static int	calculate_cylinder_intersection(t_ray *ray, t_cylinder *cyl, \
		t_cyl_calc *calc)
{
	t_vec3	oc;
	double	radius;

	radius = cyl->diameter / 2.0;
	oc = vec3_subtract(ray->origin, cyl->center);
	calc->a = vec3_dot(ray->direction, ray->direction) - \
		vec3_dot(ray->direction, cyl->axis) * \
		vec3_dot(ray->direction, cyl->axis);
	calc->b = 2.0 * (vec3_dot(ray->direction, oc) - \
		vec3_dot(ray->direction, cyl->axis) * vec3_dot(oc, cyl->axis));
	calc->c = vec3_dot(oc, oc) - vec3_dot(oc, cyl->axis) * \
		vec3_dot(oc, cyl->axis) - radius * radius;
	calc->discriminant = calc->b * calc->b - 4 * calc->a * calc->c;
	if (calc->discriminant < 0 || calc->a < EPSILON)
		return (0);
	calc->t = (-calc->b - sqrt(calc->discriminant)) / (2.0 * calc->a);
	if (calc->t < 0.001)
		calc->t = (-calc->b + sqrt(calc->discriminant)) / (2.0 * calc->a);
	calc->m = vec3_dot(ray->direction, cyl->axis) * calc->t + \
		vec3_dot(oc, cyl->axis);
	return (1);
}

/*
** Check ray intersection with cylinder cap (top or bottom disc).
** Returns 1 if hit and updates hit info, 0 otherwise.
*/
static int	intersect_cylinder_cap(t_ray *ray, t_cylinder *cyl, t_hit *hit, \
		double cap_m)
{
	t_vec3	cap_center;
	double	denom;
	double	t;
	t_vec3	p;
	double	dist_sq;

	denom = vec3_dot(ray->direction, cyl->axis);
	if (fabs(denom) < EPSILON)
		return (0);
	cap_center = vec3_add(cyl->center, vec3_multiply(cyl->axis, cap_m));
	t = vec3_dot(vec3_subtract(cap_center, ray->origin), cyl->axis) / denom;
	if (t < 0.001 || t > hit->distance)
		return (0);
	p = vec3_add(ray->origin, vec3_multiply(ray->direction, t));
	dist_sq = vec3_dot(vec3_subtract(p, cap_center), \
		vec3_subtract(p, cap_center));
	if (dist_sq > (cyl->diameter / 2.0) * (cyl->diameter / 2.0))
		return (0);
	hit->distance = t;
	hit->point = p;
	hit->normal = cyl->axis;
	if (vec3_dot(ray->direction, hit->normal) > 0)
		hit->normal = vec3_multiply(hit->normal, -1.0);
	hit->color = cyl->color;
	return (1);
}

/*
** Test ray-cylinder body intersection (side surface).
** Returns 1 if hit and closer than current hit->distance, 0 otherwise.
*/
static int	intersect_cylinder_body(t_ray *ray, t_cylinder *cyl, t_hit *hit)
{
	t_cyl_calc	calc;
	t_vec3		hit_point;
	t_vec3		axis_point;

	if (!calculate_cylinder_intersection(ray, cyl, &calc))
		return (0);
	if (calc.t < 0.001 || calc.t > hit->distance)
		return (0);
	hit_point = vec3_add(ray->origin, vec3_multiply(ray->direction, calc.t));
	if (calc.m < -cyl->height / 2.0 || calc.m > cyl->height / 2.0)
		return (0);
	hit->distance = calc.t;
	hit->point = hit_point;
	axis_point = vec3_add(cyl->center, vec3_multiply(cyl->axis, calc.m));
	hit->normal = vec3_normalize(vec3_subtract(hit->point, axis_point));
	hit->color = cyl->color;
	return (1);
}

/*
** Test ray-cylinder intersection including body and caps.
** Checks body, top cap, and bottom cap, returns closest hit.
** Returns 1 if hit and closer than current hit->distance, 0 otherwise.
*/
int	intersect_cylinder(t_ray *ray, t_cylinder *cylinder, t_hit *hit)
{
	int		hit_found;
	t_hit	temp_hit;

	hit_found = 0;
	temp_hit.distance = hit->distance;
	if (intersect_cylinder_body(ray, cylinder, &temp_hit))
	{
		*hit = temp_hit;
		hit_found = 1;
	}
	temp_hit.distance = hit->distance;
	if (intersect_cylinder_cap(ray, cylinder, &temp_hit, \
		cylinder->height / 2.0))
	{
		*hit = temp_hit;
		hit_found = 1;
	}
	temp_hit.distance = hit->distance;
	if (intersect_cylinder_cap(ray, cylinder, &temp_hit, \
		-cylinder->height / 2.0))
	{
		*hit = temp_hit;
		hit_found = 1;
	}
	return (hit_found);
}
