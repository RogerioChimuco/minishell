/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_validate_quotes.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:39:06 by ckulembe          #+#    #+#             */
/*   Updated: 2026/03/16 00:26:36 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_message_error(int error, const char *message)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd((char *)message, 2);
	ft_putstr_fd("\n", 2);
	return (error);
}

int	ft_validate_quotes(const char *line)
{
	int	in_single;
	int	in_double;
	int	i;

	if (!line || *line == '\0')
		return (0);
	in_single = 0;
	in_double = 0;
	i = 0;
	while (line[i])
	{
		if (line[i] == SINGLE_QUOTE && !in_double)
			in_single = !in_single;
		else if (line[i] == DOUBLE_QUOTE && !in_single)
			in_double = !in_double;
		i++;
	}
	if (in_single)
		return (ft_message_error(2, "syntax error: unclosed single quote"));
	if (in_double)
		return (ft_message_error(3, "syntax error: unclosed double quote"));
	return (0);
}
