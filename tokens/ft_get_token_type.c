/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_token_type.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 09:50:29 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/07 18:06:49 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

static t_token_type	ft_type(char *operator, size_t length);

void	ft_get_token_type(void *arg)
{
	size_t	length;
	t_token	*token;

	if (!arg)
		return ;
	token = (t_token *) arg;
	length = ft_strlen(token->value);
	token->type = ft_type(token->value, length);
	return ;
}

static t_token_type	ft_type(char *operator, size_t length)
{
	t_token_type	type;

	type = WORD;
	if (length == 1)
	{
		if (!ft_strncmp(operator, "<", length))
			type = REDIR_IN;
		else if (!ft_strncmp(operator, ">", length))
			type = REDIR_OUT;
		else if (!ft_strncmp(operator, "|", length))
			type = PIPE;
	}
	else if (length == 2)
	{
		if (!ft_strncmp(operator, "<<", length))
			type = HEREDOC;
		else if (!ft_strncmp(operator, ">>", length))
			type = REDIR_APPEND;
	}
	return (type);
}

t_list	*find_prev(t_list *head, t_list *node)
{
	while (head && head->next != node)
		head = head->next;
	return (head);
}

t_quote_state	ft_get_quote_state(const char *value)
{
	int	has_single;
	int	has_double;
	int	i;

	has_single = 0;
	has_double = 0;
	i = -1;
	while (value[++i])
	{
		if (value[i] == '\'')
			has_single = 1;
		else if (value[i] == '"')
			has_double = 1;
	}
	if (has_single)
		return (SINGLE_QUOTES);
	if (has_double)
		return (DOUBLE_QUOTES);
	return (NO_QUOTES);
}
