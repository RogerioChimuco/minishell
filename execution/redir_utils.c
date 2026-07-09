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

typedef struct s_redir_map
{
	t_token_type	type;
	int				target_fd;
	int				(*open_fn)(char *);
}	t_redir_map;

static int	resolve_heredoc_target(t_redir *redir, int *fd, int *target_fd)
{
	*target_fd = STDIN_FILENO;
	*fd = redir->fd;
	if (*fd == -1)
		return (-1);
	return (0);
}

static int	resolve_file_redir(t_redir *redir, int *fd, int *target_fd)
{
	static const t_redir_map	map[3] = {
		{REDIR_IN, STDIN_FILENO, open_input_redir},
		{REDIR_OUT, STDOUT_FILENO, open_output_redir},
		{REDIR_APPEND, STDOUT_FILENO, open_append_redir}
	};
	int							i;

	if (check_ambiguous_redirect(redir) == -1)
		return (-1);
	i = -1;
	while (++i < 3)
	{
		if (redir->type == map[i].type)
		{
			*target_fd = map[i].target_fd;
			return (open_file_redir(fd, redir->value, map[i].open_fn));
		}
	}
	return (resolve_heredoc_target(redir, fd, target_fd));
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

static void	init_fd_targets(int *fd, int *target_fd)
{
	*fd = -1;
	*target_fd = -1;
}

static int	apply_single_redir(t_redir *redir, int *fd, int *target_fd)
{
	if (resolve_file_redir(redir, fd, target_fd) == -1)
		return (print_redir_error(get_redir_display_value(redir), redir->amb), -1);
	if (*fd != -1 && dup2(*fd, *target_fd) == -1)
	{
		close(*fd);
		return (print_redir_error(get_redir_display_value(redir), redir->amb), -1);
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

	init_fd_targets(&fd, &target_fd);
	while (lst)
	{
		redir = (t_redir *)lst->content;
		if (apply_single_redir(redir, &fd, &target_fd) == -1)
			return (-1);
		lst = lst->next;
	}
	return (0);
}
