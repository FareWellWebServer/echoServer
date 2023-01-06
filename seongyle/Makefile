# Compiler
CC = c++

# Compiler flags
CFLAGS = -std=c++98 -Wall -Wextra -Werror -pedantic

# Colors
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

# Default target
all: server client

# Build the server executable
server: chat_gpt_server.cpp
	@echo "$(YELLOW)Building $@...$(RESET)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)Done.$(RESET)"

# Build the client executable
client: chat_gpt_client.cpp
	@echo "$(YELLOW)Building $@...$(RESET)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)Done.$(RESET)"

# Clean up
clean:
	@echo "$(YELLOW)Cleaning up...$(RESET)"
	@echo "$(GREEN)Done.$(RESET)"

fclean:
	make clean
	@rm -f server client

re:
	make fclean
	make all

# Phony targets
.PHONY: all clean re
