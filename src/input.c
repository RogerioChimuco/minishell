/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:00:04 by luqalmei          #+#    #+#             */
/*   Updated: 2026/06/09 15:00:22 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <fcntl.h>

/*
** ms_readline — thin wrapper around readline / getline.
** When stdin is a terminal use readline (gives line editing + history).
** When stdin is a pipe or file, read a raw line with read(2) so the shell
** can be driven non-interactively (e.g. in test harnesses).
*/
char	*ms_readline(const char *prompt)
{
	char	buf[4096];
	char	*nl;
	ssize_t	n;

	if (isatty(STDIN_FILENO))
		return (readline(prompt));
	n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
	if (n <= 0)
		return (NULL);
	buf[n] = '\0';
	nl = ft_strchr(buf, '\n');
	if (nl)
		*nl = '\0';
	return (ft_strdup(buf));
}

/*
** ms_validate — scan the line for unclosed single or double quotes.
** Returns 1 when the line is valid, 0 otherwise (sets exit_code = 2
** in the caller).
*/
int	ms_validate(const char *line)
{
	t_qstate	state;
	int			i;

	state = Q_NONE;
	i = -1;
	while (line[++i])
	{
		if (state == Q_NONE && line[i] == '\'')
			state = Q_SINGLE;
		else if (state == Q_NONE && line[i] == '"')
			state = Q_DOUBLE;
		else if (state == Q_SINGLE && line[i] == '\'')
			state = Q_NONE;
		else if (state == Q_DOUBLE && line[i] == '"')
			state = Q_NONE;
	}
	if (state != Q_NONE)
	{
		ft_putendl_fd("minishell: syntax error: unclosed quote", STDERR_FILENO);
		return (0);
	}
	return (1);
}
