/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 12:52:19 by ckulembe          #+#    #+#             */
/*   Updated: 2026/02/09 10:17:08 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	copy(char *dst, const char *src)
{
	size_t	i;

	i = 0;
	while (*(src + i))
	{
		*(dst + i) = *(src + i);
		i++;
	}
	*(dst + i) = '\0';
}

char	*ft_strdup(const char *s)
{
	char	*p;

	if (!s)
		return (NULL);
	p = (char *) ft_calloc(ft_strlen(s) + 1, sizeof(char));
	if (!p)
		return (NULL);
	copy(p, s);
	return (p);
}
