/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 19:05:00 by rochimuc          #+#    #+#             */
/*   Updated: 2026/07/09 19:05:14 by rochimuc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include "../minishell.h"

int		ft_check_syntax(t_list *lst);
t_list	*ft_parser(t_list *lst);
t_list	*ft_word_count(t_list *lst, size_t *length);
t_redir	*ft_redirnew(t_list *lst);

void	ft_redirclear(void *arg);
void	command_clear(void *arg);

#endif
