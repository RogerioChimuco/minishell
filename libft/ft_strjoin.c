/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 12:40:38 by rochimuc          #+#    #+#             */
/*   Updated: 2025/09/23 20:47:44 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_strcpy(char *dst, const char *org)
{
	size_t	i;

	i = 0;
	while (org[i])
	{
		dst[i] = org[i];
		i++;
	}
	dst[i] = '\0';
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*n_str;
	size_t	len1;
	size_t	len2;

	if (!s1 || !s2)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	n_str = (char *) malloc (sizeof(char) *(len1 + len2 +1));
	if (!n_str)
		return (NULL);
	ft_strcpy(n_str, s1);
	ft_strcpy(n_str + len1, s2);
	return (n_str);
}
