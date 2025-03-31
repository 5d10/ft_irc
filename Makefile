NAME = ircserv

SRC_DIR = src
OBJ_DIR = obj
HEAD_DIR = headers/

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -I headers/

ifeq ($(DEBUG), TRUE)
	CFLAGS += -g -fsanitize=address -D DEBUG=1
endif

SRC = 	main.cpp \
		Server.cpp \
		Client.cpp \
		Channel.cpp \
		Task.cpp \
		utils.cpp\

HEAD =	Channel.hpp \
		Client.hpp \
		Colors.h \
		ft_irc.hpp \
		Server.hpp \
		Task.hpp \
		utils.hpp

OBJ = $(SRC:%.cpp=${OBJ_DIR}/%.o)
DEP = $(SRC:%.cpp=${OBJ_DIR}/%.d)
HEADS = $(addprefix $(HEAD_DIR), $(HEAD))

all: $(NAME) $(HEADS)

$(NAME): Makefile $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ): $(OBJ_DIR)%.o: ${SRC_DIR}%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@
-include $(DEP)

clean:
	rm -rf $(OBJ_DIR)

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
