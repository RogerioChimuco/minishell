/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 12:11:31 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 15:03:46 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	invoke_builtin(t_command *cmd, t_shell *shell)
{
	int		argc;
	char	*name;

	name = cmd->args[0];
	argc = ft_matrislen(cmd->args);
	if (!ft_strcmp(name, "export"))
		return (ft_export(&shell->vars, cmd->args));
	if (!ft_strcmp(name, "cd"))
		return (ft_cd(argc, cmd->args, &shell->vars));
	if (!ft_strcmp(name, "pwd"))
		return (ft_pwd(argc, cmd->args));
	if (!ft_strcmp(name, "echo"))
		return (ft_echo(argc, cmd->args));
	if (!ft_strcmp(name, "env"))
		return (ft_env(argc, cmd->args, shell->vars));
	if (!ft_strcmp(name, "exit"))
		return (ft_exit(argc, cmd->args, shell));
	if (!ft_strcmp(name, "unset"))
		return (ft_unset(&shell->vars, cmd->args));
	return (1);
}

int	is_builtin(char const *value)
{
	static char const	*builtins[] = {"export", "cd", "pwd", "echo",
		"env", "exit", "unset", NULL};
	int					idx;

	if (!value)
		return (0);
	idx = -1;
	while (builtins[++idx])
	{
		if (!ft_strcmp(value, builtins[idx]))
			return (1);
	}
	return (0);
}
