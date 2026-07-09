/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 23:26:56 by rochimuc          #+#    #+#             */
/*   Updated: 2025/09/06 00:04:07 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	size_t	len;
	char	*cop;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	cop = (char *)malloc(len + 1);
	if (!cop)
		return (NULL);
	ft_memcpy(cop, s, len + 1);
	return (cop);
}
