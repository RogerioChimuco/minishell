/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 18:48:02 by rochimuc          #+#    #+#             */
/*   Updated: 2026/07/09 18:48:04 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expansion.h"

void	ft_expasion(t_list **tokens, int *error, t_env *vars)
{
	t_token			*token;
	t_token_type	prev;
	t_list			*lst_token;

	if (!*tokens)
		return ;
	lst_token = *tokens;
	prev = 0;
	while (lst_token)
	{
		token = (t_token *)lst_token->content;
		if (prev != HEREDOC)
		{
			ft_expand(token, error, vars);
			ft_remove_quotes(token->value);
		}
		prev = token->type;
		lst_token = lst_token->next;
	}
}

char	*heredoc_expand(char *line, t_shell *shell)
{
	int	i;

	i = -1;
	while (line[++i])
	{
		if (line[i] == '$')
		{
			if (line[i + 1] == '?')
				line = get_error(line, &shell->exit_status, &i);
			line = ft_token_expend(line, shell->vars, get_var_name(line + i),
					&i);
			if (!line)
				return (NULL);
		}
	}
	return (line);
}
