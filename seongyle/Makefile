# Compiler
CC = c++

# Compiler flags
CFLAGS = -std=c++98 -Wall -Wextra -Werror -pedantic

# Colors
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

SERVER_SRC	=	./src/server_main.cpp ./src/Server.cpp
CLIENT_SRC	=	./src/client_main.cpp ./src/Client.cpp

CLIENT_OBJ	=	$(CLIENT_SRC:.cpp=.o)
SERVER_OBJ	=	$(SERVER_SRC:.cpp=.o)

# Default target
all: server client

# Build the server executable
server: $(SERVER_OBJ)
	@echo "$(YELLOW)Building $@...$(RESET)"
	@$(CC) $(CFLAGS) $(SERVER_OBJ) -o server
	@echo "$(GREEN)Done.$(RESET)"

# Build the client executable
client: $(CLIENT_OBJ)
	@echo "$(YELLOW)Building $@...$(RESET)"
	@$(CC) $(CFLAGS) $(CLIENT_OBJ) -o client
	@echo "$(GREEN)Done.$(RESET)"

# Clean up
clean:
	@echo "$(YELLOW)Cleaning up...$(RESET)"
	@rm -rf $(CLIENT_OBJ) $(SERVER_OBJ)
	@echo "$(GREEN)Done.$(RESET)"

fclean:
	make clean
	@rm -f server client

re:
	make fclean
	make all

# Phony targets
.PHONY: all clean fclean re
