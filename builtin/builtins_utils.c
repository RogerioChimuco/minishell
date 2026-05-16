/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 22:23:54 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/08 09:08:26 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	invoke_builtin(t_command *cmd, t_shell *shell)
{
	int	argc;

	argc = ft_matrislen(cmd->args);
	if (!ft_strcmp(cmd->args[0], "export"))
		return (ft_export(&shell->vars, cmd->args));
	if (!ft_strcmp(cmd->args[0], "cd"))
		return (ft_cd(argc, cmd->args, &shell->vars));
	else if (!ft_strcmp(cmd->args[0], "pwd"))
		return (ft_pwd(argc, cmd->args));
	else if (!ft_strcmp(cmd->args[0], "echo"))
		return (ft_echo(argc, cmd->args));
	else if (!ft_strcmp(cmd->args[0], "env"))
		return (ft_env(argc, cmd->args, shell->vars));
	else if (!ft_strcmp(cmd->args[0], "exit"))
		return (ft_exit(argc, cmd->args, shell));
	else if (!ft_strcmp(cmd->args[0], "unset"))
		return (ft_unset(&shell->vars, cmd->args));
	return (1);
}

int	is_builtin(char const *value)
{
	if (!value)
		return (0);
	if (!ft_strcmp(value, "cd"))
		return (1);
	if (!ft_strcmp(value, "echo"))
		return (1);
	if (!ft_strcmp(value, "env"))
		return (1);
	if (!ft_strcmp(value, "exit"))
		return (1);
	if (!ft_strcmp(value, "pwd"))
		return (1);
	if (!ft_strcmp(value, "unset"))
		return (1);
	if (!ft_strcmp(value, "export"))
		return (1);
	return (0);
}
