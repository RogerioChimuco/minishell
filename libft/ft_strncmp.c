/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 08:55:35 by ckulembe          #+#    #+#             */
/*   Updated: 2025/07/05 09:14:27 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_case_null(const char *s1, const char *s2)
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

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	if ((!s1 || !s2) && n > 0)
		return (ft_case_null(s1, s2));
	i = 0;
	while (i < n)
	{
		if (*(s1 + i) != *(s2 + i))
			return (*(s1 + i) - *(s2 + i));
		if (!(*(s1 + i)) || !(*(s2 + i)))
			return (*(s1 + i) - *(s2 + i));
		i++;
	}
	return (0);
}
