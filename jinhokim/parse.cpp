#include <iostream>

void a(void) { system("leaks a.out"); }

int main(void) {
  atexit(a);
  std::string request_("GET / Moved Permanently");

  std::size_t method_end_idx = request_.find_first_of(" ");
  std::cout << "method_end_idx: " << method_end_idx << std::endl;
  std::string method = request_.substr(0, method_end_idx);
  std::size_t uri_end_idx = request_.find_first_of(" ", method_end_idx + 1);
  std::cout << "uri_end_idx: " << uri_end_idx << std::endl;
  std::string uri =
      request_.substr(method_end_idx + 1, uri_end_idx - method_end_idx - 1);

  std::cout << "[INFO] Request: method=" << method << ", URI=" << uri
            << std::endl;
  const char* s = method.c_str();
}