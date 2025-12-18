/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoshin <yoshin@student.42gyeongsan.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:20:16 by yoshin            #+#    #+#             */
/*   Updated: 2025/12/18 15:20:16 by yoshin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
** Print error message to standard error.
** Outputs "Error\n" followed by custom message.
** Returns 0 to indicate failure (convention for parse functions).
*/
int	print_error(const char *message)
{
	write(2, "Error\n", 6);
	if (message)
	{
		write(2, message, strlen(message));
		write(2, "\n", 1);
	}
	return (0);
}

/*
** Print error message and exit program with failure status.
** Used for unrecoverable errors.
*/
void	error_exit(const char *message)
{
	print_error(message);
	exit(EXIT_FAILURE);
}
