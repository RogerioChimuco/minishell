/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirects_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 13:00:31 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 13:00:35 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	set_heredoc_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

static void	close_fd_pair(int *fds)
{
	close(fds[0]);
	close(fds[1]);
}

int	heredoc_fork(char *delimiter, int *pipefd, t_shell *shell)
{
	pid_t	pid;

	if (pipe(pipefd) == -1)
		return (-1);
	if (pipe(shell->pipe_lineno) == -1)
		return (close_fd_pair(pipefd), -1);
	set_heredoc_signals();
	pid = fork();
	if (pid == -1)
	{
		close_fd_pair(pipefd);
		close_fd_pair(shell->pipe_lineno);
		signal(SIGINT, handler);
		return (-1);
	}
	if (pid == 0)
	{
		close(pipefd[0]);
		close(shell->pipe_lineno[0]);
		run_heredoc_child(delimiter, pipefd[1], shell);
	}
	close(pipefd[1]);
	close(shell->pipe_lineno[1]);
	return (pid);
}

static int	handle_heredoc_sigint(int *pipefd, t_shell *shell)
{
	close(pipefd[0]);
	close(shell->pipe_lineno[0]);
	shell->exit_status = 130;
	g_signal = SIGINT;
	write(1, "\n", 1);
	return (-1);
}

int	heredoc_wait(pid_t pid, int *pipefd, char *delimiter, t_shell *shell)
{
	int		status;

	waitpid(pid, &status, 0);
	signal(SIGINT, handler);
	signal(SIGQUIT, SIG_IGN);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		return (handle_heredoc_sigint(pipefd, shell));
	if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		return (print_heredoc_warning(shell, delimiter), pipefd[0]);
	close(shell->pipe_lineno[0]);
	return (pipefd[0]);
}
