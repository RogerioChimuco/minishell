/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 09:27:31 by ckulembe          #+#    #+#             */
/*   Updated: 2025/06/26 08:48:50 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char		*byte_s;
	size_t					i;

	if (!s && n > 0)
		return (NULL);
	i = 0;
	byte_s = (const unsigned char *)s;
	while (i < n)
	{
		if (*(byte_s + i) == (char)c)
			return ((void *)(byte_s + i));
		i++;
	}
	return (NULL);
}
