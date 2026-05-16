/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 05:08:41 by rochimuc          #+#    #+#             */
/*   Updated: 2026/01/17 15:07:47 by rochimuc         ###   ########.fr       */
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

static int	ft_overflow(long nbr, int digit, int sign)
{
	if (sign == 1 && (nbr > (INT_MAX - digit) / 10))
		return (1);
	if (sign == -1 && (-nbr < (INT_MIN + digit) / 10))
		return (1);
	return (0);
}

int	ft_atoi(const char *nptr)
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
		if (ft_overflow(nbr, *nptr - '0', sign))
		{
			if (sign == 1)
				return (INT_MAX);
			return (INT_MIN);
		}
		nbr = nbr * 10 + (*nptr - '0');
		nptr++;
	}
	return ((int)(nbr * sign));
}
