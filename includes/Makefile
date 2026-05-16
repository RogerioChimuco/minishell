# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rochimuc <franciscogama99@outlook.com>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/15 07:05:17 by rochimuc          #+#    #+#              #
#    Updated: 2026/01/25 21:03:54 by rochimuc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = push_swap
CC = cc
CFLAGS = -Wall -Wextra -Werror
HEADER = ./includes/push_swap.h
AR = ar -rcs
RM = rm -f
LIBFT = ./libft/libft.a
PRINTF = ./ft_print/libftprintf.a
SRC_DIR = ./src
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/stack.c $(SRC_DIR)/utils.c $(SRC_DIR)/move.c \
	  $(SRC_DIR)/sort.c $(SRC_DIR)/aux.c $(SRC_DIR)/push_swap.c $(SRC_DIR)/init_utils.c \
	  $(SRC_DIR)/sort_algo.c $(SRC_DIR)/target.c $(SRC_DIR)/turk_utils.c $(SRC_DIR)/cheap.c
OBJ = $(SRC:.c=.o)

	
all: $(NAME)
$(NAME): $(LIBFT) $(PRINTF) $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIBFT) $(PRINTF)
$(OBJ): $(HEADER)

$(LIBFT):
	make -C ./libft
$(PRINTF):
	make -C ./ft_print
clean:
	$(RM) $(OBJ)
	make -C ./libft clean
	make -C ./ft_print clean
fclean: clean
	$(RM) $(NAME)
	make -C ./libft fclean
	make -C ./ft_print fclean
re: fclean all
.PHONY: all clean fclean re