/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ambiguous_redirect.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:58:00 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:58:05 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "redir.h"

static int	count_words(char const *s, char c)
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

char	*get_redir_display_value(t_redir *redir)
{
	char	*value;

	value = redir->value;
	if (redir->amb == 1)
		value = redir->original_value;
	return (value);
}

int	check_ambiguous_redirect(t_redir *redir)
{
	int	is_redir;

	is_redir = (redir->type == REDIR_IN || redir->type == REDIR_OUT
			|| redir->type == REDIR_APPEND);
	redir->amb = (is_redir && count_words(redir->value, ' ') != 1);
	return (-redir->amb);
}
