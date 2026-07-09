/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:58:13 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:58:16 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "redir.h"

static void	check_delimiter_quotes(char *delimiter, int *quoted);
static int	read_heredoc_line(char **line, int *_exit);
static void	finish_heredoc_child(t_shell *shell, char *line, int counter);

void	run_heredoc_child(char *delimiter, int write_fd, t_shell *shell)
{
	int		counter;
	int		_exit;
	int		quoted;
	char	*line;

	_exit = 0;
	counter = 0;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	check_delimiter_quotes(delimiter, &quoted);
	while (++counter && !read_heredoc_line(&line, &_exit)
		&& ft_strcmp(delimiter, line))
	{
		if (!quoted)
			line = heredoc_expand(line, shell);
		ft_putendl_fd(line, write_fd);
		ft_free((void *)&line, 0);
	}
	finish_heredoc_child(shell, line, counter);
	close(write_fd);
	exit (_exit);
}

static void	check_delimiter_quotes(char *delimiter, int *quoted)
{
	*quoted = (ft_strchr(delimiter, '\'') || ft_strchr(delimiter, '"')) != 0;
	if (*quoted)
		ft_remove_quotes(delimiter);
}

static int	read_heredoc_line(char **line, int *_exit)
{
	*line = ft_readline("> ");
	*_exit = (*line == NULL);
	return (*_exit);
}

static void	finish_heredoc_child(t_shell *shell, char *line, int counter)
{
	ft_free((void *)&line, 0);
	ft_putnbr_fd(counter, shell->pipe_lineno[1]);
	close(shell->pipe_lineno[1]);
	clear_shell(shell);
}
