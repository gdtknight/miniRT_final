/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud_format_helpers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:00:00 by yoshin            #+#    #+#             */
/*   Updated: 2026/01/04 00:00:00 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hud.h"

/**
 * @brief ft strcpy 함수
 *
 * @param dst 파라미터
 * @param src 파라미터
 */
void	ft_strcpy(char *dst, const char *src)
{
	int	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
}

/**
 * @brief ft numlen 함수
 *
 * @param n 파라미터
 *
 * @return int 반환값
 */
int	ft_numlen(int n)
{
	int	len;

	len = 0;
	if (n <= 0)
		len = 1;
	while (n)
	{
		n /= 10;
		len++;
	}
	return (len);
}

/**
 * @brief ft itoa buf 함수
 *
 * @param buf 파라미터
 * @param n 파라미터
 */
void	ft_itoa_buf(char *buf, int n)
{
	int		len;
	long	num;

	num = n;
	if (num < 0)
	{
		buf[0] = '-';
		num = -num;
	}
	len = ft_numlen(n);
	buf[len] = '\0';
	while (len > 0 && num >= 0)
	{
		buf[--len] = (num % 10) + '0';
		num /= 10;
		if (n < 0 && len == 1)
			break ;
	}
}
