
#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	t_list	*node;

	if (!lst || !f)
		return ;
	while (lst)
	{
		node = lst->next;
		f(lst->content);
		lst = node;
	}
	return ;
}
