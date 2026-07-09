/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:58:44 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:58:46 by luqalmei         ###   ########.fr       */
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
	int	std_in;
	int	std_out;

	std_in = dup(STDIN_FILENO);
	std_out = dup(STDOUT_FILENO);
	if (cmd->redir && apply_redirections(cmd->redir) == -1)
	{
		perror("minishell");
		return (restore_fds(std_in, std_out));
	}
	shell->exit_status = invoke_builtin(cmd, shell);
	return (restore_fds(std_in, std_out));
}

static int	get_error_message_and_code(int err, char **msg)
{
	if (err == ENOENT)
		return (*msg = "No such file or directory", 127);
	if (err == EACCES)
		return (*msg = "Permission denied", 126);
	if (err == EISDIR)
		return (*msg = "Is a directory", 126);
	if (err == ENOEXEC)
		return (*msg = "Exec format error", 126);
	return (*msg = strerror(err), 1);
}

int	print_command_error(const char *cmd, int err)
{
	char	*msg;
	int		code;

	ft_putstr_fd((char *)cmd, 2);
	write(2, ": ", 2);
	if (!ft_strchr(cmd, '/'))
		return (ft_putendl_fd("command not found", 2), 127);
	code = get_error_message_and_code(err, &msg);
	ft_putendl_fd(msg, 2);
	return (code);
}

void	execute_child(t_command *cmd, int i, t_shell *shell)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	setup_pipes(i, shell);
	if (cmd->redir && apply_redirections(cmd->redir) == -1)
		exit(1);
	if (cmd->is_builtin)
		exit(invoke_builtin(cmd, shell));
	if (!cmd->args[0])
		exit(0);
	check_exec_permissions(cmd->args[0]);
	execve(cmd->path, cmd->args, shell->envp);
	exit(print_command_error(cmd->path, errno));
}

void	execute(t_shell *shell)
{
	t_command	*cmd;

	cmd = (t_command *)shell->cmds->content;
	if (shell->length == 1 && is_builtin(cmd->args[0]))
		execute_builtin(cmd, shell);
	else
		execute_pipeline(shell, shell->vars);
}
