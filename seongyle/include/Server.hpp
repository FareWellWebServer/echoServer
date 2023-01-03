#ifndef SERVER_HPP
# define SERVER_HPP

class Server
{
	private:
	public:
		Server(void);
		Server(const Server& src);
		virtual ~Server(void);
		Server& operator=(Server const& rhs);
};

#endif