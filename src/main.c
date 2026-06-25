
#include "minishell.h"

static void	msh_init(t_msh *sh, char **envp)
{
	sh->line = NULL;
	sh->prompt = NULL;
	sh->tokens = NULL;
	sh->pipeline = NULL;
	sh->status = 0;
	sh->env = env_build(envp);
}

static void	msh_cleanup(t_msh *sh)
{
	msh_free((void **)&sh->line);
	msh_free((void **)&sh->prompt);
	tokens_free(&sh->tokens);
	pipeline_free(&sh->pipeline);
	env_free(&sh->env);
}

/*
** Read one line of input.
** Returns: 1 ready, 0 EOF, -1 skip (empty / SIGINT).
*/
static int	msh_readline(t_msh *sh)
{
	g_sig = 0;
	msh_free((void **)&sh->prompt);
	sh->prompt = prompt_build(sh->env);
	sh->line = readline(sh->prompt);
	if (!sh->line)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "exit\n", 5);
		return (0);
	}
	if (g_sig == SIGINT)
	{
		msh_free((void **)&sh->line);
		sh->status = 130;
		return (-1);
	}
	if (sh->line[0] == '\0')
	{
		msh_free((void **)&sh->line);
		return (-1);
	}
	add_history(sh->line);
	return (1);
}

/*
** Run quote check, then lex, then syntax-validate tokens.
** Returns 1 on success, 0 on any error (sh->status set to 2).
*/
static int	msh_lex_and_validate(t_msh *sh)
{
	if (!syntax_quotes_ok(sh->line))
	{
		fprintf(stderr,
			"minishell: syntax error: unclosed quote\n");
		sh->status = 2;
		return (0);
	}
	tokens_free(&sh->tokens);
	sh->tokens = lexer_run(sh->line);
	if (!sh->tokens)
	{
		fprintf(stderr, "minishell: syntax error\n");
		sh->status = 2;
		return (0);
	}
	if (!syntax_tokens_ok(sh->tokens))
	{
		sh->status = 2;
		return (0);
	}
	return (1);
}

/*
** Parse the validated token list into a pipeline of t_cmd nodes.
** Returns 1 on success, 0 on allocation failure.
*/
static int	msh_parse(t_msh *sh)
{
	pipeline_free(&sh->pipeline);
	sh->pipeline = parser_run(sh->tokens);
	if (!sh->pipeline)
	{
		fprintf(stderr, "minishell: parse error\n");
		sh->status = 1;
		return (0);
	}
	return (1);
}

/*
** Main read-eval loop.
*/
static void	msh_loop(t_msh *sh)
{
	int	rc;

	while (1)
	{
		signals_interactive();
		rc = msh_readline(sh);
		if (rc == 0)
			break ;
		if (rc < 0)
			continue ;
		if (!msh_lex_and_validate(sh))
			continue ;
		if (!msh_parse(sh))
			continue ;
		/*
		** Executor will be added in commit 3.
		** Pipeline is built and ready in sh->pipeline.
		*/
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_msh	sh;

	(void)argc;
	(void)argv;
	msh_init(&sh, envp);
	msh_loop(&sh);
	msh_cleanup(&sh);
	rl_clear_history();
	return (sh.status);
}
