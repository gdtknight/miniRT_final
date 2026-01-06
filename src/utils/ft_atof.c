/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:20 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:20:21 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Check if character is whitespace (space, tab, newline, etc).
*/
/**
 * @brief ft isspace 함수
 *
 * @param c 파라미터
 *
 * @return int 반환값
 */
static int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\r' || c == '\v' || c == '\f');
}

/*
** Check if character is a digit (0-9).
*/
/**
 * @brief ft isdigit 함수
 *
 * @param c 파라미터
 *
 * @return int 반환값
 */
static int	ft_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

/*
** Parse integer part of floating point number.
** Updates string pointer and returns integer value.
*/
/**
 * @brief parse integer part 함수 - 파싱 수행
 *
 * @param str 파라미터
 *
 * @return double 반환값
 */
static double	parse_integer_part(const char **str)
{
	double	result;

	result = 0.0;
	while (ft_isdigit(**str))
	{
		result = result * 10.0 + (**str - '0');
		(*str)++;
	}
	return (result);
}

/*
** Parse fractional part of floating point number.
** Handles digits after decimal point.
*/
/**
 * @brief parse fractional part 함수 - 파싱 수행
 *
 * @param str 파라미터
 *
 * @return double 반환값
 */
static double	parse_fractional_part(const char **str)
{
	double	result;
	double	divisor;

	result = 0.0;
	divisor = 10.0;
	if (**str == '.')
	{
		(*str)++;
		while (ft_isdigit(**str))
		{
			result += (**str - '0') / divisor;
			divisor *= 10.0;
			(*str)++;
		}
	}
	return (result);
}

/*
** Convert string to double precision floating point number.
** Format: [whitespace][+/-][digits][.digits]
** Stops at first invalid character.
*/
/**
 * @brief ft atof 함수
 *
 * @param str 파라미터
 *
 * @return double 반환값
 */
double	ft_atof(const char *str)
{
	double	result;
	double	sign;

	sign = 1.0;
	while (ft_isspace(*str))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1.0;
		str++;
	}
	result = parse_integer_part(&str);
	result += parse_fractional_part(&str);
	return (result * sign);
}
