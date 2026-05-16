/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:46:58 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/09 13:59:47 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	setup_pipes(int index, t_shell *shell)
{
	int	iter;

	if (index > 0)
	{
		if (dup2(shell->pipes[index - 1][0], STDIN_FILENO) == -1)
		{
			perror("dup2");
			exit(1);
		}
	}
	if (index < shell->length - 1)
	{
		if (dup2(shell->pipes[index][1], STDOUT_FILENO) == -1)
		{
			perror("dup2");
			exit(1);
		}
	}
	iter = 0;
	while (iter < shell->length - 1)
	{
		close(shell->pipes[iter][0]);
		close(shell->pipes[iter][1]);
		iter++;
	}
}

void	close_pipes(t_shell *shell, int length)
{
	int	iter;

	if (!shell->pipes)
		return ;
	iter = 0;
	while (iter < length)
	{
		close(shell->pipes[iter][0]);
		close(shell->pipes[iter][1]);
		iter ++;
	}
	free(shell->pipes);
	shell->pipes = NULL;
	return ;
}

int	create_pipes(t_shell *shell)
{
	int	iter;

	shell->pipes = malloc(sizeof(*shell->pipes) * (shell->length - 1));
	if (!shell->pipes)
		return (-1);
	iter = -1;
	while (++iter < shell->length - 1)
	{
		if (pipe(shell->pipes[iter]) != 0)
			return (close_pipes(shell, iter), -1);
	}
	return (0);
}

void	restore_fds(int in, int out)
{
	dup2(in, STDIN_FILENO);
	dup2(out, STDOUT_FILENO);
	close(in);
	close(out);
}

void	is_dir_or_file(const char *path)
{
	struct stat	st;

	if (!ft_strchr(path, '/'))
		return ;
	if (stat(path, &st) < 0)
	{
		message_error_cmd(path, ENOENT);
		exit(127);
	}
	if (S_ISDIR(st.st_mode))
	{
		message_error_cmd(path, EISDIR);
		exit(126);
	}
	if (!(st.st_mode & S_IXUSR))
	{
		message_error_cmd(path, EACCES);
		exit(126);
	}
	return ;
}
