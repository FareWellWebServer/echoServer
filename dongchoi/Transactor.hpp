#ifndef TRANSACTOR
#define TRANSACTOR

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
		void GetRequest();
		void ProcessRequest();
		void SendResponse();
};

#endif