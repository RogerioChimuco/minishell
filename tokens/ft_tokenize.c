/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tokenize.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:53:09 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:53:11 by luqalmei         ###   ########.fr       */
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

int	extract_quoted_word(t_list **tokens, const char *line, t_data d)
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

static void	add_token_and_advance(t_list **tokens, const char *line,
		int start, int token_end, int n_val, int *n)
{
	ft_add_token(tokens, line, start, token_end - start);
	*n = n_val;
}

void	ft_extract_word(t_list **tokens,
			const char *line, int *n, int start)
{
	int	i;

	i = *n;
	while (*(line + i) && !ft_is_space(*(line + i)))
	{
		if (ft_is_operator(line, i))
			return (add_token_and_advance(tokens, line, start, i, i, n));
		if (extract_quoted_word(tokens, line, build_word_data(start, &i, n)))
			return ;
		if (!line[i + 1] && (ft_is_operator(line, i + 1)
				|| ft_is_space(line[i + 1])))
		{
			add_token_and_advance(tokens, line, start, i, i + 1, n);
			break ;
		}
		i++;
	}
	if (i > start)
		add_token_and_advance(tokens, line, start, i, i, n);
	else
		*n = i;
}
