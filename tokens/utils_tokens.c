/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:41:12 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/08 16:41:47 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

int	ft_is_space(char c)
{
	if (c == 32 || c == 9)
		return (1);
	return (0);
}

int	ft_is_operator(const char *line, int i)
{
	if (line[i] == 124)
		return (1);
	else if (line[i] == 60)
		return (1);
	else if (line[i] == 62)
		return (1);
	return (0);
}

int	ft_is_quote(char c)
{
	if (c == 34 || c == 39)
		return (1);
	return (0);
}

t_data	ft_data(int start, int *i, int *n)
{
	t_data	data;

	if (!i || !n)
	{
		data.i = 0;
		data.n = 0;
	}
	data.i = i;
	data.n = n;
	data.start = start;
	return (data);
}

void	token_clear(void *arg)
{
	t_token	*token;

	if (!arg)
		return ;
	token = (t_token *)arg;
	if (token->value)
	{
		free(token->value);
		token->value = NULL;
	}
	if (token->original_value)
	{
		free(token->original_value);
		token->original_value = NULL;
	}
	free(token);
	return ;
}
