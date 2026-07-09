/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vars.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:55:43 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:55:46 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

static void	list_push(t_env **head, t_env **tail, t_env *node);
static int	push_var(char *str, t_env **head, t_env **tail);

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
		return (free(env), ft_free((void **)&key, (void **)&value), NULL);
	env->key = key;
	env->value = value;
	env->exported = 1;
	env->next = NULL;
	return (env);
}

static void	list_push(t_env **head, t_env **tail, t_env *node)
{
	if (!*head)
		*head = node;
	else
		(*tail)->next = node;
	*tail = node;
}

static int	push_var(char *str, t_env **head, t_env **tail)
{
	t_env	*node;

	node = env_add(str);
	if (!node)
		return (env_delete(head), 0);
	list_push(head, tail, node);
	return (1);
}

int	push_computed_var(char *value, t_env **head, t_env **tail)
{
	int	ok;

	if (!value)
		return (1);
	ok = push_var(value, head, tail);
	free(value);
	return (ok);
}

t_env	*env_get(char **env, char *home)
{
	t_env	*head;
	t_env	*tail;
	int		i;
	int		(*extra[2])(t_env **, t_env **);

	(void)home;
	if (!env || !*env)
		return (NULL);
	head = NULL;
	tail = NULL;
	while (*env)
	{
		if (!push_var(*env, &head, &tail))
			return (NULL);
		env++;
	}
	extra[0] = push_home_path;
	extra[1] = push_path_home;
	i = -1;
	while (++i < 2)
		if (!extra[i](&head, &tail))
			return (NULL);
	return (head);
}
