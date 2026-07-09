/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 13:00:50 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 13:00:52 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "redir.h"

static int	resolve_file_redir(t_redir *redir, int *fd, int *target_fd)
{
	if (check_ambiguous_redirect(redir) == -1)
		return (-1);
	if (redir->type == REDIR_IN)
	{
		*target_fd = STDIN_FILENO;
		return (open_file_redir(fd, redir->value, open_input_redir));
	}
	if (redir->type == REDIR_OUT)
	{
		*target_fd = STDOUT_FILENO;
		return (open_file_redir(fd, redir->value, open_output_redir));
	}
	if (redir->type == REDIR_APPEND)
	{
		*target_fd = STDOUT_FILENO;
		return (open_file_redir(fd, redir->value, open_append_redir));
	}
	*target_fd = STDIN_FILENO;
	*fd = redir->fd;
	if (*fd == -1)
		return (-1);
	return (0);
}

int	open_heredoc_redir_fd(int *fd, char *value,
		int (*f)(char *, t_shell *), t_shell *shell)
{
	*fd = f(value, shell);
	return (*fd);
}

void	print_redir_error(char *token, int is_ambiguous)
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
}

static int	apply_single_redir(t_redir *redir, int *fd, int *target_fd)
{
	if (resolve_file_redir(redir, fd, target_fd) == -1)
	{
		print_redir_error(get_redir_display_value(redir), redir->amb);
		return (-1);
	}
	if (*fd != -1 && dup2(*fd, *target_fd) == -1)
	{
		close(*fd);
		print_redir_error(get_redir_display_value(redir), redir->amb);
		return (-1);
	}
	if (*fd != -1)
		close(*fd);
	return (0);
}

int	apply_redirections(t_list *lst)
{
	int		fd;
	int		target_fd;
	t_redir	*redir;

	fd = -1;
	target_fd = -1;
	while (lst)
	{
		redir = (t_redir *)lst->content;
		if (apply_single_redir(redir, &fd, &target_fd) == -1)
			return (-1);
		lst = lst->next;
	}
	return (0);
}
