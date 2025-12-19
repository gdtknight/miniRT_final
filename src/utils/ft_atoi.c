/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:26 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:20:26 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Check if character is whitespace (space, tab, newline, etc).
*/
static int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || \
			c == '\r' || c == '\v' || c == '\f');
}

/*
** Check if character is a digit (0-9).
*/
static int	ft_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

/*
** Convert string to integer.
** Skips leading whitespace, handles optional +/- sign.
** Stops at first non-digit character.
*/
int	ft_atoi(const char *str)
{
	int	result;
	int	sign;

	result = 0;
	sign = 1;
	while (ft_isspace(*str))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (ft_isdigit(*str))
	{
		result = result * 10 + (*str - '0');
		str++;
	}
	return (result * sign);
}
