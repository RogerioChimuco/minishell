/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 10:17:31 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/07 18:03:26 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static int	search_arg(const char *src);
static void	echo_arg_print(char **argv, int i);

int	ft_echo(int argc, char **argv)
{
	int	i;
	int	newline;

	newline = 1;
	i = 1;
	if (argc != 1)
	{
		i = 0;
		while (argv[++i] && (ft_strlen(argv[i]) != 0) && search_arg(argv[i]))
			newline = 0;
	}
	echo_arg_print(argv, i);
	if (newline)
		ft_putstr_fd("\n", 1);
	return (0);
}

static void	echo_arg_print(char **argv, int i)
{
	while (argv[i])
	{
		ft_putstr_fd(argv[i++], 1);
		if (argv[i])
			ft_putchar_fd(' ', 1);
	}
}

static int	search_arg(const char *src)
{
	int	i;

	i = -1;
	while (src[++i])
	{
		if ((!i && src[i] == '-') || src[i] == 'n')
			continue ;
		else
			return (0);
	}
	if (i == 1)
		return (0);
	return (1);
}
