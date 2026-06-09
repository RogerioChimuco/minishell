/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:59:43 by luqalmei          #+#    #+#             */
/*   Updated: 2026/06/09 14:59:50 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/*
** env_entry_new — allocate one t_env node from a "KEY=value" string.
** Both key and val are heap copies so callers can free the original.
*/
static t_env	*env_entry_new(const char *raw)
{
	t_env	*e;
	char	*eq;

	e = ft_calloc(1, sizeof(t_env));
	if (!e)
		return (NULL);
	eq = ft_strchr(raw, '=');
	if (eq)
	{
		e->key = ft_substr(raw, 0, (size_t)(eq - raw));
		e->val = ft_strdup(eq + 1);
	}
	else
	{
		e->key = ft_strdup(raw);
		e->val = ft_strdup("");
	}
	if (!e->key || !e->val)
	{
		free(e->key);
		free(e->val);
		free(e);
		return (NULL);
	}
	e->exported = 1;
	e->next = NULL;
	return (e);
}

/*
** env_build — parse the envp array passed by the OS into a t_env list.
** Appends a synthetic SHLVL entry if absent (for compatibility).
*/
t_env	*env_build(char **envp)
{
	t_env	*head;
	t_env	*tail;
	t_env	*node;
	int		i;

	head = NULL;
	tail = NULL;
	i = -1;
	if (!envp)
		return (NULL);
	while (envp[++i])
	{
		node = env_entry_new(envp[i]);
		if (!node)
		{
			env_free(&head);
			return (NULL);
		}
		if (!head)
			head = node;
		else
			tail->next = node;
		tail = node;
	}
	return (head);
}

/*
** env_get — linear search by key; returns the value string or NULL.
** The returned pointer is owned by the list — do not free it.
*/
char	*env_get(t_env *env, const char *key)
{
	while (env)
	{
		if (!ft_strcmp(env->key, key))
			return (env->val);
		env = env->next;
	}
	return (NULL);
}

/*
** env_set — update an existing key or append a new node.
** Takes ownership of val (val is freed if the node is later freed).
*/
void	env_set(t_env **env, const char *key, char *val)
{
	t_env	*cur;
	t_env	*node;

	cur = *env;
	while (cur)
	{
		if (!ft_strcmp(cur->key, key))
		{
			free(cur->val);
			cur->val = val;
			cur->exported = 1;
			return ;
		}
		if (!cur->next)
			break ;
		cur = cur->next;
	}
	node = ft_calloc(1, sizeof(t_env));
	if (!node)
		return ;
	node->key = ft_strdup(key);
	node->val = val;
	node->exported = 1;
	if (cur)
		cur->next = node;
	else
		*env = node;
}

/*
** env_free — free every node in the list and NULL the head pointer.
*/
void	env_free(t_env **env)
{
	t_env	*cur;
	t_env	*nxt;

	if (!env || !*env)
		return ;
	cur = *env;
	while (cur)
	{
		nxt = cur->next;
		free(cur->key);
		free(cur->val);
		free(cur);
		cur = nxt;
	}
	*env = NULL;
}

/*
** env_to_array — convert the linked list to a NULL-terminated char** array
** in "KEY=value" format, suitable for passing to execve(2).
*/
char	**env_to_array(t_env *env)
{
	t_env	*cur;
	char	**arr;
	char	*tmp;
	int		n;
	int		i;

	n = 0;
	cur = env;
	while (cur)
	{
		if (cur->exported)
			n++;
		cur = cur->next;
	}
	arr = ft_calloc(n + 1, sizeof(char *));
	if (!arr)
		return (NULL);
	i = 0;
	cur = env;
	while (cur)
	{
		if (cur->exported)
		{
			tmp = ft_strjoin(cur->key, "=");
			arr[i] = ft_strjoin(tmp, cur->val);
			free(tmp);
			if (!arr[i])
				return (free_matrix(arr), NULL);
			i++;
		}
		cur = cur->next;
	}
	arr[i] = NULL;
	return (arr);
}
