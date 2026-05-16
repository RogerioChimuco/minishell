/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 17:07:32 by rochimuc          #+#    #+#             */
/*   Updated: 2025/09/15 12:36:18 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*no_str(void)
{
	char	*ps;

	ps = (char *)malloc(1);
	if (!ps)
		return (NULL);
	ps[0] = '\0';
	return (ps);
}

static size_t	cps_len(unsigned int start, size_t len, size_t s_len)
{
	if (start >= s_len)
		return (0);
	if (s_len - start < len)
		return (s_len - start);
	return (len);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*ps;
	size_t	j;
	size_t	s_len;
	size_t	ps_len;

	if (!s)
		return (NULL);
	s_len = ft_strlen(s);
	if (start >= s_len)
		return (no_str());
	j = 0;
	ps_len = cps_len(start, len, s_len);
	ps = (char *)malloc(ps_len + 1);
	if (!ps)
		return (NULL);
	while (j < ps_len)
	{
		ps[j] = s[start + j];
		j++;
	}
	ps[j] = '\0';
	return (ps);
}
