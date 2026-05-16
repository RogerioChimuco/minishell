/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 23:32:40 by rochimuc          #+#    #+#             */
/*   Updated: 2025/09/05 01:39:49 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*ptn;
	unsigned char	v;
	size_t			qnt;

	qnt = 0;
	v = (unsigned char)c;
	if (s == NULL)
		return (NULL);
	ptn = (unsigned char *)s;
	while (qnt < n)
	{
		ptn[qnt] = v;
		qnt++;
	}
	return (s);
}
