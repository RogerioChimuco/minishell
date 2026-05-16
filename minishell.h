/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahuanga <marvin@42fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 13:37:42 by ckulembe          #+#    #+#             */
/*   Updated: 2026/03/26 09:18:56 by ahuanga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# ifndef DOUBLE_QUOTE
#  define DOUBLE_QUOTE '\"'
# endif
# ifndef SINGLE_QUOTE
#  define SINGLE_QUOTE '\''
# endif

# include <errno.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <dirent.h>
# include <string.h>
# include <termios.h>
# include <sys/ioctl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <termcap.h>
# include <errno.h>
# include "libft/libft.h"
# include "types.h"
# include "utils/utils.h"
# include "tokens/tokens.h"
# include "parse/parse.h"
# include "expansion/expansion.h"
# include "executor/redir.h"
# include "builtin/built_in.h"
# include "get_next_line/get_next_line.h"

extern volatile sig_atomic_t	g_signal;

int	ft_validate_quotes(const char *line);

#endif
