# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rochimuc <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/27 23:47:34 by rochimuc          #+#    #+#              #
#    Updated: 2026/01/16 21:05:22 by rochimuc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME   = libft.a
CC     = cc
HEADER = libft.h
CFLAGS = -Wall -Wextra -Werror
AR     = ar -rcs
RM     = rm -f
SRC = ./ft_atoi.c ./ft_atol.c ./ft_itoa.c ./ft_putendl_fd.c ./ft_strlcat.c ./ft_substr.c ./ft_bzero.c\
 ./ft_putnbr_fd.c ./ft_strlcpy.c ./ft_tolower.c ./ft_calloc.c ./ft_memchr.c ./ft_putstr_fd.c ./ft_strlen.c\
 ./ft_toupper.c ./ft_isalnum.c ./ft_memcmp.c ./ft_split.c ./ft_strmapi.c ./ft_isalpha.c ./ft_memcpy.c\
 ./ft_strchr.c ./ft_strncmp.c ./ft_isascii.c ./ft_memmove.c ./ft_strdup.c ./ft_strnstr.c ./ft_isdigit.c\
 ./ft_memset.c ./ft_striteri.c ./ft_strrchr.c ./ft_isprint.c ./ft_putchar_fd.c ./ft_strjoin.c ./ft_strtrim.c\
 ./ft_swap.c ./ft_swap_bits.c ./ft_reverse_bits.c

OBJS = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)	
	$(AR) $(NAME)  $(OBJS)
	

$(OBJS): $(HEADER) 

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
