/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 13:00:31 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 13:00:35 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*read_line_count_fd(int fd)
{
	char	*buffer;
	ssize_t	n;

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

void	print_heredoc_warning(t_shell *shell, char *delimiter)
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
