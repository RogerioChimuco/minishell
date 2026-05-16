/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ambiguous_redirect.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 16:33:50 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/09 12:37:16 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "redir.h"

static int	count(char const *s, char c)
{
	int	word;
	int	i;

	if (!s || *s == '\0')
		return (0);
	word = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] != c && (i == 0 || s[i - 1] == c))
			word++;
		i++;
	}
	return (word);
}

char	*valible_name(t_redir *redir)
{
	char	*value;

	value = redir->value;
	if (redir->amb == 1)
		value = redir->original_value;
	return (value);
}

int	ambiguous_redirect(t_redir *redir)
{
	if (redir->type == REDIR_IN || redir->type == REDIR_OUT
		|| redir->type == REDIR_APPEND)
	{
		if (count(redir->value, ' ') != 1)
		{
			redir->amb = 1;
			return (-1);
		}
	}
	return (0);
}
