/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expend.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 11:11:07 by ahuanga           #+#    #+#             */
/*   Updated: 2026/04/09 13:25:45 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expansion.h"

static void	quote_toggle(char c, int *quote);

static int	quote_manager(t_token *token, int *i, int in_double_quote)
{
	if (!token->value[*i + 1])
		return (0);
	if (!in_double_quote
		&& (token->value[*i + 1] == '"' || token->value[*i + 1] == '\''))
	{
		token->value = update_token(token->value, "", i, 0);
		if (!token->value)
			return (-1);
		return (1);
	}
	return (0);
}

static int	expand_home(t_env *env, t_token *token, int *i)
{
	char	*home;

	if (!token->value || !env)
		return (0);
	home = ft_getenv(env, "HOME_PATH");
	if (!home)
		return (0);
	token->value = update_token(token->value, home, i, 0);
	if (!token->value)
		return (free(home), 0);
	free(home);
	return (1);
}

int	expand_str(t_token *token, t_env *vars, int *error, int *i)
{
	if (token->value[*i + 1] == '?')
		token->value = get_error(token->value, error, i);
	else
	{
		token->value = ft_token_expend(token->value, vars,
				get_var_name(token->value + *i), i);
		if (token->quote_state == NO_QUOTES)
			token->exp = 1;
	}
	if (!token->value)
		return (1);
	return (0);
}

void	ft_expand(t_token *token, int *error, t_env *vars)
{
	int		i;
	int		quote[3];

	if (!token)
		return ;
	i = -1;
	quote[0] = 0;
	quote[1] = 0;
	while (token->value[++i])
	{
		quote_toggle(token->value[i], quote);
		if (token->value[i] == '$' && !quote[0])
		{
			quote[2] = quote_manager(token, &i, quote[1]);
			if (quote[2] == -1)
				return ;
			if (quote[2] == 1)
				continue ;
			if (expand_str(token, vars, error, &i))
				return ;
		}
		else if (token->value[i] == '~' && i == 0 && !quote[0]
			&& (!token->value[i + 1] || token->value[i + 1] == '/'))
			expand_home(vars, token, &i);
	}
}

static void	quote_toggle(char c, int *quote)
{
	if (c == (char)39 && !quote[1])
		quote[0] = !quote[0];
	else if (c == (char)34 && !quote[0])
		quote[1] = !quote[1];
}
