/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:29:22 by ahuanga           #+#    #+#             */
/*   Updated: 2026/03/26 09:24:49 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

t_listt	*fget_lstnew(char lst)
{
	t_listt	*new;

	new = (t_listt *) malloc(sizeof(t_listt));
	if (!new)
		return (NULL);
	new->content = lst;
	new->next = NULL;
	return (new);
}

void	fget_lstadd_back(t_listt **lst, t_listt *new)
{
	t_listt	*last;

	if (!new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	last = *lst;
	while (last->next != NULL)
		last = last->next;
	last->next = new;
}

int	fget_lstsize(t_listt *lst)
{
	int	size;

	if (!lst)
		return (0);
	size = 0;
	while (lst != NULL)
	{
		size++;
		lst = lst->next;
	}
	return (size);
}

int	fget_strchr(const char *s, int c)
{
	if (!s)
		return (0);
	while (*s)
	{
		if (*s == (char)c)
			return (1);
		s++;
	}
	return (0);
}
