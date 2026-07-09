/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_check_syntax.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 19:04:39 by rochimuc          #+#    #+#             */
/*   Updated: 2026/07/09 19:04:42 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

static void	ft_message_error(const char *message)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd((char *)message, 2);
	ft_putstr_fd("`\n", 2);
}

static char	*ft_operator_error(t_token_type type)
{
	if (type == REDIR_IN)
		return ("<");
	else if (type == REDIR_OUT)
		return (">");
	else if (type == REDIR_APPEND)
		return (">>");
	else if (type == HEREDOC)
		return ("<<");
	else if (type == PIPE)
		return ("|");
	return (NULL);
}

static int	ft_check_pipe(t_token_type type, t_list *next, int *first)
{
	t_token	*token;

	if (!next)
		return (ft_message_error("|"), 0);
	token = (t_token *)next->content;
	if (*first == 0)
		return (ft_message_error("|"), 0);
	else if (token->type == type)
		return (ft_message_error("|"), 0);
	return (1);
}

static int	ft_check_redir(t_list *next)
{
	t_token	*token;

	if (!next)
		return (ft_message_error("newline"), 0);
	token = (t_token *)next->content;
	if (token->type != WORD)
		return (ft_message_error(ft_operator_error(token->type)), 0);
	return (1);
}

int	ft_check_syntax(t_list *lst)
{
	int				first;
	int				res;
	t_token			*token;
	t_token_type	type;

	if (!lst)
		return (0);
	first = 0;
	while (lst)
	{
		token = (t_token *)lst->content;
		type = token->type;
		if (type == PIPE)
		{
			res = ft_check_pipe(type, lst->next, &first);
			if (res != 1)
				return (res);
		}
		if ((type != WORD && type != PIPE) && !ft_check_redir(lst->next))
			return (0);
		if (first == 0)
			first = 1;
		lst = lst->next;
	}
	return (1);
}
