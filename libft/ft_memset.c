/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 22:59:32 by ckulembe          #+#    #+#             */
/*   Updated: 2025/07/01 23:15:30 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_invalid_params(void *s, size_t n)
{
	if (!s && n > 0)
		return (0);
	return (1);
}

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*bs;
	size_t			i;

	if (!ft_invalid_params(s, n))
		return (NULL);
	i = 0;
	bs = (unsigned char *)s;
	while (i < n)
	{
		*(bs + i) = (unsigned char)c;
		i++;
	}
	return (s);
}
