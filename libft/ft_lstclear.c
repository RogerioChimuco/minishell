/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 10:20:02 by ckulembe          #+#    #+#             */
/*   Updated: 2026/03/22 11:38:19 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*node;

	if (!(*lst) || !del)
		return ;
	else
	{
		while (*lst)
		{
			node = (*lst)->next;
			del((*lst)->content);
			free(*lst);
			*lst = NULL;
			*lst = node;
		}
	}
	return ;
}
