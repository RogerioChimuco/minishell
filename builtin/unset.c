/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 10:54:46 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/02 12:43:01 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static void	env_unset(t_env **env, char *key);
static int	error(char *cmd);

int	ft_unset(t_env **env, char **argv)
{
	int	i;

	if (!env || !*env || !argv || !*argv)
		return (1);
	i = 0;
	while (argv[++i])
	{
		if (i == 1 && argv[i][0] == '-')
			return (error(argv[i]));
		env_unset(env, argv[i]);
	}
	return (0);
}

static void	env_unset(t_env **env, char *key)
{
	t_env	*tmp;
	t_env	*unset;

	tmp = *env;
	unset = NULL;
	if (!ft_strncmp(tmp->key, key, ft_strlen(tmp->key)))
	{
		unset = tmp;
		*env = unset->next;
	}
	else
	{
		while (tmp->next)
		{
			if (!ft_strcmp(tmp->next->key, key))
			{
				unset = tmp->next;
				tmp->next = unset->next;
				break ;
			}
			tmp = tmp->next;
		}
	}
	var_delete(unset);
}

static int	error(char *cmd)
{
	int	i;

	ft_putstr_fd("minishell: unset: ", 2);
	i = -1;
	while (cmd[++i] && i < 2)
		ft_putchar_fd(cmd[i], 2);
	ft_putendl_fd(": invalid option", 2);
	return (2);
}
