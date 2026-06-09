/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:01:05 by luqalmei          #+#    #+#             */
/*   Updated: 2026/06/09 15:01:11 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "include/minishell.h"

volatile sig_atomic_t	g_sig = 0;

/*
** ms_init — zero-fill the shell struct and build the env list from envp.
** Called once at startup before entering the main loop.
*/
void	ms_init(t_shell *sh, char **envp)
{
	sh->prompt = NULL;
	sh->line = NULL;
	sh->tokens = NULL;
	sh->cmds = NULL;
	sh->env = env_build(envp);
	sh->envp = NULL;
	sh->exit_code = 0;
	sh->ncmds = 0;
	sh->pipes = NULL;
	sh->pids = NULL;
	sh->npids = 0;
	sig_interactive();
	build_prompt(sh);
}

/*
** ms_reset — release per-cycle allocations (tokens, cmds, line, envp array).
** Called at the end of every iteration so the next readline is clean.
*/
void	ms_reset(t_shell *sh)
{
	if (sh->line)
	{
		free(sh->line);
		sh->line = NULL;
	}
	if (sh->tokens)
		node_free_all(&sh->tokens, free_token);
	if (sh->cmds)
		node_free_all(&sh->cmds, free_cmd);
	if (sh->envp)
	{
		free_matrix(sh->envp);
		sh->envp = NULL;
	}
	if (sh->pipes)
	{
		free(sh->pipes);
		sh->pipes = NULL;
	}
	if (sh->pids)
	{
		free(sh->pids);
		sh->pids = NULL;
	}
	sh->ncmds = 0;
	sh->npids = 0;
}

/*
** ms_destroy — called once on clean exit to free the env list and prompt.
*/
void	ms_destroy(t_shell *sh)
{
	ms_reset(sh);
	env_free(&sh->env);
	if (sh->prompt)
		free(sh->prompt);
	rl_clear_history();
}

/*
** read_line — call readline (or getline when stdin is not a tty).
** Returns NULL on EOF, sets g_sig when Ctrl-C interrupts.
*/
static char	*read_line(t_shell *sh)
{
	char	*line;

	g_sig = 0;
	line = ms_readline(sh->prompt);
	if (!line)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "exit\n", 5);
		return (NULL);
	}
	if (g_sig == SIGINT)
	{
		free(line);
		sh->exit_code = 130;
		return (NULL);
	}
	return (line);
}

/*
** shell_loop — the main REPL.
** Reads a line, validates quotes, tokenises, parses, expands, executes.
*/
static void	shell_loop(t_shell *sh)
{
	while (1)
	{
		sh->line = read_line(sh);
		if (!sh->line)
			break ;
		if (sh->line[0] == '\0')
		{
			free(sh->line);
			sh->line = NULL;
			continue ;
		}
		if (!ms_validate(sh->line))
		{
			sh->exit_code = 2;
			ms_reset(sh);
			build_prompt(sh);
			continue ;
		}
		add_history(sh->line);
		ms_reset(sh);
		build_prompt(sh);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	sh;

	(void)argc;
	(void)argv;
	ms_init(&sh, envp);
	shell_loop(&sh);
	ms_destroy(&sh);
	return (sh.exit_code);
}
