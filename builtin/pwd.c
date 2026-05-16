/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:41:39 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/07 18:02:57 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

int	ft_pwd(int argc, char **av)
{
	char	*path;

	if (!av || !*av)
		return (1);
	if (argc > 1)
	{
		if (av[1][0] == '-')
		{
			if (ft_strcmp(av[1], "-L") && ft_strcmp(av[1], "-P")
				&& ft_strcmp(av[1], "--"))
				return (built_error(av[0], av[1], "invalid option"));
		}
	}
	errno = 0;
	path = getcwd(NULL, 0);
	if (!path)
		return ((built_error(av[0], NULL,
					"error retrieving current directory")), 1);
	ft_putendl_fd(path, 1);
	return ((free(path)), 0);
}
