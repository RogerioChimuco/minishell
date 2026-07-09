/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:58:25 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:58:29 by luqalmei         ###   ########.fr       */
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
}

static int	try_paths(t_command *cmd, char **paths, char *slash)
{
	int		i;
	char	*path;

	i = -1;
	while (paths[++i])
	{
		path = ft_strjoin(paths[i], slash);
		if (!path)
			return (-1);
		if (access(path, F_OK) == 0)
		{
			cmd->path = path;
			return (0);
		}
		free(path);
	}
	return (-1);
}

static int	find_command(t_command *cmd, t_env *envp)
{
	char	*slash;
	char	**paths;
	int		result;

	paths = get_paths(envp);
	if (!paths)
		return (-1);
	slash = ft_strjoin("/", cmd->args[0]);
	if (!slash)
	{
		clean_paths(paths);
		return (-1);
	}
	result = try_paths(cmd, paths, slash);
	clean_paths(paths);
	free(slash);
	return (result);
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
	else if (find_command(cmd, envp) == -1)
		cmd->path = cmd->args[0];
}
