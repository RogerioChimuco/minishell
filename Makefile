
NAME    = minishell
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -Iinclude

# ---------------------------------------------------------------------------- #
# Sources
# ---------------------------------------------------------------------------- #
SRC_CORE = src/main.c      \
           src/signals.c   \
           src/input.c     \
           src/env.c       \
           src/list.c      \
           src/utils.c

SRC_LIBFT = libft/ft_strlen.c   \
            libft/ft_strdup.c   \
            libft/ft_substr.c   \
            libft/ft_strjoin.c  \
            libft/ft_strtrim.c  \
            libft/ft_strcmp.c   \
            libft/ft_strncmp.c  \
            libft/ft_strchr.c   \
            libft/ft_calloc.c   \
            libft/ft_memcpy.c   \
            libft/ft_itoa.c     \
            libft/ft_atoi.c     \
            libft/ft_isdigit.c  \
            libft/ft_isalpha.c  \
            libft/ft_isalnum.c  \
            libft/ft_putstr_fd.c \
            libft/ft_putchar_fd.c \
            libft/ft_putendl_fd.c \
            libft/ft_split.c    \
            libft/ft_strlcpy.c  \
            libft/ft_strlcat.c

SRC     = $(SRC_CORE) $(SRC_LIBFT)
OBJS    = $(SRC:.c=.o)

# ---------------------------------------------------------------------------- #
# Rules
# ---------------------------------------------------------------------------- #
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -lreadline

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
