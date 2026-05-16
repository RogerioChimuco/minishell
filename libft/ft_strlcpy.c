/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 02:17:45 by ckulembe          #+#    #+#             */
/*   Updated: 2025/07/02 08:30:01 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_invalid_params(char *dst, const char *src, size_t size)
{
	if ((!dst && size > 0) || !src)
		return (0);
	return (1);
}

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t		i;

	if (!ft_invalid_params(dst, src, size))
		return (0);
	if (size > 0)
	{
		i = 0;
		while (i < size - 1 && *(src + i))
		{
			*(dst + i) = *(src + i);
			i++;
		}
		*(dst + i) = '\0';
	}
	return (ft_strlen(src));
}
