/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 04:16:06 by rochimuc          #+#    #+#             */
/*   Updated: 2025/09/24 22:19:37 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_numlen(long n)
{
	int	len;

	len = 0;
	if (n <= 0)
		len = 1;
	if (n < 0)
		n = -n;
	while (n > 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

static char	*ft_convert(char *s, long nbr)
{
	if (nbr >= 10)
		s = ft_convert(s, nbr / 10);
	*s = (nbr % 10) + '0';
	return (s + 1);
}

char	*ft_itoa(int n)
{
	char	*str;
	char	*end_ptr;
	long	nbr;
	int		len;

	nbr = n;
	len = ft_numlen(nbr);
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	if (nbr < 0)
	{
		str[0] = '-';
		nbr = -nbr;
		end_ptr = ft_convert(str + 1, nbr);
	}
	else
		end_ptr = ft_convert(str, nbr);
	*end_ptr = '\0';
	return (str);
}
