/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_tokens2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:54:08 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:54:09 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

void	free_token_partial(t_token *token)
{
	if (!token)
		return ;
	if (token->value)
		free(token->value);
	if (token->original_value)
		free(token->original_value);
	free(token);
}

int	ft_extract_operator(t_list **tokens, const char *line, int *n)
{
	int	oplen;

	oplen = 1 + (line[*n + 1] && (line[*n] == '<' || line[*n] == '>')
			&& line[*n] == line[*n + 1]);
	ft_add_token(tokens, line, *n, oplen);
	*n += oplen;
	return (1);
}

void	ft_add_token(t_list **tokens, const char *line,
			int start, int length)
{
	t_token	*token;
	t_list	*node;

	token = token_create(line, start, length);
	if (!token)
		return ;
	node = ft_lstnew(token);
	if (!node)
		return (free_token_partial(token));
	ft_lstadd_back(tokens, node);
}
