/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_lines.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 16:02:42 by ckulembe          #+#    #+#             */
/*   Updated: 2026/03/29 23:10:37 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*new_strjoin(char *s1, char *s2)
{
	char	*tmp;
	size_t	size1;
	size_t	size2;

	if (!s1)
		s1 = ft_strdup("");
	size1 = ft_strlen(s1);
	size2 = ft_strlen(s2);
	tmp = malloc(size1 + size2 + 1);
	if (!tmp)
		return (NULL);
	ft_memcpy(tmp, s1, size1);
	ft_strlcpy(tmp + size1, s2, size2);
	ft_free((void *)&s1, 0);
	return (tmp);
}

static char	*ft_get_lines(void)
{
	int		i;
	char	*tmp;
	char	*line;

	i = 1;
	line = NULL;
	while (++i)
	{
		tmp = get_next_line(0);
		if (!tmp)
			break ;
		line = new_strjoin(line, tmp);
		if (!line)
			return (ft_free((void *)&tmp, (void *)&line), NULL);
		free(tmp);
	}
	return (line);
}

char	*ft_readline(char *prompt)
{
	if (!prompt)
		return (NULL);
	if (!isatty(0))
		return (ft_get_lines());
	else
		return (readline(prompt));
	return (NULL);
}
