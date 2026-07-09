/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 18:48:14 by rochimuc          #+#    #+#             */
/*   Updated: 2026/07/09 18:48:16 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expansion.h"

void	ft_remove_quotes(char *str)
{
	char	quote;
	int		i;
	int		e;

	if (!str)
		return ;
	i = 0;
	e = -1;
	quote = 0;
	while (str[++e])
	{
		if ((str[e] == (char)34 || str[e] == (char)39) && !quote)
			quote = str[e];
		else if (str[e] == quote)
			quote = 0;
		else
			str[i++] = str[e];
	}
	str[i] = '\0';
}

char	*ft_token_expend(char *token, t_env *vars, char *var_name, int *i)
{
	char	*value;
	int		pos;

	pos = *i;
	if (!var_name)
		return (token);
	pos++;
	value = ft_getenv(vars, var_name);
	if (!value)
		token = update_token(token, "", i, ft_strlen(var_name));
	else
	{
		token = update_token(token, value, i, ft_strlen(var_name));
		free(value);
	}
	return (free(var_name), token);
}

char	*get_var_name(char *token)
{
	char	*name;
	int		i;

	i = 1;
	while (token[i] && (ft_isalnum(token[i]) || token[i] == '_'))
		i++;
	if (i == 1)
		return (NULL);
	name = ft_calloc(i, sizeof(char));
	if (!name)
		return (NULL);
	ft_strlcpy(name, token + 1, i);
	return (name);
}

char	*get_error(char *token, int *error, int *i)
{
	char	*status;

	status = ft_itoa(*error);
	token = update_token(token, status, i, 1);
	if (status)
		free(status);
	return (token);
}

char	*update_token(char *token, const char *content, int *i, int var_length)
{
	char	*new_token;
	int		length[3];
	int		pos;

	length[0] = ft_strlen(token);
	length[1] = ft_strlen(content);
	length[2] = (length[0] - (var_length + 1) + length[1] + 1);
	new_token = ft_calloc(length[2], 1);
	if (!new_token)
		return (free(token), NULL);
	pos = *i;
	ft_memcpy(new_token, token, pos);
	ft_memcpy(new_token + pos, content, (length[1]));
	*i = pos + length[1] - 1;
	ft_strlcpy(new_token + (*i + 1), token + (pos + 1 + var_length),
		(length[2]));
	free(token);
	return (new_token);
}
