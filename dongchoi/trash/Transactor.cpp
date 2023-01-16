#include "Transactor.hpp"

Transactor::Transactor(int client_fd) : client_fd_(client_fd), request_(0)
{
}

Transactor::~Transactor()
{

}

void Transactor::Transact()
{

}

void Transactor::GetRequest(int client_fd)
{
	// 소켓 읽어서 임시파일에 저장
	// 파서 생성(임시파일 넣어주기)
	// 파서 실행 -> HttpHeader 뱉어줌(맵으로 저장)
	// 임시파일 지우기
	std::string file_name("request_http_header");
	file_name.append(std::to_string(client_fd)); // C++11
	write_file_name_temp = file_name.c_str();
	int write_file_fd = open(file_name.c_str(), O_WRONLY | O_CREAT);
	if (write_file_fd == -1)
		throw std::runtime_error("GetRequest : write file open error");
	char buf[1024];
	while(read(client_fd, buf, 1024))
		write(write_file_fd, buf, 1024);
}

void Transactor::ProcessRequest()
{
	// HttpHeader 보면서 보내줄 문서 만들기
}

void Transactor::SendResponse()
{
	// 문서 보내기
}
