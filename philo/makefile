NAME		=	philo
CC			=	cc
CFLAGS		=	-Wall -Werror -Wextra

SRCS		=	$(addsuffix .c, \
				main \
				tools \
				)

OBJS		=	$(SRCS:.c=.o)
INCLUDES	=	philo.h

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o : %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re