# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ahuanga <marvin@42fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/09 14:38:10 by ckulembe          #+#    #+#              #
#    Updated: 2026/04/09 11:48:48 by ahuanga          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#	Variáveis para processo de compilação

CC	= cc
CFLAGS 	= -Wall -Wextra -Werror

SRC_PARSE    = parse/ft_check_syntax.c \
	       parse/parser.c \
	       parse/utils.c \
	       parse/ft_clean.c
SRC_TOKENS   = tokens/ft_get_token_type.c \
	       tokens/ft_tokenize.c \
	       tokens/utils_tokens.c \
		   tokens/utils_tokens2.c
SRC_EXPAND   = expansion/expansion.c \
	       expansion/utils.c \
	       expansion/expend.c
SRC_EXECUTOR = executor/redir_utils.c \
	       executor/redirects.c \
	       executor/redirects_utils.c \
	       executor/executor.c \
	       executor/ambiguous_redirect.c \
	       executor/child.c \
	       executor/heredoc.c \
	       executor/pipes.c \
	       executor/command.c \
	       executor/pids.c \
	       executor/executor_utils.c
SRC_BUILTIN  = builtin/cd.c \
	       builtin/echo.c \
	       builtin/pwd.c \
	       builtin/utils.c \
	       builtin/export/export.c \
	       builtin/export/utils.c \
	       builtin/env.c \
	       builtin/exit.c \
	       builtin/unset.c \
	       builtin/builtins_utils.c
SRC_UTILS    = utils/historic.c \
	       utils/handler_signal.c \
	       utils/utils.c \
	       utils/vars.c


SRC	     = main.c ft_validate_quotes.c \
			get_next_line/get_next_line_utils.c \
			get_next_line/get_next_line.c \
	       $(SRC_UTILS) \
	       $(SRC_TOKENS) \
	       $(SRC_PARSE) \
	       $(SRC_EXPAND) \
	       $(SRC_EXECUTOR) \
	       $(SRC_BUILTIN)

OBJS	= $(SRC:.c=.o)

NAME	  = minishell
TOKENS    = tokens
EXPAND    = expansion
PARSE     = parse
LIB_DIR   = libft
TOKEN_DIR = tokens
EXECUTOR  = executor
LIBFT	  = $(LIB_DIR)/libft.a
RM	  = rm -f

INCLUDE = builtin/built_in.h executor/redir.h expansion/expansion.h \
			get_next_line/get_next_line.h libft/libft.h \
			parse/parse.h tokens/tokens.h utils/utils.h minishell.h

all: $(NAME) $(INCLUDE)

$(NAME): $(OBJS)
	@make -C $(LIB_DIR)
	@make bonus -C $(LIB_DIR)
	$(CC) $(CFLAGS) -I$(LIB_DIR) -I$(TOKEN_DIR) -I$(PARSE) -I$(EXPAND) -I$(EXECUTOR) $(OBJS) -o $@ -L$(LIB_DIR) -lft -lreadline

%.o:%.c $(INCLUDE)
	$(CC) $(CFLAGS) -I$(TOKEN_DIR) -c $< -o $@

clean:
	@make clean -C $(LIB_DIR)
	@$(RM) $(OBJS)

fclean:	clean
	@make fclean -C $(LIB_DIR)
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
