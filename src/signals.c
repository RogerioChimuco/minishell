/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:01:33 by luqalmei          #+#    #+#             */
/*   Updated: 2026/06/09 15:01:38 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../include/minishell.h"

/*
** sig_handler — SIGINT handler used at the prompt.
** Prints a newline, clears the readline buffer, and redraws the prompt.
*/
void	sig_handler(int s)
{
	if (s == SIGINT)
	{
		g_sig = SIGINT;
		write(STDOUT_FILENO, "\n", 1);
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}

/*
** sig_interactive — install the interactive-mode signal dispositions.
** SIGINT → sig_handler (prints newline and resets the line).
** SIGQUIT → SIG_IGN  (Ctrl-\ does nothing at the prompt).
*/
void	sig_interactive(void)
{
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);
}

/*
** sig_exec — install signal dispositions used while a child is running.
** Both SIGINT and SIGQUIT are forwarded to the foreground process group
** by the kernel; the shell itself ignores them so it does not die.
*/
void	sig_exec(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
