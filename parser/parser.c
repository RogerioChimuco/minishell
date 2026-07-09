/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 19:05:04 by rochimuc          #+#    #+#             */
/*   Updated: 2026/07/09 19:05:13 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

static t_command	*ft_set_cmd(t_list **lst);
static t_command	*ft_extract_cmd(t_list **lst, int i);
static int			ft_process_token(t_list **lst, t_command *cmd, int *i);

t_list	*ft_parser(t_list *lst)
{
	int			index;
	int			i;
	t_list		*lst_commands;
	t_list		*node;
	t_command	*cmd;

	index = -1;
	lst_commands = NULL;
	while (lst)
	{
		i = -1;
		cmd = ft_extract_cmd(&lst, i);
		if (!cmd)
			return (ft_lstclear(&lst_commands, command_clear), NULL);
		cmd->index = ++index;
		node = ft_lstnew(cmd);
		if (!node)
		{
			ft_lstclear(&lst_commands, command_clear);
			return (command_clear(cmd), NULL);
		}
		ft_lstadd_back(&lst_commands, node);
	}
	return (lst_commands);
}

static t_command	*ft_set_cmd(t_list **lst)
{
	size_t		length;
	t_command	*cmd;

	if (!lst || !*lst)
		return (NULL);
	cmd = (t_command *)malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->path = NULL;
	cmd->redir = NULL;
	ft_word_count(*lst, &length);
	cmd->args = (char **)ft_calloc(length + 1, sizeof(char *));
	if (!cmd->args)
		return (free(cmd), NULL);
	cmd->is_builtin = 0;
	cmd->index = 0;
	return (cmd);
}

static int	ft_process_token(t_list **lst, t_command *cmd, int *i)
{
	t_token	*token;
	t_redir	*redir;
	t_list	*node;

	token = (t_token *)(*lst)->content;
	if (token->type == PIPE)
		return (((*lst) = (*lst)->next), 0);
	else if (token->type == WORD && (token->value[0] != 0
			|| token->quote_state != NO_QUOTES))
		cmd->args[++(*i)] = ft_strdup(token->value);
	else if (token->type != PIPE && token->type != WORD)
	{
		redir = ft_redirnew(*lst);
		if (!redir)
			return (command_clear(cmd), -1);
		node = ft_lstnew(redir);
		if (!node)
			return (ft_redirclear(redir), command_clear(cmd), -1);
		ft_lstadd_back(&cmd->redir, node);
		(*lst) = (*lst)->next;
	}
	(*lst) = (*lst)->next;
	return (1);
}

static t_command	*ft_extract_cmd(t_list **lst, int i)
{
	t_command	*cmd;
	int			ret;

	cmd = ft_set_cmd(lst);
	if (!cmd)
		return (NULL);
	while (*lst)
	{
		ret = ft_process_token(lst, cmd, &i);
		if (ret == -1)
			return (NULL);
		if (ret == 0)
			break ;
	}
	return (cmd);
}
