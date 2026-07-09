/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 12:21:19 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:51:51 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static int	env_error(char **argv);

int	ft_env(int argc, char **argv, t_env *env)
{
	if (!env)
		return (1);
	if (argc > 1)
		return (env_error(argv));
	env_print(env, 0, -1);
	return (0);
}

static int	env_error(char **argv)
{
	ft_putstr_fd(argv[0], 2);
	ft_putstr_fd(": ‘", 2);
	ft_putstr_fd(argv[1], 2);
	ft_putendl_fd("’: No such file or directory", 2);
	return (127);
}
