CXX = c++
CXXFLAGS =
SERVER_NAME = server
SERVER_SRC = TinyServer.cpp main.cpp
SERVER_OBJECT = $(SERVER_SRC:.cpp=.o)

CLIENT_NAME = client
CLIENT_SRC = Client.cpp client_main.cpp
CLIENT_OBJECT = $(CLIENT_SRC:.cpp=.o)

all : $(SERVER_NAME) $(CLIENT_NAME)

$(SERVER_NAME) : $(SERVER_OBJECT)
	$(CXX) $(CXXFLAGS) -o $(SERVER_NAME) $(SERVER_OBJECT)

$(CLIENT_NAME) : $(CLIENT_OBJECT)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_NAME) $(CLIENT_OBJECT)

clean :
	rm -rf $(CLIENT_OBJECT) $(SERVER_OBJECT)

fclean : clean
	rm -rf $(SERVER_NAME) $(CLIENT_NAME)

re : fclean all