/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 16:24:04 by ckulembe          #+#    #+#             */
/*   Updated: 2026/03/28 15:43:56 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	create_pipes_and_pids(t_shell *shell)
{
	if (create_pipes(shell) == -1)
	{
		perror("pipe");
		return (-1);
	}
	if (create_pids(shell) == -1)
	{
		perror("pids");
		return (-1);
	}
	return (0);
}

void	shell_clear(t_shell *shell)
{
	if (shell->input || shell->pipes)
		ft_free((void *)&shell->input, (void *)&shell->pipes);
	if (shell->tokens)
	{
		ft_lstclear(&shell->tokens, token_clear);
		shell->tokens = NULL;
	}
	if (shell->cmds)
	{
		ft_lstclear(&shell->cmds, command_clear);
		shell->cmds = NULL;
	}
}

void	fork_pipeline(t_shell *shell, t_env *envp)
{
	t_list		*cmds;
	t_command	*cmd;

	if (create_pipes_and_pids(shell) == -1)
		return ;
	cmds = shell->cmds;
	while (cmds)
	{
		cmd = (t_command *)cmds->content;
		resolve_command(cmd, envp);
		shell->pids[shell->pid_i] = fork();
		if (shell->pids[shell->pid_i] == -1)
		{
			close_pipes(shell, shell->length - 1);
			wait_pids(shell, shell->pid_i);
			return ((void)perror("fork"));
		}
		if (shell->pids[shell->pid_i] == 0)
		{
			execute_child(cmd, cmd->index, shell);
			exit(1);
		}
		shell->pid_i ++;
		cmds = cmds->next;
	}
}

void	wait_pipeline(t_shell *shell)
{
	close_pipes(shell, shell->length - 1);
	signal(SIGINT, handler_exec);
	signal(SIGQUIT, handler_exec);
	wait_pids(shell, shell->pid_i);
	if (g_signal == SIGINT)
		write (1, "\n", 1);
	signal(SIGINT, handler);
	signal(SIGQUIT, SIG_IGN);
	g_signal = 0;
	shell->pid_i = 0;
}
