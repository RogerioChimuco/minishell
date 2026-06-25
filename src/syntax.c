
#include "minishell.h"

/*
** Walk 'line' and check that every opened quote is properly closed.
** Returns 1 if quotes are balanced, 0 otherwise.
*/
int	syntax_quotes_ok(const char *line)
{
	char	in_quote;
	int		i;

	in_quote = 0;
	i = 0;
	while (line[i])
	{
		if (!in_quote && (line[i] == '\'' || line[i] == '"'))
			in_quote = line[i];
		else if (in_quote && line[i] == in_quote)
			in_quote = 0;
		i++;
	}
	return (in_quote == 0);
}

/*
** Print a bash-style syntax error for an unexpected token.
** token == NULL means "unexpected end of input".
*/
static void	syntax_error(const char *token)
{
	if (token)
		fprintf(stderr,
			"minishell: syntax error near unexpected token `%s'\n", token);
	else
		fprintf(stderr,
			"minishell: syntax error near unexpected token `newline'\n");
}

/*
** Return 1 if tok is an operator that cannot start or end a pipeline
** or appear right after another operator.
*/
static int	is_op(t_tok kind)
{
	return (kind == TOK_PIPE
		|| kind == TOK_REDIR_IN
		|| kind == TOK_REDIR_OUT
		|| kind == TOK_REDIR_APPEND
		|| kind == TOK_HEREDOC);
}

/*
** Validate the token list produced by the lexer.
** Rules checked:
**   1. First token must not be a pipe.
**   2. Every operator (< > >> <<) must be followed by a WORD.
**   3. A pipe must be followed by at least one token (not another pipe or EOF).
**   4. The last token must not be a pipe or redirection operator.
** Returns 1 if valid, 0 on first error found.
*/
int	syntax_tokens_ok(t_token *toks)
{
	t_token	*cur;

	if (!toks)
		return (1);
	if (toks->kind == TOK_PIPE)
		return (syntax_error("|"), 0);
	cur = toks;
	while (cur)
	{
		if (cur->kind == TOK_REDIR_IN || cur->kind == TOK_REDIR_OUT
			|| cur->kind == TOK_REDIR_APPEND || cur->kind == TOK_HEREDOC)
		{
			if (!cur->next || cur->next->kind != TOK_WORD)
			{
				syntax_error(cur->next ? cur->next->raw : NULL);
				return (0);
			}
		}
		if (cur->kind == TOK_PIPE)
		{
			if (!cur->next || is_op(cur->next->kind))
			{
				syntax_error(cur->next ? cur->next->raw : NULL);
				return (0);
			}
		}
		cur = cur->next;
	}
	return (1);
}
