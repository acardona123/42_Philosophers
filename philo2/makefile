NAME		=	philo
CC			=	cc
CFLAGS		=	-Wall -Werror -Wextra

SRCS		=	$(addsuffix .c, \
				main \
				init \
				unset \
				routine \
				routine1 \
				tools \
				)

OBJS		=	$(addprefix obj/, $(patsubst %.c, %.o, $(SRCS)))
INCLUDES	=	philo.h

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lpthread -o $(NAME)

obj/%.o : %.c $(INCLUDES)
	mkdir -p obj/
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -rf obj/

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re