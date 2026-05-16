/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:56:31 by rochimuc          #+#    #+#             */
/*   Updated: 2025/09/05 03:44:19 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	unsigned char	uc;
	const char		*cs = NULL;

	if (!s)
		return (NULL);
	uc = (unsigned char)c;
	while (*s)
	{
		if (*s == uc)
			cs = s;
		s++;
	}
	if (uc == '\0')
		return ((char *)s);
	return ((char *)cs);
}
