# Define the compiler and flags

CXX				:=	c++
CXXFLAGS		:=	-Wall -Wextra -Werror -std=c++98 -pedantic -march=native -O2 -pipe

# Define the directories

SRC_DIR			:=	src
BUILD_DIR		:=	build
INC_DIR			:=	include

# Define the source files

SERVER_SRCS 	:= $(addprefix $(SRC_DIR)/, server_main.cpp Server.cpp Response.cpp)
CLIENT_SRCS 	:= $(addprefix $(SRC_DIR)/, client_main.cpp Client.cpp)
SERVER_OBJS		:= $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SERVER_SRCS))
CLIENT_OBJS		:= $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CLIENT_SRCS))
DEPS			:= $(patsubst %.cpp, $(BUILD_DIR)/%.d, $(SERVER_SRCS) $(CLIENT_SRCS))

# Define the variables for progress bar

TOTAL_FILES		:=	$(shell find $(SRC_DIR) -type f -name '*.cpp' | wc -l)
COMPILED_FILES	:=	0
STEP			:=	100

# Define the name

SERVER			:= server
CLIENT			:= client

# Define the rules

all:
	@$(MAKE) -j $(SERVER)
	@$(MAKE) -j $(CLIENT)

$(SERVER) : $(SERVER_OBJS)
	@$(CXX) $(CXXFLAGS) $^ -o $@
	@printf "\n$(MAGENTA)[WEBSERV] Linking SERVER Success\n$(DEF_COLOR)"

$(CLIENT) : $(CLIENT_OBJS)
	@$(CXX) $(CXXFLAGS) $^ -o $@
	@printf "\n$(MAGENTA)[WEBSERV] Linking CLIENT Success\n$(DEF_COLOR)"

$(BUILD_DIR)/%.o : %.cpp | dir_guard
	@$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c $^ -o $@
	$(eval COMPILED_FILES = $(shell expr $(COMPILED_FILES) + 1))
	$(eval PROGRESS = $(shell expr $(COMPILED_FILES) "*" $(STEP) / $(TOTAL_FILES)))
	@printf "                                                                                                   \r"
	@printf "$(YELLOW)[WEBSERV] [%02d/%02d] ( %3d %%) Compiling $<\r$(DEF_COLOR)" $(COMPILED_FILES) $(TOTAL_FILES) $(PROGRESS)

# $(BUILD_DIR)/%.d : %.cpp | dir_guard
#     @$(CXX) -M $< > $@  
#@$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c $^ -o $@

dir_guard:
	@mkdir -p $(addprefix $(BUILD_DIR)/, $(SRC_DIR))

clean:
	@$(RM) -r $(BUILD_DIR)
	@printf "$(BLUE)[WEBSERV]:\tobj. dep. files$(DEF_COLOR)$(GREEN)	=> Cleaned!\n$(DEF_COLOR)"

fclean: clean
	@$(RM) $(SERVER) $(CLIENT)
	@printf "$(CYAN)[WEBSERV]:\texec. files$(DEF_COLOR)$(GREEN)	=> Cleaned!\n$(DEF_COLOR)"

re: fclean
	@$(MAKE) all
	@printf "$(GREEN)[WEBSERV]Cleaned and Rebuilt everything\n$(DEF_COLOR)"

.PHONY:	all clean fclean re dir_guard norm

norm:
	@(norminette | grep Error) || (printf "$(GREEN)[WEBSERV]:\tNorminette Success\n$(DEF_COLOR)")

# Colors

RESET		=	\033[1;39m
YELLOW		=	\033[1;33m
GRAY		=	\033[1;90m
RED			=	\033[1;91m
GREEN		=	\033[1;92m
YELLOW		=	\033[1;93m
BLUE		=	\033[1;94m
MAGENTA		=	\033[1;95m
CYAN		=	\033[1;96m
WHITE		=	\033[1;97m

-include $(DEPS)
