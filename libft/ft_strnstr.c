/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 02:38:04 by ckulembe          #+#    #+#             */
/*   Updated: 2025/07/02 15:24:33 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;
	size_t	little_len;
	size_t	big_len;

	if ((!big && len > 0) || !little)
		return (NULL);
	i = 0;
	big_len = ft_strlen(big);
	little_len = ft_strlen(little);
	if (little_len == 0)
		return ((char *)(big + i));
	while (i < len)
	{
		if (len - i < little_len || big_len < little_len)
			return (NULL);
		j = 0;
		while (*(big + (i + j)) == *(little + j))
		{
			if (++j == little_len)
				return ((char *)(big + i));
		}
		i++;
	}
	return (NULL);
}
