#include "Client.hpp"

Client::Client(const int fd)
{
	std::cout << "create clinet by fd" << std::endl;
}

Client::~Client(void)
{
	// TODO: close fd;
}
