/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:00:34 by luqalmei          #+#    #+#             */
/*   Updated: 2026/06/09 15:00:41 by luqalmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/*
** node_new — allocate a list node wrapping the given data pointer.
*/
t_node	*node_new(void *data)
{
	t_node	*n;

	n = ft_calloc(1, sizeof(t_node));
	if (!n)
		return (NULL);
	n->data = data;
	n->next = NULL;
	return (n);
}

/*
** node_push_back — append n to the end of the list rooted at *head.
*/
void	node_push_back(t_node **head, t_node *n)
{
	t_node	*cur;

	if (!*head)
	{
		*head = n;
		return ;
	}
	cur = *head;
	while (cur->next)
		cur = cur->next;
	cur->next = n;
}

/*
** node_len — return the number of nodes in the list.
*/
int	node_len(t_node *head)
{
	int	c;

	c = 0;
	while (head)
	{
		c++;
		head = head->next;
	}
	return (c);
}

/*
** node_free_all — walk the list, call del(node->data) on each node,
** free the node itself, then set *head to NULL.
*/
void	node_free_all(t_node **head, void (*del)(void *))
{
	t_node	*cur;
	t_node	*nxt;

	if (!head || !*head)
		return ;
	cur = *head;
	while (cur)
	{
		nxt = cur->next;
		if (del && cur->data)
			del(cur->data);
		free(cur);
		cur = nxt;
	}
	*head = NULL;
}
