/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 11:20:22 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/08 07:48:57 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtins.h"

void	env_update(t_env **env, char *key, char *value)
{
	t_env	*node;

	node = env_find(*env, key);
	if (!node)
	{
		node = env_new(key, value);
		if (!node)
			return (ft_free((void *)&key, (void *)&value));
		node->next = *env;
		*env = node;
		return ;
	}
	ft_free((void *)&node->value, 0);
	node->value = value;
	ft_free((void *)&key, 0);
}

t_env	*env_find(t_env *env, char *key)
{
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
	t_env	*new_node;

	new_node = ft_calloc(1, sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = key;
	new_node->value = value;
	new_node->exported = 1;
	return (new_node);
}

char	*var_add(t_env *env)
{
	char	*entry;
	int		key_len;
	int		value_len;

	if (!env)
		return (NULL);
	key_len = ft_strlen(env->key);
	value_len = ft_strlen(env->value);
	entry = ft_calloc(key_len + value_len + 2, sizeof(char));
	if (!entry)
		return (NULL);
	ft_memcpy(entry, env->key, key_len);
	if (!value_len)
		return (entry);
	ft_memcpy(entry + key_len, "=", 1);
	ft_memcpy(entry + key_len + 1, env->value, value_len);
	return (entry);
}

size_t	env_size(t_env *env)
{
	size_t	count;

	count = 0;
	while (env)
	{
		count++;
		env = env->next;
	}
	return (count);
}
