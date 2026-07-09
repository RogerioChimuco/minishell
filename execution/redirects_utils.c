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

static char	*read_line_count_fd(int fd);
static void	print_heredoc_warning(t_shell *shell, char *delimiter);

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

static pid_t	cleanup_heredoc_fork_error(int *pipefd, t_shell *shell)
{
	close_fd_pair(pipefd);
	close_fd_pair(shell->pipe_lineno);
	signal(SIGINT, handler);
	return (-1);
}

static void	spawn_heredoc_child(char *delimiter, int *pipefd, t_shell *shell)
{
	close(pipefd[0]);
	close(shell->pipe_lineno[0]);
	run_heredoc_child(delimiter, pipefd[1], shell);
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
		return (cleanup_heredoc_fork_error(pipefd, shell));
	if (pid == 0)
		spawn_heredoc_child(delimiter, pipefd, shell);
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

static void	print_heredoc_warning(t_shell *shell, char *delimiter)
{
	char	*line;

	line = ft_itoa(shell->line_count);
	ft_putstr_fd("minishell: warning: here-document at line ", 1);
	ft_putstr_fd(line, 1);
	ft_putstr_fd(" delimited by end-of-file (wanted `", 1);
	ft_putstr_fd(delimiter, 1);
	ft_putstr_fd("`)\n", 1);
	ft_free((void *)&line, 0);
	line = read_line_count_fd(shell->pipe_lineno[0]);
	shell->line_count += ft_atoi(line);
	ft_free((void *)&line, 0);
}

static char	*read_line_count_fd(int fd)
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
