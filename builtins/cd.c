/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 12:01:27 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:51:30 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static int	cd_error(char **argv);
static int	cd_management(int argc, char **argv, t_env **env);
static int	getcwd_fail(char *name, char *arg);
char		*dir_get(char *dir, t_env *env);

int	ft_cd(int argc, char **argv, t_env **env)
{
	char	*oldpwd;
	char	*newpwd;
	int		ret;

	if (argc > 2)
		return (built_error(argv[0], 0, "too many arguments"), 1);
	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
		return (getcwd_fail(argv[0], argv[1]));
	ret = cd_management(argc, argv, env);
	if (ret != 0)
		return (free(oldpwd), ret);
	env_update(env, ft_strdup("OLDPWD"), oldpwd);
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		return (getcwd_fail(argv[0], argv[1]));
	env_update(env, ft_strdup("PWD"), newpwd);
	return (0);
}

static int	getcwd_fail(char *name, char *arg)
{
	return (built_error(name, arg, strerror(errno)), 1);
}

static int	cd_management(int argc, char **argv, t_env **env)
{
	char	*path;
	int		is_oldpwd;

	is_oldpwd = 0;
	if (argc == 1)
	{
		path = ft_getenv(*env, "HOME");
		if (!path)
			return (built_error(argv[0], 0, "HOME not set"), 1);
	}
	else
	{
		is_oldpwd = !ft_strcmp(argv[1], "-");
		path = dir_get(argv[1], *env);
	}
	if (!path)
		return (cd_error(argv));
	if (chdir(path) != 0)
		return (free(path),
			built_error(argv[0], argv[1], strerror(errno)), 1);
	if (is_oldpwd)
		ft_putendl_fd(path, 1);
	free(path);
	return (0);
}

char	*dir_get(char *dir, t_env *env)
{
	if (!dir || !env)
		return (NULL);
	if (!ft_strcmp(dir, "-"))
		return (ft_getenv(env, "OLDPWD"));
	return (ft_strdup(dir));
}

static int	cd_error(char **argv)
{
	if (argv[1] && !ft_strcmp(argv[1], "-"))
		return (built_error(argv[0], argv[1], "OLDPWD not set"), 1);
	return (built_error(argv[0], argv[1], strerror(errno)), 1);
}
