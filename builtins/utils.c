/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:28:03 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:52:25 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int	ft_matrislen(char **argv)
{
	int	i;

	if (!argv)
		return (0);
	i = 0;
	while (argv[i])
		i++;
	return (i);
}

int	built_error(char *cmd, char *arg, char *msg)
{
	int	err_code;

	err_code = errno;
	if (!err_code)
		err_code = 2;
	ft_putstr_fd("minishell: ", 2);
	if (cmd)
	{
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": ", 2);
	}
	if (arg)
	{
		ft_putstr_fd(arg, 2);
		ft_putstr_fd(": ", 2);
	}
	if (msg)
		ft_putstr_fd(msg, 2);
	ft_putchar_fd('\n', 2);
	return (err_code);
}

void	matriz_delete(char **matriz)
{
	int	i;

	if (!matriz)
		return ;
	i = -1;
	while (matriz[++i])
	{
		ft_free((void *)&matriz[i], 0);
		matriz[i] = NULL;
	}
	free(matriz);
}

char	*ft_getenv(t_env *env, char *name)
{
	t_env	*node;

	if (!env || !name)
		return (NULL);
	node = env_find(env, name);
	if (!node)
		return (NULL);
	return (ft_strdup(node->value));
}

int	var_name_validate(char *var)
{
	int	i;

	if (!ft_isalpha(var[0]) && var[0] != '_')
		return (0);
	i = 0;
	while (var[++i])
	{
		if (ft_isalnum(var[i]) || var[i] == '_'
			|| (var[i] == '+' && !var[i + 1]))
			continue ;
		return (0);
	}
	return (1);
}
