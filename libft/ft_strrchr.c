/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 04:58:35 by ckulembe          #+#    #+#             */
/*   Updated: 2025/06/26 10:30:54 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	unsigned char	bc;
	int				i;
	int				t;

	if (!s)
		return (NULL);
	t = -1;
	i = 0;
	bc = (unsigned char)c;
	while (*(s + i))
	{
		if (*(s + i) == bc)
			t = i;
		i++;
	}
	if (t >= 0)
		return ((char *)(s + t));
	else if (bc == '\0')
		return ((char *)(s + i));
	return (NULL);
}
