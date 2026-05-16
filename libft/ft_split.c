/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 23:27:35 by ckulembe          #+#    #+#             */
/*   Updated: 2026/02/28 21:14:26 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_count(char const *s, char c)
{
	int	word;
	int	i;

	if (!s || *s == '\0')
		return (0);
	word = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] != c && (i == 0 || s[i - 1] == c))
			word++;
		i++;
	}
	return (word);
}

static char	*ft_next_word(char const *s, char c, int *start)
{
	char	*ptr;
	size_t	len;

	len = 0;
	while (s[*start] && s[*start] == c)
		(*start)++;
	while (s[*start + len] && s[*start + len] != c)
		len++;
	ptr = ft_substr(s, *start, len);
	*start = *start + (int)len;
	return (ptr);
}

static void	ft_free(char **ptr, int qun)
{
	int	i;

	i = 0;
	while (i < qun)
		free(ptr[i++]);
	free(ptr);
}

char	**ft_split(char const *s, char c)
{
	char	**ptr;
	int		words;
	int		i;
	int		start;

	if (!s)
		return (NULL);
	words = ft_count(s, c);
	ptr = (char **) malloc((words + 1) * sizeof(char *));
	if (!ptr)
		return (NULL);
	i = 0;
	start = 0;
	while (i < words)
	{
		ptr[i] = ft_next_word(s, c, &start);
		if (!ptr[i])
		{
			ft_free(ptr, words);
			return (NULL);
		}
		i++;
	}
	ptr[i] = NULL;
	return (ptr);
}
