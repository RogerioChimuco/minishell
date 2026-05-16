/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 01:33:27 by ckulembe          #+#    #+#             */
/*   Updated: 2025/07/05 14:54:32 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned const char	*bt_src;
	unsigned char		*bt_dest;
	size_t				i;

	if ((!dest && n > 0) || !src)
		return (NULL);
	bt_src = (unsigned const char *) src;
	bt_dest = (unsigned char *) dest;
	i = 0;
	if (dest > src)
		while (n--)
			*(bt_dest + n) = *(bt_src + n);
	else
	{
		while (i < n)
		{
			*(bt_dest + i) = *(bt_src + i);
			i++;
		}
	}
	return (dest);
}
