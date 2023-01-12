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

void Transactor::GetRequest()
{
	// 소켓 읽어서 임시파일에 저장
	// 파서 생성(임시파일 넣어주기)
	// 파서 실행 -> HttpHeader 뱉어줌(맵으로 저장)
	// 임시파일 지우기
}

void Transactor::ProcessRequest()
{
	// HttpHeader 보면서 보내줄 문서 만들기
}

void Transactor::SendResponse()
{
	// 문서 보내기
}
