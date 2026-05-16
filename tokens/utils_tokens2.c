/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_tokens2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 22:34:21 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/09 22:34:26 by ahuanga          ###   ########.fr       */
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

	oplen = 1;
	if (line[*n + 1] && (line[*n] == '<' || line[*n] == '>')
		&& line[*n] == line[*n + 1])
		oplen = 2;
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

void	var_get_key_value(char *var, char **key, char **value)
{
	char	*tmp;
	int		k_size;

	tmp = ft_strchr(var, '=');
	if (!tmp)
	{
		*key = ft_strdup(var);
		*value = ft_strdup("");
	}
	else
	{
		k_size = (tmp - var) + 1;
		*key = malloc(k_size);
		ft_strlcpy(*key, var, k_size);
		*value = ft_strdup(tmp + 1);
	}
	if (!*key || !*value)
	{
		free(*key);
		free(*value);
		*value = NULL;
		*key = NULL;
	}
}
