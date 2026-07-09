/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 17:15:59 by rochimuc          #+#    #+#             */
/*   Updated: 2025/09/23 20:43:34 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

static int	em_set(char c, char const *set)
{
	size_t	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	len;
	size_t	i;

	if (!s1 || !set)
		return (0);
	len = ft_strlen(s1);
	i = 0;
	while (s1[i] && em_set(s1[i], set))
		i++;
	while (len > i && em_set(s1[len - 1], set))
		len--;
	return (ft_substr(s1, i, len - i));
}
