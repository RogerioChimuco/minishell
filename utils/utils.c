/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:55:11 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:55:17 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

void	ft_free(void **s1, void **s2)
{
	if (s1 && *s1)
	{
		free(*s1);
		*s1 = NULL;
	}
	if (s2 && *s2)
	{
		free(*s2);
		*s2 = NULL;
	}
}

void	var_delete(t_env *env)
{
	if (!env)
		return ;
	ft_free((void **)&env->key, (void **)&env->value);
	free(env);
}

static void	env_delete_iter(t_env *node)
{
	t_env	*next;

	while (node)
	{
		next = node->next;
		var_delete(node);
		node = next;
	}
}

void	env_delete(t_env **var)
{
	if (!var || !*var)
		return ;
	env_delete_iter(*var);
	*var = NULL;
}
