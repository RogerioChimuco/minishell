
#include "minishell.h"

/*
** Allocate a blank t_cmd node.
*/
static t_cmd	*cmd_new(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->argc = 0;
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

/*
** Append one string to cmd->argv, growing the array.
** argv is always NULL-terminated.
** Returns 0 on success, -1 on failure.
*/
static int	cmd_push_arg(t_cmd *cmd, char *word)
{
	char	**new_argv;
	int		i;

	new_argv = malloc(sizeof(char *) * (cmd->argc + 2));
	if (!new_argv)
		return (-1);
	i = 0;
	while (i < cmd->argc)
	{
		new_argv[i] = cmd->argv[i];
		i++;
	}
	new_argv[i] = word;
	new_argv[i + 1] = NULL;
	free(cmd->argv);
	cmd->argv = new_argv;
	cmd->argc++;
	return (0);
}

/*
** Append a redirection entry to cmd->redirs list.
** Returns 0 on success, -1 on allocation failure.
*/
static int	cmd_push_redir(t_cmd *cmd, t_tok kind, char *target)
{
	t_redir	*rd;
	t_redir	*tail;

	rd = malloc(sizeof(t_redir));
	if (!rd)
		return (-1);
	rd->kind = kind;
	rd->target = target;
	rd->next = NULL;
	if (!cmd->redirs)
	{
		cmd->redirs = rd;
		return (0);
	}
	tail = cmd->redirs;
	while (tail->next)
		tail = tail->next;
	tail->next = rd;
	return (0);
}

/*
** Free a single t_cmd node (does not follow ->next).
*/
static void	cmd_free_one(t_cmd *cmd)
{
	t_redir	*rd;
	t_redir	*rn;
	int		i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (i < cmd->argc)
			msh_free((void **)&cmd->argv[i++]);
		free(cmd->argv);
	}
	rd = cmd->redirs;
	while (rd)
	{
		rn = rd->next;
		msh_free((void **)&rd->target);
		free(rd);
		rd = rn;
	}
	free(cmd);
}

/*
** Free the entire pipeline list.
*/
void	pipeline_free(t_cmd **head)
{
	t_cmd	*cur;
	t_cmd	*nxt;

	if (!head || !*head)
		return ;
	cur = *head;
	while (cur)
	{
		nxt = cur->next;
		cmd_free_one(cur);
		cur = nxt;
	}
	*head = NULL;
}

/*
** Parse one command segment (tokens up to the next pipe or end).
** *tok advances past all consumed tokens.
** Returns the new t_cmd, or NULL on allocation failure.
*/
static t_cmd	*parse_cmd(t_token **tok)
{
	t_cmd	*cmd;
	t_tok	rk;
	char	*word;

	cmd = cmd_new();
	if (!cmd)
		return (NULL);
	while (*tok && (*tok)->kind != TOK_PIPE)
	{
		if ((*tok)->kind == TOK_WORD)
		{
			word = msh_strdup((*tok)->raw);
			if (!word || cmd_push_arg(cmd, word) < 0)
				return (msh_free((void **)&word), cmd_free_one(cmd), NULL);
			*tok = (*tok)->next;
		}
		else
		{
			rk = (*tok)->kind;
			*tok = (*tok)->next;
			word = msh_strdup((*tok)->raw);
			if (!word || cmd_push_redir(cmd, rk, word) < 0)
				return (msh_free((void **)&word), cmd_free_one(cmd), NULL);
			*tok = (*tok)->next;
		}
	}
	return (cmd);
}

/*
** Build a pipeline (linked list of t_cmd) from the token list.
** Returns head of the list, or NULL on allocation failure.
*/
t_cmd	*parser_run(t_token *toks)
{
	t_cmd	*head;
	t_cmd	*tail;
	t_cmd	*cmd;

	head = NULL;
	tail = NULL;
	while (toks)
	{
		cmd = parse_cmd(&toks);
		if (!cmd)
			return (pipeline_free(&head), NULL);
		if (!head)
			head = cmd;
		else
			tail->next = cmd;
		tail = cmd;
		if (toks && toks->kind == TOK_PIPE)
			toks = toks->next;
	}
	return (head);
}
