
#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*node;

	if (!lst || !new)
		return ;
	if (!(*lst))
		*lst = new;
	else
	{
		node = ft_lstlast(*lst);
		if (!node)
			return ;
		node->next = new;
	}
	return ;
}
