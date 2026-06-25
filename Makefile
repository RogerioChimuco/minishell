
NAME    = minishell
CC      = cc
CFLAGS  = -Wall -Wextra -Werror
LDFLAGS = -lreadline

SRCS    = main.c \
          env.c \
          lexer.c \
          parser.c \
          prompt.c \
          signal.c \
          syntax.c \
          utils.c

OBJS    = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

%.o: %.c minishell.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
