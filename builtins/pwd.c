/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 12:02:03 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:52:08 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static int	is_valid_option(const char *arg);

int	ft_pwd(int argc, char **argv)
{
	char	*path;

	if (!argv || !*argv)
		return (1);
	if (argc > 1 && argv[1][0] == '-' && !is_valid_option(argv[1]))
		return (built_error(argv[0], argv[1], "invalid option"), 1);
	errno = 0;
	path = getcwd(NULL, 0);
	if (!path)
		return (built_error(argv[0], NULL,
				"error retrieving current directory"), 1);
	ft_putendl_fd(path, 1);
	return (free(path), 0);
}

static int	is_valid_option(const char *arg)
{
	static const char	*valid[] = {"-L", "-P", "--", NULL};
	int					idx;

	idx = -1;
	while (valid[++idx])
	{
		if (!ft_strcmp(arg, valid[idx]))
			return (1);
	}
	return (0);
}
