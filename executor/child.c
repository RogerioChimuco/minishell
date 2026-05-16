/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 13:01:09 by ahuanga           #+#    #+#             */
/*   Updated: 2026/03/30 14:41:20 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "redir.h"

static void	quote_management(char *delimiter, int *quoted);
static int	readline_helper(char **line, int *_exit);
static void	help_child(t_shell *shell, char *line, int counter);

void	heredoc_child(char *delimiter, int write_fd, t_shell *shell)
{
	int		counter;
	int		_exit;
	int		quoted;
	char	*line;

	_exit = 0;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	counter = 0;
	quote_management(delimiter, &quoted);
	while (++counter)
	{
		if (readline_helper(&line, &_exit) || !ft_strcmp(delimiter, line))
			break ;
		if (!quoted)
			line = heredoc_expand(line, shell);
		ft_putendl_fd(line, write_fd);
		ft_free((void *)&line, 0);
	}
	help_child(shell, line, counter);
	close(write_fd);
	exit (_exit);
}

static void	quote_management(char *delimiter, int *quoted)
{
	*quoted = 0;
	if (ft_strchr(delimiter, '\'') || ft_strchr(delimiter, '"'))
	{
		ft_remove_quotes(delimiter);
		*quoted = 1;
	}
}

static int	readline_helper(char **line, int *_exit)
{
	*line = ft_readline("> ");
	if (*line == NULL)
	{
		*_exit = 1;
		return (1);
	}
	return (0);
}

static void	help_child(t_shell *shell, char *line, int counter)
{
	ft_free((void *)&line, 0);
	ft_putnbr_fd(counter, shell->pipe_lineno[1]);
	close(shell->pipe_lineno[1]);
	shell_clear(shell);
}
