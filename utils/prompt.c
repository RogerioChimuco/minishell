/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:45:00 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 15:45:00 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

char	*prompt_user(t_env *env)
{
	char	*user;

	user = ft_getenv(env, "USER");
	if (!user)
		user = ft_strdup("~");
	return (user);
}

char	*prompt_path(t_env *env)
{
	char	*path;
	char	*user;

	path = getcwd(NULL, 0);
	if (!path)
		return (ft_strdup("minishel$ "));
	user = ft_getenv(env, "HOME");
	if (user)
	{
		apply_home_shorthand(&path, user);
		free(user);
	}
	return (path);
}
