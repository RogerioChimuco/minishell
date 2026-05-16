/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 10:34:52 by ckulembe          #+#    #+#             */
/*   Updated: 2025/07/05 08:41:39 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_case_null(const void *s1, const void *s2)
{
	unsigned const char	*bs1;
	unsigned const char	*bs2;

	if (!s1 && !s2)
		return (0);
	else if (!s1)
	{
		bs2 = (unsigned char *)s2;
		return (-(int)(bs2[0]));
	}
	else
	{
		bs1 = (unsigned char *)s1;
		return ((int)(bs1[0]));
	}
}

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*byte_s1;
	const unsigned char	*byte_s2;
	size_t				i;

	if ((!s1 || !s2) && n > 0)
		return (ft_case_null(s1, s2));
	byte_s1 = (const unsigned char *)s1;
	byte_s2 = (const unsigned char *)s2;
	i = 0;
	while (i < n)
	{
		if (*(byte_s1 + i) != *(byte_s2 + i))
			return (*(byte_s1 + i) - *(byte_s2 + i));
		i++;
	}
	return (0);
}
