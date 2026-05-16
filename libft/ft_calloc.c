/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 02:59:13 by ckulembe          #+#    #+#             */
/*   Updated: 2025/06/21 08:59:32 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t num, size_t size)
{
	void	*tmp;

	if (size != 0 && num > SIZE_MAX / size)
		return (NULL);
	tmp = malloc(num * size);
	if (tmp != NULL)
		ft_memset(tmp, '\0', num * size);
	return (tmp);
}
