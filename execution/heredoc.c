/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:59:18 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:59:21 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	assign_heredoc_fd(t_redir *redir, t_shell *shell)
{
	return (open_heredoc_redir_fd(&redir->fd, redir->value,
			open_heredoc_redir, shell));
}

static int	process_single_redir(t_redir *redir, t_shell *shell)
{
	if (redir->type != HEREDOC)
		return (0);
	return (assign_heredoc_fd(redir, shell));
}

static int	process_cmd_heredocs(t_list *lst, t_shell *shell)
{
	t_redir	*redir;

	while (lst)
	{
		redir = (t_redir *)lst->content;
		if (process_single_redir(redir, shell) == -1)
			return (-1);
		lst = lst->next;
	}
	return (0);
}

static int	report_heredoc_error(t_shell *shell)
{
	if (shell->exit_status == 130)
		return (-1);
	return (print_redir_error(NULL, 0), -1);
}

int	process_all_heredocs(t_list *lst, t_shell *shell)
{
	t_command	*cmd;

	while (lst)
	{
		cmd = (t_command *)lst->content;
		if (cmd->redir && process_cmd_heredocs(cmd->redir, shell) == -1)
			return (report_heredoc_error(shell));
		lst = lst->next;
	}
	return (0);
}
