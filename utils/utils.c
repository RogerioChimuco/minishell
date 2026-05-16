/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:34:43 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/09 12:42:32 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

static void	path_rules(char **path, char *user);

void	var_delete(t_env *env)
{
	if (!env)
		return ;
	if (env->key)
	{
		free(env->key);
		env->key = NULL;
	}
	if (env->value)
	{
		free(env->value);
		env->value = NULL;
	}
	free(env);
	env = NULL;
}

void	env_delete(t_env **var)
{
	t_env	*tmp;
	t_env	*next;

	if (!var || !*var)
		return ;
	tmp = *var;
	while (tmp)
	{
		next = tmp->next;
		var_delete(tmp);
		tmp = next;
	}
	*var = NULL;
}

void	ft_prompt(char **prompt, t_env *env)
{
	char	*path;
	char	*user;
	int		size[2];

	user = ft_getenv(env, "USER");
	if (!user)
		user = ft_strdup("~");
	path = getcwd(NULL, 0);
	if (!path)
		path = ft_strdup("minishel$ ");
	if (*prompt)
		ft_free((void *)&(*prompt), 0);
	size[0] = ft_strlen(user);
	path_rules(&path, user);
	size[1] = ft_strlen(path);
	*prompt = malloc(size[0] + size[1] + 4);
	if (!*prompt)
		return ((ft_free((void *)&path, (void *)&user)),
			(void)(*prompt = ft_strdup("minishel$ ")));
	ft_memcpy(*prompt, user, size[0]);
	ft_memcpy(*prompt + size[0], ":", 1);
	ft_memcpy(*prompt + size[0] + 1, path, size[1]);
	ft_strlcpy(*prompt + (size[0] + size[1] + 1), "$ ", 3);
	ft_free((void *)&user, (void *)&path);
}

static void	path_rules(char **path, char *home)
{
	char	*tmp;
	int		i[2];

	i[0] = 0;
	i[1] = 0;
	while (home[i[0]] && path[0][i[0]] == home[i[0]])
	{
		if (!home[i[0] + 1])
			i[1] = 1;
		i[0]++;
	}
	if (i[1])
	{
		tmp = ft_strjoin("~", *path + i[0]);
		if (!tmp)
			return ;
		free(*path);
		*path = tmp;
	}
}

void	ft_free(void **s1, void **s2)
{
	if (s1 && *s1)
	{
		free(*s1);
		*(s1) = NULL;
	}
	if (s2 && *s2)
	{
		free(*s2);
		*(s2) = NULL;
	}
}
