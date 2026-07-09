/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:22:33 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 15:22:40 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "utils.h"
#include "../gnl/get_next_line.h"

static char	*read_from_pipe(void)
{
	char	*line;
	char	*trimmed;

	line = get_next_line(fileno(stdin));
	if (!line)
		return (NULL);
	trimmed = ft_strtrim(line, "\n");
	free(line);
	return (trimmed);
}

char	*ft_readline(char *prompt)
{
	if (isatty(fileno(stdin)))
		return (readline(prompt));
	return (read_from_pipe());
}

char	*home_path(t_env *env)
{
	char	*home;
	char	*result;

	home = ft_getenv(env, "HOME");
	if (!home)
		return (NULL);
	result = ft_strjoin("HOME_PATH=", home);
	free(home);
	return (result);
}

void	apply_home_shorthand(char **path, char *home)
{
	size_t	len;
	char	*tmp;

	len = ft_strlen(home);
	if (len == 0 || ft_strncmp(*path, home, len) != 0)
		return ;
	tmp = ft_strjoin("~", *path + len);
	if (!tmp)
		return ;
	free(*path);
	*path = tmp;
}

char	*concat_and_free(char *s1, char *s2)
{
	char	*joined;

	joined = ft_strjoin(s1, s2);
	free(s1);
	return (joined);
}
