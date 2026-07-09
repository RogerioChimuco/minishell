/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_signal.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:54:53 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:54:55 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

static void	reset_prompt_line(void)
{
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	rl_done = 1;
}

void	handler(int sig)
{
	if (sig != SIGINT)
		return ;
	g_signal = SIGINT;
	reset_prompt_line();
}

void	handler_exec(int sig)
{
	static const char	*msgs[] = {"\n", "Quit (core dumped)\n"};

	g_signal = sig;
	if (sig == SIGINT)
		write(1, msgs[0], 1);
	else if (sig == SIGQUIT)
		write(1, msgs[1], 19);
}

void	setup_prompt_signal(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = handler;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

void	ft_prompt(char **prompt, t_env *env)
{
	char	*user;
	char	*path;
	char	*step;

	if (*prompt)
		ft_free((void **)prompt, NULL);
	user = prompt_user(env);
	path = prompt_path(env);
	step = concat_and_free(user, ":");
	if (step)
		step = concat_and_free(step, path);
	free(path);
	if (!step)
	{
		*prompt = ft_strdup("minishel$ ");
		return ;
	}
	*prompt = concat_and_free(step, "$ ");
	if (!*prompt)
		*prompt = ft_strdup("minishel$ ");
}
