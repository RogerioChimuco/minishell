/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 02:49:46 by ahuanga           #+#    #+#             */
/*   Updated: 2026/03/26 09:24:37 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*fget_read(int fd);
static char	*fget_buffer_update(t_listt **lst, int fd, char *temp);
static char	*fget_rem_nwline(t_listt **lst);

char	*get_next_line(int fd)
{
	static t_listt	*buffer = NULL;
	char			*temp;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	temp = NULL;
	return (fget_buffer_update(&buffer, fd, temp));
}

static char	*fget_rem_nwline(t_listt **lst)
{
	t_listt	*temp;
	char	*content;
	int		size;
	int		i;

	if (!lst || !*lst)
		return (NULL);
	size = fget_lstsize(*lst);
	content = malloc(size + 1);
	if (!content)
		return (NULL);
	i = 0;
	while (*lst)
	{
		content[i++] = (*lst)->content;
		temp = *lst;
		*lst = (*lst)->next;
		free(temp);
		if (content[i - 1] == '\n')
			break ;
	}
	content[i] = '\0';
	return (content);
}

static char	*fget_buffer_update(t_listt **lst, int fd, char *temp)
{
	ssize_t	i;

	while (1)
	{
		temp = fget_read(fd);
		if (temp)
		{
			i = 0;
			while (temp[i])
				fget_lstadd_back(lst, fget_lstnew(temp[i++]));
			if ((temp && fget_strchr(temp, '\n')))
			{
				free (temp);
				return (fget_rem_nwline(lst));
			}
			free (temp);
		}
		else
		{
			if (*lst)
				return (fget_rem_nwline(lst));
			return (NULL);
		}
	}
}

static char	*fget_read(int fd)
{
	char		*reader;
	ssize_t		size;

	if (BUFFER_SIZE <= 0 || fd < 0)
		return (NULL);
	reader = malloc ((BUFFER_SIZE + 1) * sizeof(char));
	if (!reader)
		return (NULL);
	size = read(fd, reader, BUFFER_SIZE);
	if (size <= 0)
	{
		free (reader);
		return (NULL);
	}
	reader[size] = '\0';
	return (reader);
}
