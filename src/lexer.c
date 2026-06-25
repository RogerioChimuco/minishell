
#include "minishell.h"

/*
** Allocate and initialise a single token node.
*/
static t_token	*tok_new(t_tok kind, const char *raw, size_t len)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->kind = kind;
	tok->raw = malloc(len + 1);
	if (!tok->raw)
		return (free(tok), NULL);
	memcpy(tok->raw, raw, len);
	tok->raw[len] = '\0';
	tok->next = NULL;
	return (tok);
}

/*
** Append a token to the end of *lst (updating *tail for O(1) append).
*/
static int	tok_append(t_token **head, t_token **tail, t_token *tok)
{
	if (!tok)
		return (-1);
	if (!*head)
		*head = tok;
	else
		(*tail)->next = tok;
	*tail = tok;
	return (0);
}

/*
** Skip a quoted span starting at line[*i] (which must be '\'' or '"').
** Advances *i past the closing quote.  Returns 1 on success, 0 if
** the closing quote is missing (unclosed quote error).
*/
static int	skip_quoted(const char *line, size_t *i)
{
	char	q;

	q = line[*i];
	(*i)++;
	while (line[*i] && line[*i] != q)
		(*i)++;
	if (!line[*i])
		return (0);
	(*i)++;
	return (1);
}

/*
** Collect a WORD token starting at line[start].
** Stops at whitespace or an operator character not inside quotes.
** Advances *i past the last character of the word.
*/
static t_token	*lex_word(const char *line, size_t *i)
{
	size_t	start;

	start = *i;
	while (line[*i]
		&& !msh_isspace(line[*i])
		&& line[*i] != '|'
		&& line[*i] != '<'
		&& line[*i] != '>')
	{
		if (line[*i] == '\'' || line[*i] == '"')
		{
			if (!skip_quoted(line, i))
				return (NULL);
		}
		else
			(*i)++;
	}
	return (tok_new(TOK_WORD, line + start, *i - start));
}

/*
** Consume an operator token at line[*i] (<, >, >>, <<, |).
*/
static t_token	*lex_op(const char *line, size_t *i)
{
	char	c;
	char	nc;

	c = line[*i];
	nc = line[*i + 1];
	if (c == '|')
		return ((*i)++, tok_new(TOK_PIPE, "|", 1));
	if (c == '>' && nc == '>')
		return ((*i) += 2, tok_new(TOK_REDIR_APPEND, ">>", 2));
	if (c == '<' && nc == '<')
		return ((*i) += 2, tok_new(TOK_HEREDOC, "<<", 2));
	if (c == '>')
		return ((*i)++, tok_new(TOK_REDIR_OUT, ">", 1));
	if (c == '<')
		return ((*i)++, tok_new(TOK_REDIR_IN, "<", 1));
	return (NULL);
}

/*
** Free all tokens in a list and set *lst to NULL.
*/
void	tokens_free(t_token **lst)
{
	t_token	*cur;
	t_token	*nxt;

	if (!lst || !*lst)
		return ;
	cur = *lst;
	while (cur)
	{
		nxt = cur->next;
		msh_free((void **)&cur->raw);
		free(cur);
		cur = nxt;
	}
	*lst = NULL;
}

/*
** Main lexer entry-point.
** Tokenises 'line' and returns the head of the token list,
** or NULL on allocation failure or unclosed quote.
*/
t_token	*lexer_run(const char *line)
{
	t_token	*head;
	t_token	*tail;
	t_token	*tok;
	size_t	i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (line[i])
	{
		while (msh_isspace(line[i]))
			i++;
		if (!line[i])
			break ;
		if (line[i] == '|' || line[i] == '<' || line[i] == '>')
			tok = lex_op(line, &i);
		else
			tok = lex_word(line, &i);
		if (!tok || tok_append(&head, &tail, tok) < 0)
		{
			msh_free((void **)&tok);
			tokens_free(&head);
			return (NULL);
		}
	}
	return (head);
}
