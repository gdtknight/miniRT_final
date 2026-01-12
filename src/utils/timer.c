/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 21:30:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/12 20:31:46 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "metrics.h"

/**
 * @brief timer start 함수
 *
 * @param tv 파라미터
 */
void	timer_start(struct timeval *tv)
{
	gettimeofday(tv, 0);
}

/**
 * @brief timer elapsed us 함수
 *
 * @param start 파라미터
 *
 * @return long 반환값
 */
long	timer_elapsed_us(struct timeval *start)
{
	struct timeval	now;
	long			elapsed;

	gettimeofday(&now, 0);
	elapsed = (now.tv_sec - start->tv_sec) * 1000000L;
	elapsed += (now.tv_usec - start->tv_usec);
	return (elapsed);
}

/**
 * @brief timer elapsed ms 함수
 *
 * @param start 파라미터
 *
 * @return double 반환값
 */
double	timer_elapsed_ms(struct timeval *start)
{
	return (timer_elapsed_us(start) / 1000.0);
}
