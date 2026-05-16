/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:32:27 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/07 18:02:01 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

int	ft_matrislen(char **argv)
{
	int	i;

	if (!argv || !*argv)
		return (0);
	i = 0;
	while (argv[i])
		i++;
	return (i);
}

int	built_error(char *cmd, char *arg, char *msg)
{
	int	erro;

	erro = errno;
	if (!erro)
		erro = 2;
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
	return (erro);
}

void	matriz_delete(char **matriz)
{
	int	i;

	if (!matriz || !*matriz)
		return ;
	i = -1;
	while (matriz[++i])
	{
		ft_free((void *)&matriz[i], 0);
		matriz[i] = NULL;
	}
	free(matriz);
	matriz = NULL;
}

char	*ft_getenv(t_env *env, char *name)
{
	t_env	*tmp;
	char	*var;
	char	*dir;

	if (!env || !name)
		return (NULL);
	tmp = env_find(env, name);
	if (!tmp)
		return (NULL);
	var = var_add(tmp);
	if (!var)
		return (NULL);
	dir = ft_strchr(var, '=');
	if (dir)
		dir = ft_strdup(dir + 1);
	ft_free((void *)&var, 0);
	return (dir);
}

int	var_name_validate(char *var)
{
	int	i;

	i = -1;
	while (var[++i])
	{
		if (ft_isalnum(var[i]) || var[i] == '_'
			|| (var[i] == '+' && !var[i + 1]))
			continue ;
		return (0);
	}
	return (ft_isalpha(var[0]) || var[0] == '_');
}
