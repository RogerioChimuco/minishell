/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tokenize.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 12:47:47 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/09 18:30:56 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

t_token	*token_create(const char *line, int start, int length)
{
	t_token	*token;

	token = (t_token *) malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = ft_substr(line, start, (size_t)length);
	if (!token->value)
		return (free(token), NULL);
	token->original_value = ft_strdup(token->value);
	if (!token->original_value)
		return (free_token_partial(token), NULL);
	token->exp = 0;
	token->quote_state = ft_get_quote_state(token->value);
	ft_get_token_type(token);
	return (token);
}

t_list	*ft_tokenize(const char *line)
{
	int		i;
	t_list	*list;
	t_list	*tokens;

	tokens = ft_lstnew("init");
	if (!tokens)
		return (NULL);
	i = 0;
	while (*(line + i))
	{
		while (ft_is_space(*(line + i)))
			i++;
		if (!line[i])
			break ;
		if (ft_is_operator(line, i))
		{
			ft_extract_operator(&tokens, line, &i);
			continue ;
		}
		ft_extract_word(&tokens, line, &i, i);
	}
	list = tokens->next;
	return (free(tokens), list);
}

int	ft_extract_word_in(t_list **tokens, const char *line, t_data d)
{
	char	*_addr;

	if (ft_is_quote(line[*(d.i)]))
	{
		_addr = ft_strchr((line + *(d.i) + 1), *(line + *(d.i)));
		if (!_addr)
		{
			*(d.n) = ft_strlen(line);
			ft_add_token(tokens, line, d.start, *d.i - d.start);
			return (1);
		}
		*(d.i) = _addr - line;
	}
	return (0);
}

void	ft_extract_word(t_list **tokens,
			const char *line, int *n, int start)
{
	int	i;

	i = *n;
	while (*(line + i) && !ft_is_space(*(line + i)))
	{
		if (ft_is_operator(line, i))
		{
			ft_add_token(tokens, line, start, i - start);
			*n = i;
			return ;
		}
		if (ft_extract_word_in(tokens, line, ft_data(start, &i, n)))
			return ;
		if (!line[i + 1] && (ft_is_operator(line, i + 1)
				|| ft_is_space(line[i + 1])))
		{
			ft_add_token(tokens, line, start, i - start);
			*n = i + 1;
			break ;
		}
		i++;
	}
	if (i > start)
		ft_add_token(tokens, line, start, i - start);
	*n = i;
}
