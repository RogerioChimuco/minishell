/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 18:47:54 by rochimuc          #+#    #+#             */
/*   Updated: 2026/07/09 18:47:58 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANSION_H
# define EXPANSION_H

# include "../minishell.h"

void	ft_expasion(t_list **tokens, int *error, t_env *vars);
void	ft_expand(t_token *token, int *error, t_env *vars);
void	ft_remove_quotes(char *str);

char	*ft_token_expend(char *token, t_env *vars, char *var_name, int *i);
char	*get_var_name(char *token);
char	*get_error(char *token, int *error, int *i);
char	*heredoc_expand(char *line, t_shell *shell);
char	*update_token(char *token, const char *content, int *i, int var_length);

#endif
