NAME = ircserv

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors

ifeq ($(DEBUG), TRUE)
	CFLAGS += -g -fsanitize=address
endif

HEADS = ft_irc.hpp \
		Server.hpp \
		Client.hpp \
		Channel.hpp \
		Task.hpp \
		utils.hpp

SRC = 	main.cpp \
		Server.cpp \
		Client.cpp \
		Channel.cpp \
		Task.cpp \
		utils.cpp\

OBJS = $(SRC:%.cpp=%.o)
DEPS = $(SRC:%.cpp=%.d)

all: $(NAME) 

%.o: %.cpp $(HEADS)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(NAME): Makefile $(OBJS) $(HEADS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug:
	make DEBUG=TRUE

debug_run:
	make run DEBUG=TRUE

run: $(NAME)
	./$(NAME) 6667 "1234"

.PHONY: all clean fclean re debug debug_run run
