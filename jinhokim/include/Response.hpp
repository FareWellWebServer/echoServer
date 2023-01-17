#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 10240
#define HEADER_FORMAT "HTTP/1.1 %d %s\r\nContent-Length: %ld\nContent-Type: %s\r\n\r\n"

class Response
{
public:
	Response(char *request);
	~Response(void);

	void ResponseHandler(void);
	void FindMime(char *ct_type, char *uri);
	void FillHeader(int status, long len, std::string type);
	void Handle200(int ct_len, char *local_uri);
	void Handle404(void);
	void Handle500(void);

	std::string GetResponse(void);

private:
	std::string request_;
	std::string response_;
};

#endif // RESPONSE_HPP
