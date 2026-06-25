#include "minishell.h"

volatile sig_atomic_t	g_sig = 0;

/*
** Handler used in interactive mode.
** SIGINT prints a new prompt line; SIGQUIT is ignored.
*/
static void	handler_interactive(int sig)
{
	g_sig = sig;
	if (sig == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

/*
** Interactive mode: SIGINT triggers a newline + redisplay,
** SIGQUIT is silenced.
*/
void	signals_interactive(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler_interactive;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

/*
** Execution mode: restore default behaviour so children receive signals.
*/
void	signals_exec(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}
