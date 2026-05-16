/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 02:47:54 by rochimuc          #+#    #+#             */
/*   Updated: 2025/09/05 01:18:08 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*p = (const unsigned char *)s;
	unsigned char		uc;

	uc = (unsigned char)c;
	if (s != NULL)
	{
		while (n--)
		{
			if (*p == uc)
				return ((void *)p);
			p++;
		}
	}
	return (NULL);
}
