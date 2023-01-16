#include <vector>
#include <string>


/* Config file parsing */
class ServerConfigInfo {
    public:
        //config 데이터들 갖고 올 수 있는 메소드 함수 만들기

    private:
        struct server_data s_data_; // config에 들어있는 정보들 다 넣기
        std::vector<struct location> locations_; // location이 여러개일 수 있으니 vector로 관리

};


/* Request Handler */
// 1. 요청 메시지 첫줄 유효성 체크
// 2. 에러로그, access 로그 Handler 호출
// 3. parse for each method
// 4. 정보를 <RequestController>
class ReqHandler{
    public:
        bool IsValidMethod(const std::string &line); // 사용할 수 있는 Method 인지? (글자만 확인)
        bool IsVaildHTTP(const std::string &line); // HTTP 버전 1.1 아니면 false
        bool IsValidURL(const std::string &line); // 글자만 확인
    private:
        bool IsValidFirstLien(); // 글자 자체만 확인
        void Parse(); // 요청 메시지 파싱. 이때, Config 내용 보면서 에러처리 (throw)
        struct something TansferToCore();
};


/* MsgComposer */
// 응답 메시지 구성
// status code, entity(char*), cgi에서오면 pipe(fd)
class MsgComposer{
    public:

    private:
        // string.append  ||  pipe에 write해서 ResHandler에서 pipe read
};