# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rochimuc <rochimuc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/09 17:54:53 by rochimuc          #+#    #+#              #
#    Updated: 2026/07/09 17:54:58 by rochimuc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


#	Variáveis para processo de compilação

CC	= cc
CFLAGS 	= -Wall -Wextra -Werror -g

SRC_PARSE    = parser/ft_check_syntax.c \
	       parser/parser.c \
	       parser/utils.c \
	       parser/ft_clean.c
SRC_TOKENS   = tokens/ft_get_token_type.c \
	       tokens/ft_tokenize.c \
	       tokens/utils_tokens.c \
		   tokens/utils_tokens2.c
SRC_EXPAND   = env_expansion/expansion.c \
	       env_expansion/utils.c \
	       env_expansion/expend.c
SRC_EXECUTOR = execution/redir_utils.c \
	       execution/redirects.c \
	       execution/redirects_utils.c \
	       execution/heredoc_utils.c \
	       execution/executor.c \
	       execution/ambiguous_redirect.c \
	       execution/child.c \
	       execution/heredoc.c \
	       execution/pipes.c \
	       execution/command.c \
	       execution/pids.c \
	       execution/executor_utils.c
SRC_BUILTIN  = builtins/cd.c \
	       builtins/echo.c \
	       builtins/pwd.c \
	       builtins/utils.c \
	       builtins/export/export.c \
	       builtins/export/utils.c \
	       builtins/env.c \
	       builtins/exit.c \
	       builtins/unset.c \
	       builtins/builtins_utils.c
SRC_UTILS    = utils/historic.c \
	       utils/handler_signal.c \
	       utils/utils.c \
	       utils/vars.c \
	       utils/readline_utils.c \
	       utils/prompt.c \
	       utils/vars_extra.c


SRC	     = main.c ft_validate.c \
			gnl/get_next_line_utils.c \
			gnl/get_next_line.c \
	       $(SRC_UTILS) \
	       $(SRC_TOKENS) \
	       $(SRC_PARSE) \
	       $(SRC_EXPAND) \
	       $(SRC_EXECUTOR) \
	       $(SRC_BUILTIN)

OBJS	= $(SRC:.c=.o)

NAME	  = minishell
TOKENS    = tokens
EXPAND    = env_expansion
PARSE     = parser
LIB_DIR   = libft
TOKEN_DIR = tokens
EXECUTOR  = execution
LIBFT	  = $(LIB_DIR)/libft.a
RM	  = rm -f

INCLUDE = builtins/builtins.h execution/redir.h env_expansion/expansion.h \
			gnl/get_next_line.h libft/libft.h \
			parser/parse.h tokens/tokens.h utils/utils.h minishell.h

all: $(NAME) $(INCLUDE)

$(NAME): $(OBJS)
	@make -C $(LIB_DIR)
	@make bonus -C $(LIB_DIR)
	$(CC) $(CFLAGS) -I$(LIB_DIR) -I$(TOKEN_DIR) -I$(PARSER) -I$(EXPAND) -I$(EXECUTOR) $(OBJS) -o $@ -L$(LIB_DIR) -lft -lreadline

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
