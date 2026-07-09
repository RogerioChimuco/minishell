
#include "parse.h"

t_list	*ft_word_count(t_list *lst, size_t *length)
{
	t_token	*token;

	if (!lst)
		return (0);
	*length = 0;
	while (lst)
	{
		token = (t_token *)lst->content;
		if (token->type == PIPE)
			return (lst->next);
		else if (token->type == WORD)
			(*length)++;
		else
			lst = lst->next;
		lst = lst->next;
	}
	return (lst);
}

t_redir	*ft_redirnew(t_list *lst)
{
	t_token	*token;
	t_token	*tkn_next;
	t_redir	*redir;
	t_list	*next;

	if (!lst)
		return (NULL);
	token = (t_token *)lst->content;
	redir = (t_redir *) malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	next = lst->next;
	tkn_next = (t_token *)next->content;
	redir->value = ft_strdup(tkn_next->value);
	redir->original_value = ft_strdup(tkn_next->original_value);
	if (!redir->value || !redir->original_value)
	{
		free(redir->value);
		free(redir->original_value);
		free(redir);
		return (NULL);
	}
	redir->fd = -1;
	redir->type = token->type;
	return (redir);
}
