NAME		=	philo
CC			=	cc
CFLAGS		=	-Wall -Werror -Wextra

SRCS		=	$(addsuffix .c, \
				forks \
				init \
				main \
				print_msg \
				routine \
				time \
				tools \
				unset \
				)

OBJS		=	$(SRCS:.c=.o)
INCLUDES	=	philo.h

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lpthread -o $(NAME)

%.o : %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re