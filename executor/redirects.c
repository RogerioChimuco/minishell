/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirects.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 09:22:15 by ckulembe          #+#    #+#             */
/*   Updated: 2026/03/24 13:01:18 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "redir.h"

int	redir_in(char *filename)
{
	int	filename_fd;

	filename_fd = open(filename, O_RDONLY);
	if (filename_fd == -1)
		return (-1);
	return (filename_fd);
}

int	redir_out(char *filename)
{
	int	filename_fd;

	filename_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (filename_fd == -1)
		return (-1);
	return (filename_fd);
}

int	redir_append(char *filename)
{
	int	filename_fd;

	filename_fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0777);
	if (filename_fd == -1)
		return (-1);
	return (filename_fd);
}

int	redir_heredoc(char *delimiter, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pid;

	pid = heredoc_fork(delimiter, pipefd, shell);
	if (pid == -1)
		return (-1);
	return (heredoc_wait(pid, pipefd, delimiter, shell));
}

int	open_redir_fd(int *fd, char *value, int (*f)(char *))
{
	*fd = f(value);
	return (*fd);
}
