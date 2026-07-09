/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirects.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 13:00:14 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 13:00:17 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "redir.h"

int	open_input_redir(char *filename)
{
	return (open(filename, O_RDONLY));
}

int	open_output_redir(char *filename)
{
	return (open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777));
}

int	open_append_redir(char *filename)
{
	return (open(filename, O_WRONLY | O_CREAT | O_APPEND, 0777));
}

int	open_heredoc_redir(char *delimiter, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pid;

	pid = heredoc_fork(delimiter, pipefd, shell);
	if (pid == -1)
		return (-1);
	return (heredoc_wait(pid, pipefd, delimiter, shell));
}

int	open_file_redir(int *fd, char *value, int (*f)(char *))
{
	*fd = f(value);
	return (*fd);
}
