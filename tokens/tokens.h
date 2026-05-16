/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:40:44 by ckulembe          #+#    #+#             */
/*   Updated: 2026/04/09 11:48:04 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENS_H
# define TOKENS_H

# include "../minishell.h"

typedef struct s_data
{
	int	start;
	int	*n;
	int	*i;
}	t_data;

int				ft_is_space(char c);
int				ft_is_quote(char c);
int				ft_is_operator(const char *line, int i);
int				ft_extract_operator(t_list **tokens,
					const char *line, int *n);

void			token_clear(void *arg);
void			ft_get_token_type(void *arg);
void			free_token_partial(t_token *token);
void			ft_add_token(t_list **tokens, const char *line,
					int start, int length);
int				ft_extract_word_in(t_list **tokens, const char *line,
					t_data d);
void			ft_extract_word(t_list **tokens, const char *line,
					int *n, int start);
t_token			*token_create(const char *line, int start,
					int length);					
t_data			ft_data(int start, int *i, int *n);
t_list			*ft_tokenize(const char *line);
t_list			*find_prev(t_list *head, t_list *node);
t_quote_state	ft_get_quote_state(const char *value);

#endif
