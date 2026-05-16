/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pids.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 17:12:50 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/07 15:53:46 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	create_pids(t_shell *shell)
{
	shell->pids = (pid_t *) malloc(sizeof(pid_t) * shell->length);
	if (!shell->pids)
		return (-1);
	return (0);
}

void	wait_pids(t_shell *shell, int pid_i)
{
	int	status;
	int	iter;

	iter = -1;
	while (++iter < pid_i)
		waitpid(shell->pids[iter], &status, 0);
	if (WIFEXITED(status))
		shell->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		shell->exit_status = 128 + WTERMSIG(status);
		if (shell->exit_status == 131)
			ft_putendl_fd("Quit (core dumped)", 1);
	}
	else
		shell->exit_status = 0;
	free(shell->pids);
	shell->pids = NULL;
	return ;
}
