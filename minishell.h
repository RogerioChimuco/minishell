/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 17:53:49 by rochimuc          #+#    #+#             */
/*   Updated: 2026/07/09 17:53:50 by rochimuc         ###   ########.fr       */
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
# include "parser/parse.h"
# include "env_expansion/expansion.h"
# include "execution/redir.h"
# include "builtins/builtins.h"
# include "gnl/get_next_line.h"

extern volatile sig_atomic_t	g_signal;

int	ft_validate_quotes(const char *line);

#endif
