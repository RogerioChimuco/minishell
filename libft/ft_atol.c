/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atol.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 05:08:41 by rochimuc          #+#    #+#             */
/*   Updated: 2026/01/25 02:21:28 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static const char	*ft_skispace(const char *str)
{
	while ((*str >= 9 && *str <= 13) || *str == 32)
		str++;
	return (str);
}

static const char	*ft_getsign(const char *str, int *sign)
{
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			*sign = -1;
		str++;
	}
	return (str);
}

static int	ft_overflow(long nbr, int digit)
{
	if (nbr > (LONG_MAX - digit) / 10)
		return (1);
	return (0);
}

long	ft_atol(const char *nptr)
{
	long	nbr;
	int		sign;

	if (!nptr)
		return (0);
	sign = 1;
	nbr = 0;
	nptr = ft_skispace(nptr);
	nptr = ft_getsign(nptr, &sign);
	while (ft_isdigit(*nptr))
	{
		if (ft_overflow(nbr, *nptr - '0'))
		{
			if (sign == 1)
				return (LONG_MAX);
			return (LONG_MIN);
		}
		nbr = nbr * 10 + (*nptr - '0');
		nptr++;
	}
	return (nbr * sign);
}
