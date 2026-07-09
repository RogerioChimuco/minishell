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
