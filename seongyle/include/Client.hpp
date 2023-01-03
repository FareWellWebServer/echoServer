#ifndef CLIENT_HPP
# define CLIENT_HPP

class Client
{
	private:
		
	public:
		Client(void);
		Client(const Client& src);
		virtual ~Client(void);
		Client& operator=(Client const& rhs);
};

#endif