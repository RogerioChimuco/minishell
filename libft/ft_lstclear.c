
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
