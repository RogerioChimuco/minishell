/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:45:42 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/09 13:56:27 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	execute_pipeline(t_shell *shell, t_env *envp)
{
	fork_pipeline(shell, envp);
	wait_pipeline(shell);
}

static void	execute_builtin(t_command *cmd, t_shell *shell)
{
	int			std_in;
	int			std_out;

	std_in = dup(STDIN_FILENO);
	std_out = dup(STDOUT_FILENO);
	if (cmd->redir)
	{
		if (apply_redir(cmd->redir) == -1)
		{
			perror("minishell");
			return (restore_fds(std_in, std_out));
		}
	}
	shell->exit_status = invoke_builtin(cmd, shell);
	return (restore_fds(std_in, std_out));
}

int	message_error_cmd(const char *cmd, int err)
{
	ft_putstr_fd((char *)cmd, 2);
	write(2, ": ", 2);
	if (!ft_strchr(cmd, '/'))
		return (ft_putendl_fd("command not found", 2), 127);
	if (err == ENOENT)
		return (ft_putendl_fd("No such file or directory", 2), 127);
	else if (err == EACCES)
		return (ft_putendl_fd("Permission denied", 2), 126);
	else if (err == EISDIR)
		return (ft_putendl_fd("Is a directory", 2), 126);
	else if (err == ENOEXEC)
		return (ft_putendl_fd("Exec format error", 2), 126);
	else
	{
		ft_putendl_fd(strerror(err), 2);
		return (1);
	}
}

void	execute_child(t_command *cmd, int i, t_shell *shell)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	setup_pipes(i, shell);
	if (cmd->redir)
	{
		if (apply_redir(cmd->redir) == -1)
			exit(1);
	}
	if (cmd->is_builtin)
		exit(invoke_builtin(cmd, shell));
	if (!cmd->args[0])
		exit(0);
	if (cmd->args[0] == NULL)
	{
		message_error_cmd(cmd->args[0], ENOENT);
		exit(127);
	}
	is_dir_or_file(cmd->args[0]);
	execve(cmd->path, cmd->args, shell->envp);
	exit(message_error_cmd(cmd->path, errno));
}

void	execute(t_shell *shell)
{
	t_command	*cmd;

	cmd = (t_command *)shell->cmds->content;
	if (shell->length == 1 && is_builtin(cmd->args[0]))
		execute_builtin(cmd, shell);
	else
		execute_pipeline(shell, shell->vars);
	return ;
}
