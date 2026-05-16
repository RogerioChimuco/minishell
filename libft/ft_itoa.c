/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 16:42:30 by ckulembe          #+#    #+#             */
/*   Updated: 2025/06/25 19:01:21 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_number(int n)
{
	int	count;

	count = 0;
	if (n <= 0)
		count = 1;
	while (n != 0)
	{
		n /= 10;
		count++;
	}
	return (count);
}

static void	calc(char *dest, long num, int len)
{
	int	i;

	i = len;
	while (num > 0)
	{
		dest[--i] = (num % 10) + '0';
		num /= 10;
	}
}

char	*ft_itoa(int n)
{
	int		len;
	char	*dest;
	long	number;

	len = count_number(n);
	dest = malloc(len + 1);
	if (!dest)
		return (NULL);
	dest[len] = '\0';
	number = n;
	if (number == 0)
		dest[0] = '0';
	if (number < 0)
	{
		dest[0] = '-';
		number = -number;
	}
	calc(dest, number, len);
	return (dest);
}
