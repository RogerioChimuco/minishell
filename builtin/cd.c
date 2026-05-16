/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:25:54 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/09 18:30:56 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static int	cd_error(char **av);
static int	cd_management(int ac, char **av, t_env **env);
char		*dir_get(char *dir, t_env *env);

int	ft_cd(int argc, char **argv, t_env **env)
{
	char	*oldpwd;
	char	*newpwd;
	int		out;

	if (argc > 2)
		return (built_error(argv[0], 0, "too many arguments"), 1);
	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
		return (built_error(argv[0], argv[1], strerror(errno)), 1);
	out = cd_management(argc, argv, env);
	if (out != 0)
		return (free(oldpwd), out);
	env_update(env, ft_strdup("OLDPWD"), oldpwd);
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		return (built_error(argv[0], argv[1], strerror(errno)), 1);
	env_update(env, ft_strdup("PWD"), newpwd);
	return (0);
}

static int	cd_management(int ac, char **av, t_env **env)
{
	char	*path;

	if (ac == 1)
	{
		path = ft_getenv(*env, "HOME");
		if (!path)
			return (built_error(av[0], 0, "HOME not set"), 1);
	}
	else
		path = dir_get(av[1], *env);
	if (!path)
		return (cd_error(av));
	if (chdir(path) != 0)
		return (free(path),
			built_error(av[0], av[1], strerror(errno)), 1);
	free(path);
	return (0);
}

char	*dir_get(char *dir, t_env *env)
{
	char	*path;

	if (!dir || !env)
		return (NULL);
	if (!ft_strcmp(dir, "-"))
	{
		path = ft_getenv(env, "OLDPWD");
		if (!path)
			return (NULL);
		ft_putendl_fd(path, 1);
		return (path);
	}
	return (ft_strdup(dir));
}

static int	cd_error(char **av)
{
	if (av[1] && !ft_strcmp(av[1], "-"))
		return (built_error(av[0], av[1], "OLDPWD not set"), 1);
	return (built_error(av[0], av[1], strerror(errno)), 1);
}
