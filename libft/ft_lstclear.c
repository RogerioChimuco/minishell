/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 18:26:09 by rochimuc          #+#    #+#             */
/*   Updated: 2026/07/09 18:26:14 by rochimuc         ###   ########.fr       */
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
