#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

static void	init_minishell(t_shell *shell)
{
	ft_bzero(shell, sizeof(t_shell));
}

static int	fetch_user_line(t_shell *shell)
{
	g_signal = 0;
	shell->input = ft_readline(shell->prompt);
	if (!shell->input)
	{
		if (isatty(STDIN_FILENO))
			ft_putstr_fd("exit\n", STDOUT_FILENO);
		return (0);
	}
	if (g_signal == SIGINT)
	{
		ft_free((void **)&shell->input, 0);
		shell->input = NULL;
		shell->exit_status = 130;
		g_signal = 0;
		return (2);
	}
	if (shell->input[0] == '\0')
	{
		ft_free((void **)&shell->input, 0);
		return (2);
	}
	return (1);
}

static int	parse_and_prepare(t_shell *shell)
{
	int	status;

	status = ft_validate_quotes(shell->input);
	if (status != 0)
	{
		shell->exit_status = status;
		return (0);
	}
	shell->tokens = ft_tokenize(shell->input);
	if (!shell->tokens || !ft_check_syntax(shell->tokens))
	{
		if (shell->tokens)
			shell->exit_status = 2;
		return (0);
	}
	add_history(shell->input);
	if (shell->input[0] == '#')
		return (0);
	ft_expasion(&shell->tokens, &shell->exit_status, shell->vars);
	shell->cmds = ft_parser(shell->tokens);
	return (shell->cmds != NULL);
}

static void	execute_pipeline(t_shell *shell)
{
	shell->length = ft_lstsize(shell->cmds);
	if (process_all_heredocs(shell->cmds, shell) == 0)
	{
		envp_update(shell);
		execute(shell);
	}
}

static void	start_prompt_loop(t_shell *shell)
{
	int	status;

	while (1)
	{
		status = fetch_user_line(shell);
		shell->error = status;
		shell->line_count++;
		if (status == 0)
			break ;
		if (status == 2)
			continue ;
		if (parse_and_prepare(shell))
			execute_pipeline(shell);
		clear_shell(shell);
		ft_prompt(&shell->prompt, shell->vars);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	setup_prompt_signal();
	init_minishell(&shell);
	shell.vars = env_get(envp, NULL);
	ft_prompt(&shell.prompt, shell.vars);
	start_prompt_loop(&shell);
	env_delete(&shell.vars);
	free(shell.prompt);
	rl_clear_history();
	matriz_delete(shell.envp);
	return (shell.exit_status);
}
