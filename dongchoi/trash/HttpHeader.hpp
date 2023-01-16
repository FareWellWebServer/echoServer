#ifndef HTTPHEADER_HPP
#define HTTPHEADER_HPP

#include <string>

class HttpHeader {
    public:
        HttpHeader();
        HttpHeader(std::string method);
        ~HttpHeader();
        void SetMethod(std::string method);

    private:
        std::string method_;
};

#endif