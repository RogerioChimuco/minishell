/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 19:02:56 by ckulembe          #+#    #+#             */
/*   Updated: 2025/06/25 06:58:46 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*ft_copy(char const *s, unsigned int start, unsigned int end)
{
	unsigned int		i;
	char				*ptr;

	i = 0;
	ptr = (char *) malloc((end - start + 1) * sizeof(char));
	if (ptr == NULL)
		return (NULL);
	while (i < end - start)
	{
		*(ptr + i) = *(s + start + i);
		i++;
	}
	*(ptr + i) = '\0';
	return (ptr);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	n;
	char	*ptr;

	if (!s || start >= ft_strlen(s))
		return (ft_strdup(""));
	n = 0;
	while (*(s + start + n) && n < len)
		n++;
	ptr = ft_copy(s, start, start + n);
	return (ptr);
}
