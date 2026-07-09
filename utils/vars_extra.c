/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vars_extra.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:30:00 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 15:30:00 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

int	push_home_path(t_env **head, t_env **tail)
{
	return (push_computed_var(home_path(*head), head, tail));
}

int	push_path_home(t_env **head, t_env **tail)
{
	char	*home;
	int		ok;

	home = ft_getenv(*head, "HOME");
	if (!home)
		return (1);
	ok = push_computed_var(ft_strjoin("PATH_HOME=", home), head, tail);
	free(home);
	return (ok);
}

static void	equal_split(char *var, char *tmp, char **key, char **value)
{
	int	k_size;

	k_size = (tmp - var) + 1;
	*key = malloc(k_size);
	if (*key)
		ft_strlcpy(*key, var, k_size);
	*value = ft_strdup(tmp + 1);
}

void	var_get_key_value(char *var, char **key, char **value)
{
	char	*tmp;

	tmp = ft_strchr(var, '=');
	if (!tmp)
	{
		*key = ft_strdup(var);
		*value = ft_strdup("");
	}
	else
		equal_split(var, tmp, key, value);
	if (!*key || !*value)
	{
		free(*key);
		free(*value);
		*key = NULL;
		*value = NULL;
	}
}
