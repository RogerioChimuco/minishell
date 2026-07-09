/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pids.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:59:31 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:59:33 by luqalmei         ###   ########.fr       */
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

static void	set_exit_status(t_shell *shell, int status)
{
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
}

void	wait_pids(t_shell *shell, int pid_i)
{
	int	status;
	int	iter;

	iter = -1;
	while (++iter < pid_i)
		waitpid(shell->pids[iter], &status, 0);
	set_exit_status(shell, status);
	free(shell->pids);
	shell->pids = NULL;
}
