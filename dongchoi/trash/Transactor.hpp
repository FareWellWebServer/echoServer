#ifndef TRANSACTOR
#define TRANSACTOR


#include <fstream>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class HttpHeader;

class Transactor {
	public:
		Transactor(int client_fd);
		~Transactor();
		void Transact();

	private:
		int client_fd_;
		int response_file_fd_;
		HttpHeader* request_;
		const char* write_file_name_temp;
		std::string method_;
		void GetRequest(int client_fd);
		void ProcessRequest();
		void SendResponse();
};

#endif