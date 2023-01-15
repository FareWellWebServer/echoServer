#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <cstring>
# include <sstream>

# include <sys/stat.h>
# include <fcntl.h>

# include <string.h>
# include <unistd.h>

# define BUFSIZE 				1024
# define HEADER_FORMAT  		"HTTP/1.1 %d %s\r\nContent-Length: %ld\nContent-Type: %s\r\n\r\n"
# define NOT_FOUND_CONTENT 		"<h1>404 Not Found</h1>\n"
# define SERVER_ERROR_CONTENT 	"<h1>500 Internal Server Error</h1>\n"

class Response {
    public:
        Response(char* request);
        ~Response(void);

		void    	ResponseHandler(void);
		void    	FindMime(char* ct_type, char* uri);
		void		FillHeader(int status, long len, std::string type);
		void    	Handle200(int ct_len, char* local_uri);
		void    	Handle404(void);
		void    	Handle500(void);

		std::string	GetResponse(void);
	private:
        std::string	request_;
        std::string	response_;
};

#endif // RESPONSE_HPP
