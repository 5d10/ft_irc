NAME = ircserv

SRC_DIR = src
OBJ_DIR = obj

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

OBJ = $(SRC:%.cpp=${OBJ_DIR}/%.o)
DEP = $(SRC:%.cpp=${OBJ_DIR}/%.d)

all: $(NAME)

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
