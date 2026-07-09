/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_reverse_bits.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 12:58:12 by rochimuc          #+#    #+#             */
/*   Updated: 2026/07/09 18:23:38 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

unsigned char	ft_reverse_bits(unsigned char octet)
{
	unsigned char	res;
	int				i;

	res = 0;
	i = 8;
	while (i > 0)
	{
		res = (res << 1) | (octet & 1);
		octet = octet >> 1;
		i--;
	}
	return (res);
}
