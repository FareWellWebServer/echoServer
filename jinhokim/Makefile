CC = c++

CFLAGS = -std=c++98 -Wall -Wextra -Werror -pedantic

GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

SERVER_SRCS = src/Server.cpp \
				src/http.cpp \
				src/server_main.cpp \

CLIENT_SRCS = src/Client.cpp \
				src/client_main.cpp \

all: server client

server: 
	@echo "$(YELLOW)Building $@...$(RESET)"
	@$(CC) $(SERVER_SRCS) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)Done.$(RESET)"

client: 
	@echo "$(YELLOW)Building $@...$(RESET)"
	@$(CC) $(CLIENT_SRCS) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)Done.$(RESET)"

clean:
	@echo "$(YELLOW)Cleaning up...$(RESET)"
	@echo "$(GREEN)Done.$(RESET)"

fclean:
	make clean
	@rm -f server client

re:
	make fclean
	make all

.PHONY: all clean fclean re
