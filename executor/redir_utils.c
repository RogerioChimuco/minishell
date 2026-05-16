/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:46:39 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/08 16:51:15 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "redir.h"

static int	resolve_redir(t_redir *redir, int *fd, int *target_fd)
{
	if (ambiguous_redirect(redir) == -1)
		return (-1);
	if (redir->type == REDIR_IN)
	{
		*target_fd = STDIN_FILENO;
		return (open_redir_fd(fd, redir->value, redir_in));
	}
	else if (redir->type == REDIR_OUT)
	{
		*target_fd = STDOUT_FILENO;
		return (open_redir_fd(fd, redir->value, redir_out));
	}
	else if (redir->type == REDIR_APPEND)
	{
		*target_fd = STDOUT_FILENO;
		return (open_redir_fd(fd, redir->value, redir_append));
	}
	else
	{
		*target_fd = STDIN_FILENO;
		*fd = redir->fd;
		if (*fd == -1)
			return (-1);
		return (0);
	}
}

int	open_redir_heredoc_fd(int *fd, char *value,
		int (*f)(char *, t_shell *), t_shell *shell)
{
	*fd = f(value, shell);
	return (*fd);
}

void	message_error_redir(char *token, int is_ambiguous)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (token)
	{
		ft_putstr_fd(token, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	if (is_ambiguous)
	{
		ft_putendl_fd("ambiguous redirect", STDERR_FILENO);
		return ;
	}
	ft_putendl_fd(strerror(errno), STDERR_FILENO);
	return ;
}

static void	init_fd(int *fd, int *target_fd)
{
	*fd = -1;
	*target_fd = -1;
}

int	apply_redir(t_list *lst)
{
	int		fd;
	int		target_fd;
	t_redir	*redir;

	init_fd(&fd, &target_fd);
	while (lst)
	{
		redir = (t_redir *)lst->content;
		if (resolve_redir(redir, &fd, &target_fd) == -1)
			return (message_error_redir(valible_name(redir), redir->amb), -1);
		if (fd != -1 && dup2(fd, target_fd) == -1)
		{
			close(fd);
			return (message_error_redir(valible_name(redir), redir->amb), -1);
		}
		if (fd != -1)
			close(fd);
		lst = lst->next;
	}
	return (0);
}
