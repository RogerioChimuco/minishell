/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ckulembe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 08:39:51 by ckulembe          #+#    #+#             */
/*   Updated: 2025/07/07 19:38:36 by ckulembe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_atoi(const char *str)
{
	long long	number;
	int			sign;

	if (!str)
		return (0);
	sign = 1;
	number = 0;
	while (*str == 32 || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign *= -sign;
		str++;
	}
	while (*str >= 48 && *str <= 57)
	{
		number = number * 10 + (*str - '0');
		if (sign == 1 && number > INT_MAX)
			return (-1);
		if (sign == -1 && number < INT_MIN)
			return (0);
		str++;
	}
	return ((int)number * sign);
}
