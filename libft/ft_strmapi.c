/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 22:37:48 by ckulembe          #+#    #+#             */
/*   Updated: 2025/06/25 06:51:04 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	size_t	i;
	size_t	quant;
	char	*ptr;

	if (!s || !f)
		return (NULL);
	i = 0;
	quant = ft_strlen(s);
	ptr = (char *) malloc((quant + 1) * sizeof(char));
	if (!ptr)
		return (NULL);
	while (i < quant)
	{
		ptr[i] = f(i, s[i]);
		i++;
	}
	ptr[i] = '\0';
	return (ptr);
}
