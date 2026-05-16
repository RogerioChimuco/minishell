/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:45:59 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/08 16:52:15 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	resolve_heredoc(t_redir *redir, t_shell *shell)
{
	return (open_redir_heredoc_fd(&redir->fd, redir->value,
			redir_heredoc, shell));
}

static int	foreach_heredoc(t_list *lst, t_shell *shell)
{
	t_redir	*redir;

	while (lst)
	{
		redir = (t_redir *)lst->content;
		if (redir->type == HEREDOC)
		{
			if (resolve_heredoc(redir, shell) == -1)
				return (-1);
		}
		lst = lst->next;
	}
	return (0);
}

int	foreach_cmd(t_list *lst, t_shell *shell)
{
	t_command	*cmd;

	if (!lst)
		return (0);
	while (lst)
	{
		cmd = (t_command *)lst->content;
		if (!cmd->redir)
		{
			lst = lst->next;
			continue ;
		}
		if (foreach_heredoc(cmd->redir, shell) == -1)
		{
			if (shell->exit_status == 130)
				return (-1);
			return (message_error_redir(NULL, 0), -1);
		}
		lst = lst->next;
	}
	return (0);
}
