/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 02:52:05 by ckulembe          #+#    #+#             */
/*   Updated: 2025/06/26 08:37:02 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	const unsigned char	*byte_src;
	unsigned char		*byte_dest;
	size_t				i;

	if ((!dest && !src) && n > 0)
		return (NULL);
	byte_dest = (unsigned char *)dest;
	byte_src = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		*(byte_dest + i) = *(byte_src + i);
		i++;
	}
	return (dest);
}
