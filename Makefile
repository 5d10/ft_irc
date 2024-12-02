NAME = ircserv

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

ifeq ($(DEBUG), TRUE)
	CFLAGS +=  -g
endif

HEADS = ft_irc.hpp

SRC = main.cpp

OBJS = $(SRC:%.cpp=%.o)

all: $(NAME) 

%.o: %.cpp $(HEADS)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): Makefile $(OBJS) $(HEADS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug:
	make DEBUG=TRUE

run: $(NAME)
	./$(NAME) 6667 ""