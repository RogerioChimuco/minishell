/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_signal.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 15:15:39 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/07 17:04:06 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "../get_next_line/get_next_line.h"

void	setup_prompt_signal(void)
{
	signal(SIGINT, handler);
	signal(SIGQUIT, SIG_IGN);
}

void	handler_exec(int sig)
{
	g_signal = sig;
}

void	handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal = SIGINT;
		write(1, "\n", 1);
		rl_done = 1;
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}

char	*ft_readline(char *prompt)
{
	char	*line;

	if (isatty(fileno(stdin)))
		return (readline(prompt));
	else
	{
		line = get_next_line(fileno(stdin));
		prompt = ft_strtrim(line, "\n");
		free(line);
	}
	return (prompt);
}
