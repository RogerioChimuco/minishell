/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 08:55:48 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/08 09:31:51 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static int	env_error(char **av);

int	ft_env(int argc, char **argv, t_env *env)
{
	if (!env)
		return (1);
	if (argc > 1)
		return (env_error(argv));
	env_print(env, 0, -1);
	return (0);
}

static int	env_error(char **av)
{
	ft_putstr_fd(av[0], 2);
	ft_putstr_fd(": ‘", 2);
	ft_putstr_fd(av[1], 2);
	ft_putendl_fd("’: No such file or directory", 2);
	return (127);
}
