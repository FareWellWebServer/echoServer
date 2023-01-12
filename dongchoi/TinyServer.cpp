#include "TinyServer.hpp"


TinyServer::TinyServer(const char* port)
{
	/* socket() */
	listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd_ == -1)
		throw std::runtime_error("TinyServer : socket create failed");

	/* get address infomation */
	struct addrinfo hint;
	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE | AI_NUMERICSERV | AI_ADDRCONFIG;
	getaddrinfo(NULL, port, &hint, &listen_addr_);

	/* kqueue */
	kq_ = kqueue();
	if (kq_ == -1)
		throw std::runtime_error("TinyServer : kq create failed");

	/* event enroll listen FD */
	EV_SET(&event_setting_, listen_fd_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	if (kevent(kq_, &event_setting_, 1, NULL, 0, NULL) == -1) {
		throw std::runtime_error("TinyServer : kevent failed in Construct");
	}
	if (event_setting_.flags & EV_ERROR) {
		throw std::runtime_error("TinyServer : event someting wrong");
	}

}

TinyServer::~TinyServer()
{
	freeaddrinfo(listen_addr_);
}

void TinyServer::Listen()
{
	if (listen(listen_fd_, listen_backlog_cnt_) == -1)
		throw std::runtime_error("TinyServer : listen function failed");
}

void TinyServer::Bind()
{
	if (bind(listen_fd_, listen_addr_->ai_addr, listen_addr_->ai_addrlen) == -1)
		throw std::runtime_error("TinyServer : bind function failed");
}

// accept 할 때 client.addr 정보 저장 해놓을 필요 없을까
void TinyServer::Accept()
{
	struct addrinfo clientSockAddr;
	int connect_fd(0);

	connect_fd = accept(listen_fd_, (clientSockAddr.ai_addr), &(clientSockAddr.ai_addrlen));
	if (connect_fd == -1)
		throw std::runtime_error("TinyServer : accept function failed");
	else
		std::cout << "[Connect Success] fd : " << connect_fd << std::endl;
	clients_fd_.push_back(connect_fd);

	EV_SET(&event_setting_, connect_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	if (kevent(kq_, &event_setting_, 1, NULL, 0, NULL) == -1) {
		throw std::runtime_error("TinyServer : kevent failed in Accept()");
	}
}

// event_trigger size는 어떻게 설정해야 하는가. clents 연결 끊기면 제거해주면 되는가
void TinyServer::Action() {
	struct kevent event_trigger[clients_fd_.size() + 1];
	char buffer[rdwr_buf_size_];
	int exist_events = kevent(kq_, NULL, 0, event_trigger, clients_fd_.size() + 1, 0);
	if (exist_events == -1) {
		throw std::runtime_error("TinyServer : kevent failed in Action()");
	}
	for (int i = 0; i < exist_events; i++) {
		if (event_trigger[i].ident == listen_fd_) {
			Accept();
			continue;
		}
		// already connection
		int client_fd = event_trigger[i].ident;
		// GetRequest();
		// ProcessRequest();
		// SendResponse();
	}
}
void TinyServer::run() {
	Bind();
	Listen();
	Action();
}
