/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:38:31 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/09 13:09:08 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

static void	shell_init(t_shell *shell);
static int	read_input(t_shell *shell);
static int	process_pipeline(t_shell *shell);
static void	shell_loop(t_shell *shell);

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argv;
	(void)argc;
	setup_prompt_signal();
	shell_init(&shell);
	shell.vars = env_get(envp, NULL);
	ft_prompt(&shell.prompt, shell.vars);
	shell_loop(&shell);
	env_delete(&shell.vars);
	free(shell.prompt);
	rl_clear_history();
	if (shell.envp)
		matriz_delete(shell.envp);
	return (shell.exit_status);
}

static void	shell_loop(t_shell *shell)
{
	while (1)
	{
		shell->error = read_input(shell);
		shell->line_count += 1;
		if (!shell->error)
			break ;
		else if (shell->error == 2)
			continue ;
		if (!process_pipeline(shell))
		{
			shell_clear(shell);
			continue ;
		}
		shell->length = ft_lstsize(shell->cmds);
		if (foreach_cmd(shell->cmds, shell) != 0)
		{
			shell_clear(shell);
			continue ;
		}
		envp_update(shell);
		execute(shell);
		shell_clear(shell);
		ft_prompt(&shell->prompt, shell->vars);
	}
	return ;
}

static void	shell_init(t_shell *shell)
{
	shell->prompt = NULL;
	shell->cmds = NULL;
	shell->vars = NULL;
	shell->tokens = NULL;
	shell->input = NULL;
	shell->pipes = NULL;
	shell->pids = NULL;
	shell->envp = NULL;
	shell->cmd_i = 0;
	shell->pid_i = 0;
	shell->exit_status = 0;
	shell->line_count = 0;
}

static int	read_input(t_shell *shell)
{
	g_signal = 0;
	shell->input = ft_readline(shell->prompt);
	if (shell->input == NULL)
	{
		if (isatty(0))
			write (1, "exit\n", 5);
		return (0);
	}
	if (g_signal == SIGINT)
	{
		if (shell->input)
		{
			ft_free((void *)&shell->input, 0);
			shell->input = NULL;
		}
		shell->exit_status = 130;
		g_signal = 0;
		return (2);
	}
	if (shell->input == NULL)
		return ((ft_putstr_fd("exit\n", 1)), 0);
	if (shell->input[0] == '\0')
		return ((ft_free((void *)&shell->input, 0)), 2);
	return (1);
}

static int	process_pipeline(t_shell *shell)
{
	shell->error = ft_validate_quotes(shell->input);
	if (shell->error)
	{
		shell->exit_status = shell->error;
		return (0);
	}
	shell->tokens = ft_tokenize(shell->input);
	if (!shell->tokens)
		return (0);
	if (!ft_check_syntax(shell->tokens))
	{
		shell->exit_status = 2;
		return (0);
	}
	add_history(shell->input);
	if (shell->input[0] == '#')
		return (0);
	ft_expasion(&shell->tokens, &shell->exit_status, shell->vars);
	shell->cmds = ft_parser(shell->tokens);
	if (!shell->cmds)
		return (0);
	return (1);
}
