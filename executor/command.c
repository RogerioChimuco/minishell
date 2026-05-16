/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:45:30 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/09 12:51:53 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	**get_paths(t_env *env)
{
	char	**paths;

	while (env && ft_strncmp("PATH", env->key, 4))
		env = env->next;
	if (env == NULL)
		return (NULL);
	paths = ft_split(env->value, ':');
	if (!paths)
		return (NULL);
	return (paths);
}

static void	clean_paths(char **paths)
{
	int	i;

	if (!paths)
		return ;
	i = -1;
	while (paths[++i])
		free(paths[i]);
	free(paths);
	return ;
}

static void	clean(char **paths, char *path, char *slash)
{
	if (paths)
		clean_paths(paths);
	if (path)
		free(path);
	if (slash)
		free(slash);
	return ;
}

static int	find_command(t_command *cmd, t_env *envp)
{
	int		i;
	char	*path;
	char	*slash;
	char	**paths;

	paths = get_paths(envp);
	if (!paths)
		return (-1);
	slash = ft_strjoin("/", cmd->args[0]);
	if (!slash)
		return (clean_paths(paths), -1);
	i = -1;
	while (paths[++i])
	{
		path = ft_strjoin(paths[i], slash);
		if (!path)
			return (clean(paths, NULL, slash), -1);
		if (access(path, F_OK) == 0)
		{
			cmd->path = path;
			return (clean(paths, NULL, slash), 0);
		}
		free(path);
	}
	return (clean(paths, NULL, slash), -1);
}

void	resolve_command(t_command *cmd, t_env *envp)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (is_builtin(cmd->args[0]))
	{
		cmd->is_builtin = 1;
		return ;
	}
	if (ft_strchr(cmd->args[0], '/'))
		cmd->path = cmd->args[0];
	else
	{
		if (find_command(cmd, envp) == -1)
			cmd->path = cmd->args[0];
	}
	return ;
}
