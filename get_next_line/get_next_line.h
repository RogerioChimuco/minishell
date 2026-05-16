/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:30:08 by ahuanga           #+#    #+#             */
/*   Updated: 2026/03/26 13:55:36 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif
/* ========== INCLUDES (FUNÇÕES EXTERNAS) ==========*/
# include <unistd.h>
# include <stdlib.h>

/* ========== Função principal ========== */
char	*get_next_line(int fd);

/* ========== Definição da estrutura ========== */
typedef struct s_listt
{
	char			content;
	struct s_listt	*next;
}	t_listt;

/* ========== funções da estrutura ========== */
t_listt	*fget_lstnew(char lst);
void	fget_lstadd_back(t_listt **lst, t_listt *new);
int		fget_lstsize(t_listt *lst);

/* ========== Outras funções da auxiliares ========== */
int		fget_strchr(const char *s, int c);

#endif
