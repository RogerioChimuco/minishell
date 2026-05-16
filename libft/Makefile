NAME = libft.a

CC = cc
CFLAGS = -Wall -Wextra -Werror -fPIC

SRCS = ft_isalpha.c ft_isdigit.c ft_isalnum.c ft_isascii.c ft_isprint.c \
	ft_strlen.c ft_memset.c ft_bzero.c ft_memcpy.c ft_memmove.c \
	ft_strlcpy.c ft_strlcat.c ft_toupper.c ft_tolower.c ft_strchr.c \
	ft_strrchr.c ft_strncmp.c ft_memchr.c ft_memcmp.c ft_strnstr.c \
	ft_atoi.c ft_calloc.c ft_strdup.c ft_substr.c ft_strjoin.c \
	ft_strtrim.c ft_split.c ft_itoa.c ft_putchar_fd.c ft_putstr_fd.c \
	ft_putendl_fd.c ft_strmapi.c ft_putnbr_fd.c ft_striteri.c ft_strcmp.c

BONUS_SRCS = ft_lstnew.c ft_lstlast.c ft_lstsize.c ft_lstmap.c ft_lstdelone.c\
	   ft_lstadd_front.c ft_lstadd_back.c ft_lstclear.c ft_lstiter.c

OBJS = $(SRCS:.c=.o)
BONUS_OBJS = $(BONUS_SRCS:.c=.o)

all:	$(NAME)

$(NAME): 	$(OBJS)
	ar rcs $@ $^

bonus: $(NAME) .bonus

.bonus: $(BONUS_OBJS)
	ar rcs $(NAME) $(BONUS_OBJS)
	touch $@

$(OBJS): libft.h
$(BONUS_OBJS): libft.h

clean:
	rm -f $(OBJS) $(BONUS_OBJS)

fclean: clean
	rm -f $(NAME) .bonus

re:	fclean all

.PHONY: all clean fclean re bonus
