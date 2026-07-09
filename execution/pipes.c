/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:59:39 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:59:42 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	safe_dup2(int fd, int std_fd)
{
	if (dup2(fd, std_fd) == -1)
	{
		perror("dup2");
		exit(1);
	}
}

static void	close_all_pipes(t_shell *shell, int length)
{
	int	iter;

	iter = 0;
	while (iter < length)
	{
		close(shell->pipes[iter][0]);
		close(shell->pipes[iter][1]);
		iter++;
	}
}

void	setup_pipes(int index, t_shell *shell)
{
	if (index > 0)
		safe_dup2(shell->pipes[index - 1][0], STDIN_FILENO);
	if (index < shell->length - 1)
		safe_dup2(shell->pipes[index][1], STDOUT_FILENO);
	close_all_pipes(shell, shell->length - 1);
}

void	close_pipes(t_shell *shell, int length)
{
	if (!shell->pipes)
		return ;
	close_all_pipes(shell, length);
	free(shell->pipes);
	shell->pipes = NULL;
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
