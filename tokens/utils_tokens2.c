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

static void	no_equal_split(char *var, char **key, char **value)
{
	*key = ft_strdup(var);
	*value = ft_strdup("");
}

static void	equal_split(char *var, char *tmp, char **key, char **value)
{
	int	k_size;

	k_size = (tmp - var) + 1;
	*key = malloc(k_size);
	if (*key)
		ft_strlcpy(*key, var, k_size);
	*value = ft_strdup(tmp + 1);
}

static void	rollback_key_value(char **key, char **value)
{
	if (!*key || !*value)
	{
		free(*key);
		free(*value);
		*key = NULL;
		*value = NULL;
	}
}

void	var_get_key_value(char *var, char **key, char **value)
{
	char	*tmp;

	tmp = ft_strchr(var, '=');
	if (!tmp)
		no_equal_split(var, key, value);
	else
		equal_split(var, tmp, key, value);
	rollback_key_value(key, value);
}
