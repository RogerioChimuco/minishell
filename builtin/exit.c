/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 09:18:22 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/07 18:00:25 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static int	error_message(char *arg, char *msg);
static int	check_number(int argc, char **argv, int *out);
static void	free_management(t_shell *shell);

static int	is_signal(char c)
{
	return ((c == '-') || (c == '+'));
}

int	ft_exit(int argc, char **argv, t_shell *shell)
{
	int	out;

	if (isatty(0))
		ft_putendl_fd("exit", 1);
	out = 0;
	if (argc == 1)
		out = shell->exit_status;
	else if (argc > 1)
	{
		if (check_number(argc, argv, &out))
		{
			if (out == 2)
				error_message(argv[1], "numeric argument required");
			else if (out == 1)
				error_message(NULL, "too many arguments");
		}
		else
			out = ft_atoi(argv[1]) % 256;
	}
	rl_clear_history();
	free_management(shell);
	exit(out);
}

static int	check_number(int argc, char **argv, int *out)
{
	int	i;

	if (!argv[1][0])
		return (*out = 2);
	i = 0;
	if (is_signal(argv[1][i]))
		i++;
	while (argv[1][i])
	{
		if (!ft_isdigit(argv[1][i]))
			return (*out = 2);
		i++;
	}
	if (argc > 2)
		return (*out = 1);
	return (0);
}

static void	free_management(t_shell *shell)
{
	if (shell)
	{
		if (shell->input)
			free(shell->input);
		if (shell->prompt)
			free(shell->prompt);
		if (shell->pipes)
			free(shell->pipes);
		if (shell->pids)
			free(shell->pids);
		if (shell->tokens)
			ft_lstclear(&shell->tokens, token_clear);
		if (shell->cmds)
			ft_lstclear(&shell->cmds, command_clear);
		env_delete(&shell->vars);
	}
}

static int	error_message(char *arg, char *msg)
{
	ft_putstr_fd("minishel: ", 2);
	ft_putstr_fd("exit: ", 2);
	if (arg)
	{
		ft_putstr_fd(arg, 2);
		ft_putstr_fd(": ", 2);
	}
	ft_putendl_fd(msg, 2);
	return (2);
}
