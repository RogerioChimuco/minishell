/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   historic.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 10:26:56 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/09 18:30:56 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

void	ft_historic(void *line)
{
	if (ft_isprint(((char *)line)[0]))
		add_history(line);
}

void	envp_update(t_shell *shell)
{
	t_env	*tmp;
	int		i;

	if (!shell)
		return ;
	if (shell->envp)
		matriz_delete(shell->envp);
	shell->envp = ft_calloc(env_size(shell->vars) + 1, sizeof(char *));
	if (!shell->envp)
		return ;
	i = -1;
	tmp = shell->vars;
	while (tmp)
	{
		shell->envp[++i] = var_add(tmp);
		if (!shell->envp[i])
			return (matriz_delete(shell->envp));
		if (!ft_strchr(shell->envp[i], '='))
			free(shell->envp[i--]);
		tmp = tmp->next;
	}
}

char	*home_path(t_env *env)
{
	char	*home;
	char	*path;

	home = ft_getenv(env, "HOME");
	if (!home)
		return (NULL);
	path = ft_strjoin("HOME_PATH=", home);
	if (!path)
		return (free(home), NULL);
	free(home);
	return (path);
}
