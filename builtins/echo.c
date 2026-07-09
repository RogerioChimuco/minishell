/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 12:01:28 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:51:41 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static int	is_n_flag(const char *arg);
static void	echo_print_from(char **argv, int start_idx);

int	ft_echo(int argc, char **argv)
{
	int	idx;
	int	newline;

	newline = 1;
	idx = 1;
	if (argc != 1)
	{
		idx = 0;
		while (argv[++idx] && (ft_strlen(argv[idx]) != 0)
			&& is_n_flag(argv[idx]))
			newline = 0;
	}
	echo_print_from(argv, idx);
	if (newline)
		ft_putstr_fd("\n", 1);
	return (0);
}

static void	echo_print_from(char **argv, int start_idx)
{
	while (argv[start_idx])
	{
		ft_putstr_fd(argv[start_idx++], 1);
		if (argv[start_idx])
			ft_putchar_fd(' ', 1);
	}
}

static int	is_n_flag(const char *arg)
{
	int	idx;

	if (arg[0] != '-')
		return (0);
	idx = 0;
	while (arg[++idx])
	{
		if (arg[idx] != 'n')
			return (0);
	}
	if (idx == 1)
		return (0);
	return (1);
}
