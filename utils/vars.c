/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vars.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 09:18:17 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/09 18:30:56 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

static void	var_append(t_env **tmp, t_env **var, t_env **tail);
static void	append(char *var, t_env	**env, t_env **tail, t_env **tmp);
static void	append_path_home(t_env **env, t_env **tail, t_env **tmp);

t_env	*env_get(char **env, char *home)
{
	t_env	*var;
	t_env	*tmp;
	t_env	*tail;
	int		i;

	if (!env || !*env)
		return (NULL);
	i = -1;
	var = NULL;
	tail = NULL;
	tmp = NULL;
	while (env[++i])
		append(env[i], &var, &tail, &tmp);
	home = home_path(var);
	if (home)
	{
		append(home, &var, &tail, &tmp);
		free(home);
	}
	append(NULL, &var, &tail, &tmp);
	return (var);
}

static void	var_append(t_env **tmp, t_env **var, t_env **tail)
{
	if (!*var)
	{
		*var = *tmp;
		*tail = *tmp;
	}
	else
	{
		(*tail)->next = *tmp;
		*tail = *tmp;
	}
}

t_env	*env_add(char *var)
{
	t_env	*env;
	char	*key;
	char	*value;

	if (!var)
		return (NULL);
	env = malloc(sizeof(t_env));
	var_get_key_value(var, &key, &value);
	if (!env || !key || !value)
	{
		free(env);
		ft_free((void *)&key, (void *)&value);
		return (NULL);
	}
	env->key = key;
	env->value = value;
	env->exported = 1;
	env->next = NULL;
	return (env);
}

static void	append(char *var, t_env	**env, t_env **tail, t_env	**tmp)
{
	if (var)
	{
		*tmp = env_add(var);
		if (!*tmp)
			return ((env_delete(env)));
		var_append(tmp, env, tail);
	}
	else
		append_path_home(env, tail, tmp);
}

static void	append_path_home(t_env **env, t_env **tail, t_env **tmp)
{
	char	*path;
	char	*value;

	path = ft_getenv(*env, "HOME");
	if (!path)
		return ;
	value = ft_strjoin("PATH_HOME=", path);
	if (!value)
		return (free(path));
	*tmp = env_add(value);
	if (!*tmp)
	{
		ft_free((void *)&path, (void *)&value);
		return (env_delete(env));
	}
	var_append(tmp, env, tail);
	ft_free((void *)&path, (void *)&value);
}
