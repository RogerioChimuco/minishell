/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 20:48:21 by rochimuc          #+#    #+#             */
/*   Updated: 2025/09/24 22:22:04 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_numsepa(char const *s, char c)
{
	int	letter;
	int	word;

	letter = 0;
	word = 0;
	while (*s)
	{
		if (*s != c && !word)
		{
			letter ++;
			word = 1;
		}
		if (*s == c)
			word = 0;
		s ++;
	}
	return (letter);
}

size_t	ft_array(char **array, const char *s, char c)
{
	size_t	j;
	size_t	i;
	size_t	sep;

	i = 0;
	sep = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		if (s[i])
		{
			j = i;
			while (s[i] && s[i] != c)
				i++;
			array[sep] = ft_substr(s, j, i - j);
			sep++;
			j = i + 1;
		}
	}
	return (sep);
}

char	**ft_split(char const *s, char c)
{
	char	**array;
	size_t	sep;

	if (!s)
		return (0);
	sep = ft_numsepa(s, c);
	array = malloc ((sep + 1) * sizeof(char *));
	if (!array)
	{
		free(array);
		return (NULL);
	}
	ft_array(array, s, c);
	array[sep] = NULL;
	return (array);
}
