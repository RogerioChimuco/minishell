/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 15:56:36 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/08 09:39:38 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../built_in.h"

static int	var_check(t_env *env, char *var);
static	int	validate(int *out, char *key, char *value);

int	ft_export(t_env **vars, char **av)
{
	int		i;
	int		out;
	char	*key;
	char	*value;

	i = 0;
	out = 0;
	if (ft_matrislen(av) > 1)
	{
		while (av[++i])
		{
			if (var_check(*vars, av[i]))
				continue ;
			var_get_key_value(av[i], &key, &value);
			if (!key || !value)
				return (1);
			if (!validate(&out, key, value))
				continue ;
			env_update(vars, key, value);
		}
	}
	else
		env_print(*vars, 1, -1);
	return (out);
}

void	env_sort(char **env)
{
	char	*tmp;
	int		i;

	if (!env || !*env)
		return ;
	i = -1;
	while (env[++i + 1])
	{
		if ((ft_strncmp(env[i], env[i + 1], ft_strlen(env[i]))) > 0)
		{
			tmp = env[i + 1];
			env[i + 1] = env[i];
			env[i] = tmp;
			i = -1;
		}
	}
}

void	env_print(t_env *env, int sort, int i)
{
	char	**vars;

	vars = ft_calloc((env_size(env)) + 1, sizeof(char *));
	if (!vars)
		return ;
	while (env)
	{
		vars[++i] = var_add(env);
		if (!vars[i])
			return ((void)matriz_delete(vars));
		env = env->next;
	}
	if (sort)
		env_sort(vars);
	i = -1;
	while (vars[++i])
	{
		if (sort)
			ft_putstr_fd("declare -x ", 1);
		else if (!ft_strchr(vars[i], '='))
			continue ;
		ft_putendl_fd(vars[i], 1);
	}
	matriz_delete(vars);
}

static int	var_check(t_env *env, char *var)
{
	char	*tmp;

	tmp = ft_strchr(var, '=');
	if (tmp)
		return (0);
	tmp = ft_getenv(env, var);
	if (tmp)
		return (free(tmp), 1);
	return (0);
}

static	int	validate(int *out, char *key, char *value)
{
	int	i;

	if (key[0] == '-')
	{
		i = 0;
		ft_putstr_fd("minishell: export: `", 2);
		while (key[i] && i < 2)
			ft_putchar_fd(key[i++], 2);
		ft_putendl_fd("': invalid option", 2);
		if (!*out)
			*out = 2;
		return (ft_free((void *)&key, (void *)&value), 0);
	}
	else if (!var_name_validate(key))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(key, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		if (!*out)
			*out = 1;
		return (ft_free((void *)&key, (void *)&value), 0);
	}
	return (1);
}
