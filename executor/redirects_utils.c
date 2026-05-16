/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirects_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 08:59:31 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/06 16:51:05 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*read_fd(int fd);
static void	heredoc_message(t_shell *shell, char *delimiter);

void	set_signal(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

int	heredoc_fork(char *delimiter, int *pipefd, t_shell *shell)
{
	pid_t	pid;

	if (pipe(pipefd) == -1)
		return (-1);
	if (pipe(shell->pipe_lineno) == -1)
		return ((close(pipefd[0]), close(pipefd[1])), -1);
	set_signal();
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		close(shell->pipe_lineno[0]);
		close(shell->pipe_lineno[1]);
		signal(SIGINT, handler);
		return (-1);
	}
	if (pid == 0)
	{
		close(pipefd[0]);
		close(shell->pipe_lineno[0]);
		heredoc_child(delimiter, pipefd[1], shell);
	}
	return ((close(pipefd[1]), close(shell->pipe_lineno[1])), pid);
}

int	heredoc_wait(pid_t pid, int *pipefd, char *delimiter, t_shell *shell)
{
	int		status;

	waitpid(pid, &status, 0);
	signal(SIGINT, handler);
	signal(SIGQUIT, SIG_IGN);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		close(pipefd[0]);
		close(shell->pipe_lineno[0]);
		shell->exit_status = 130;
		g_signal = SIGINT;
		write(1, "\n", 1);
		return (-1);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		return ((heredoc_message(shell, delimiter)), pipefd[0]);
	close(shell->pipe_lineno[0]);
	return (pipefd[0]);
}

static void	heredoc_message(t_shell *shell, char *delimiter)
{
	char	*line;

	line = ft_itoa(shell->line_count);
	ft_putstr_fd("minishell: warning: here-document at line ", 1);
	ft_putstr_fd(line, 1);
	ft_putstr_fd(" delimited by end-of-file (wanted `", 1);
	ft_putstr_fd(delimiter, 1);
	ft_putstr_fd("`)\n", 1);
	ft_free((void *)&line, 0);
	line = read_fd(shell->pipe_lineno[0]);
	shell->line_count += ft_atoi(line);
	ft_free((void *)&line, 0);
}

static char	*read_fd(int fd)
{
	char		*buffer;
	ssize_t		n;

	buffer = malloc(42);
	if (!buffer)
		return (NULL);
	n = read(fd, buffer, 41);
	if (n < 0)
		return (ft_free((void *)&buffer, 0), NULL);
	buffer[n] = 0;
	close(fd);
	return (buffer);
}
