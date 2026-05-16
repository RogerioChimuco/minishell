/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 14:43:41 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/09 18:30:56 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../built_in.h"

void	env_update(t_env **env, char *key, char *value)
{
	t_env	*update;

	update = env_find(*env, key);
	if (update)
	{
		ft_free((void *)&update->value, 0);
		update->value = value;
		ft_free((void *)&key, 0);
	}
	else
	{
		update = env_new(key, value);
		if (!update)
			return (ft_free((void *)&key, (void *)&value));
		update->next = *env;
		*env = update;
	}
}

t_env	*env_find(t_env *env, char *key)
{
	if (!env)
		return (NULL);
	while (env)
	{
		if (!ft_strcmp(env->key, key))
			return (env);
		env = env->next;
	}
	return (NULL);
}

t_env	*env_new(char *key, char *value)
{
	t_env	*tmp;

	tmp = malloc(sizeof(t_env));
	if (!tmp)
		return (NULL);
	tmp->key = key;
	tmp->value = value;
	tmp->exported = 1;
	tmp->next = NULL;
	return (tmp);
}

char	*var_add(t_env *env)
{
	char	*var;
	int		key;
	int		value;

	if (!env)
		return (NULL);
	key = ft_strlen(env->key);
	value = ft_strlen(env->value);
	var = ft_calloc(key + value + 2, sizeof(char));
	if (!var)
		return (NULL);
	ft_memcpy(var, env->key, key);
	if (value)
	{
		ft_memcpy(var + key, "=", 1);
		ft_memcpy(var + (key + 1), env->value, value);
	}
	return (var);
}

size_t	env_size(t_env *env)
{
	size_t	size;

	size = 0;
	while (env)
	{
		size++;
		env = env->next;
	}
	return (size);
}
