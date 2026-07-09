/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 12:48:30 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/08 07:48:49 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtins.h"

static int	validate_key(int *exit_code, char *key, char *value);
static void	process_export_arg(t_env **vars, char *arg, int *exit_code);

int	ft_export(t_env **vars, char **argv)
{
	int	idx;
	int	exit_code;

	idx = 0;
	exit_code = 0;
	if (ft_matrislen(argv) > 1)
	{
		while (argv[++idx])
			process_export_arg(vars, argv[idx], &exit_code);
	}
	else
		env_print(*vars, 1, -1);
	return (exit_code);
}

static void	process_export_arg(t_env **vars, char *arg, int *exit_code)
{
	char	*key;
	char	*value;
	char	*existing_value;

	if (!ft_strchr(arg, '='))
	{
		existing_value = ft_getenv(*vars, arg);
		if (existing_value)
			return (free(existing_value));
	}
	var_get_key_value(arg, &key, &value);
	if (!key || !value)
	{
		*exit_code = 1;
		return ;
	}
	if (!validate_key(exit_code, key, value))
		return ;
	env_update(vars, key, value);
}

static int	validate_key(int *exit_code, char *key, char *value)
{
	int	idx;

	if (key[0] == '-')
	{
		idx = 0;
		ft_putstr_fd("minishell: export: `", 2);
		while (key[idx] && idx < 2)
			ft_putchar_fd(key[idx++], 2);
		ft_putendl_fd("': invalid option", 2);
		if (!*exit_code)
			*exit_code = 2;
		return (ft_free((void *)&key, (void *)&value), 0);
	}
	else if (!var_name_validate(key))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(key, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		if (!*exit_code)
			*exit_code = 1;
		return (ft_free((void *)&key, (void *)&value), 0);
	}
	return (1);
}

void	env_sort(char **env)
{
	char	*swap;
	int		idx;

	if (!env || !*env)
		return ;
	idx = -1;
	while (env[++idx + 1])
	{
		if ((ft_strncmp(env[idx], env[idx + 1], ft_strlen(env[idx]))) > 0)
		{
			swap = env[idx + 1];
			env[idx + 1] = env[idx];
			env[idx] = swap;
			idx = -1;
		}
	}
}

void	env_print(t_env *env, int sort, int idx)
{
	char	**entries;

	entries = ft_calloc((env_size(env)) + 1, sizeof(char *));
	if (!entries)
		return ;
	while (env)
	{
		entries[++idx] = var_add(env);
		if (!entries[idx])
			return ((void)matriz_delete(entries));
		env = env->next;
	}
	if (sort)
		env_sort(entries);
	idx = -1;
	while (entries[++idx])
	{
		if (sort)
			ft_putstr_fd("declare -x ", 1);
		else if (!ft_strchr(entries[idx], '='))
			continue ;
		ft_putendl_fd(entries[idx], 1);
	}
	matriz_delete(entries);
}
