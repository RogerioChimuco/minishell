/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 10:59:38 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:52:00 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static int	error_message(char *arg, char *msg);
static int	check_number(int argc, char **argv, int *status);
static void	free_management(t_shell *shell);
static int	is_signal(char c);

int	ft_exit(int argc, char **argv, t_shell *shell)
{
	int	status;

	if (isatty(0))
		ft_putendl_fd("exit", 1);
	status = 0;
	if (argc == 1)
		status = shell->exit_status;
	else
	{
		if (check_number(argc, argv, &status))
		{
			if (status == 2)
				error_message(argv[1], "numeric argument required");
			else if (status == 1)
				return (error_message(NULL, "too many arguments"), 1);
		}
		else
			status = ft_atoi(argv[1]) % 256;
	}
	rl_clear_history();
	free_management(shell);
	exit(status);
}

static int	is_signal(char c)
{
	return ((c == '-') || (c == '+'));
}

static int	check_number(int argc, char **argv, int *status)
{
	int	i;

	if (!argv[1][0])
		return (*status = 2);
	i = 0;
	if (is_signal(argv[1][i]))
		i++;
	if (!argv[1][i])
		return (*status = 2);
	while (argv[1][i])
	{
		if (!ft_isdigit(argv[1][i]))
			return (*status = 2);
		i++;
	}
	if (argc > 2)
		return (*status = 1);
	return (0);
}

static void	free_management(t_shell *shell)
{
	if (!shell)
		return ;
	free(shell->input);
	free(shell->prompt);
	free(shell->pipes);
	free(shell->pids);
	if (shell->tokens)
		ft_lstclear(&shell->tokens, token_clear);
	if (shell->cmds)
		ft_lstclear(&shell->cmds, command_clear);
	env_delete(&shell->vars);
}

static int	error_message(char *arg, char *msg)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd("exit: ", 2);
	if (arg)
	{
		ft_putstr_fd(arg, 2);
		ft_putstr_fd(": ", 2);
	}
	ft_putendl_fd(msg, 2);
	return (2);
}
