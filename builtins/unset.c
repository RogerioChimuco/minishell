/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 13:04:15 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 14:46:37 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static void	env_unset(t_env **env, char *key);
static int	unset_error(char *arg);

int	ft_unset(t_env **env, char **argv)
{
	int	idx;

	if (!env || !argv || !*argv)
		return (1);
	idx = 0;
	while (argv[++idx])
	{
		if (idx == 1 && argv[idx][0] == '-')
			return (unset_error(argv[idx]));
		env_unset(env, argv[idx]);
	}
	return (0);
}

static void	env_unset(t_env **env, char *key)
{
	t_env	*node;
	t_env	*prev;

	if (!env || !*env)
		return ;
	node = *env;
	prev = NULL;
	while (node && ft_strcmp(node->key, key))
	{
		prev = node;
		node = node->next;
	}
	if (!node)
		return ;
	if (prev)
		prev->next = node->next;
	else
		*env = node->next;
	var_delete(node);
}

static int	unset_error(char *arg)
{
	int	idx;

	ft_putstr_fd("minishell: unset: ", 2);
	idx = -1;
	while (arg[++idx] && idx < 2)
		ft_putchar_fd(arg[idx], 2);
	ft_putendl_fd(": invalid option", 2);
	return (2);
}
