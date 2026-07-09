/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luqalmei <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 12:53:22 by luqalmei          #+#    #+#             */
/*   Updated: 2026/07/09 12:53:31 by luqalmei         ###   ########.fr       */
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
int				extract_quoted_word(t_list **tokens, const char *line,
					t_data d);
void			ft_extract_word(t_list **tokens, const char *line,
					int *n, int start);
t_token			*token_create(const char *line, int start,
					int length);					
t_data			build_word_data(int start, int *i, int *n);
t_list			*ft_tokenize(const char *line);
t_list			*find_previous_node(t_list *head, t_list *node);
t_quote_state	ft_get_quote_state(const char *value);

#endif
