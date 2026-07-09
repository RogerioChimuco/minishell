/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:53:42 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:53:50 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

int	ft_is_space(char c)
{
	return (c == 32 || c == 9);
}

int	ft_is_operator(const char *line, int i)
{
	return (line[i] == 124 || line[i] == 60 || line[i] == 62);
}

int	ft_is_quote(char c)
{
	return (c == 34 || c == 39);
}

t_data	build_word_data(int start, int *i, int *n)
{
	t_data	data;

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
	free(token->value);
	free(token->original_value);
	free(token);
}
