/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   historic.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:55:01 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:55:04 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "utils.h"

void	ft_historic(void *line)
{
	char	*str;

	str = (char *)line;
	if (!str || str[0] == '\0' || !ft_isprint(str[0]))
		return ;
	add_history(str);
}

static int	count_vars(t_env *vars)
{
	int	count;

	count = 0;
	while (vars)
	{
		count++;
		vars = vars->next;
	}
	return (count);
}

static int	entry_is_valid(char *entry)
{
	return (ft_strchr(entry, '=') != NULL);
}

static int	fill_envp(char **envp, t_env *vars)
{
	int		idx;
	char	*entry;

	idx = 0;
	while (vars)
	{
		entry = var_add(vars);
		if (!entry)
			return (0);
		if (entry_is_valid(entry))
			envp[idx++] = entry;
		else
			free(entry);
		vars = vars->next;
	}
	envp[idx] = NULL;
	return (1);
}

void	envp_update(t_shell *shell)
{
	char	**new_envp;
	int		max;

	if (!shell)
		return ;
	max = count_vars(shell->vars);
	new_envp = ft_calloc(max + 1, sizeof(char *));
	if (!new_envp)
		return ;
	if (!fill_envp(new_envp, shell->vars))
	{
		matriz_delete(new_envp);
		return ;
	}
	if (shell->envp)
		matriz_delete(shell->envp);
	shell->envp = new_envp;
}
