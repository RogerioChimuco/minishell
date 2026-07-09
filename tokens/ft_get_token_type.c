/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_token_type.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:52:54 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:53:02 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

typedef struct s_op_map
{
	const char		*op;
	t_token_type	type;
}	t_op_map;

static t_token_type	classify_operator_type(char *operator, size_t length);

void	ft_get_token_type(void *arg)
{
	size_t	length;
	t_token	*token;

	if (!arg)
		return ;
	token = (t_token *) arg;
	length = ft_strlen(token->value);
	token->type = classify_operator_type(token->value, length);
}

static t_token_type	classify_operator_type(char *operator, size_t length)
{
	static const t_op_map	map[5] = {
		{"<", REDIR_IN},
		{">", REDIR_OUT},
		{"|", PIPE},
		{"<<", HEREDOC},
		{">>", REDIR_APPEND}
	};
	int						i;

	if (length < 1 || length > 2)
		return (WORD);
	i = -1;
	while (++i < 5)
	{
		if (ft_strlen(map[i].op) == length
			&& !ft_strncmp(operator, map[i].op, length))
			return (map[i].type);
	}
	return (WORD);
}

t_list	*find_previous_node(t_list *head, t_list *node)
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
