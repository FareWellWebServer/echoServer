#ifndef HTTP_HPP
# define HTTP_HPP

# include <iostream>
# include <string.h>
# include <unistd.h>
# include <sys/stat.h>
# include <fcntl.h>

# define BUFSIZE 1024
# define HEADER_FORMAT  "HTTP/1.1 %d %s\r\nContent-Length: %ld\nContent-Type: %s\r\n"
# define NOT_FOUND_CONTENT "<h1>404 Not Found</h1>\n"
# define SERVER_ERROR_CONTENT "<h1>500 Internal Server Error</h1>\n"

class Http {
    public:
        Http(char* request);
        ~Http(void);

		void	FillHeader(long len, std::string type);
		void    Handle404(void);
		void    Handle500(void);
		void    FindMime(char* ct_type, char* uri);
		void    HttpHandler(void);

		std::string	GetHtml(int socket) const;
		std::string	GetHeader(void) const;
	private:
        std::string	request_;
        std::string	header_;
		int		status_;
		int		fd_;
};

#endif // HTTP_HPP
