/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 13:02:46 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 13:02:49 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*gnl_join_free(char *s1, char *s2)
{
	char	*result;

	if (!s1)
		result = ft_strjoin("", s2);
	else
		result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

char	*extract_line(char *stash)
{
	int	i;

	i = 0;
	while (stash[i] && stash[i] != '\n')
		i++;
	if (stash[i] == '\n')
		i++;
	return (ft_substr(stash, 0, (size_t)i));
}

char	*update_stash(char *stash)
{
	char	*new_stash;
	int		i;

	i = 0;
	while (stash[i] && stash[i] != '\n')
		i++;
	if (stash[i] == '\n')
		i++;
	if (!stash[i])
	{
		free(stash);
		return (NULL);
	}
	new_stash = ft_substr(stash, (unsigned int)i, ft_strlen(stash + i));
	free(stash);
	return (new_stash);
}
