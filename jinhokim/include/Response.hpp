#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>

#define BUFSIZE 10240

class Response {
 public:
  Response(char *request);
  ~Response(void);

  void ResponseHandler(void);
  std::string FindMime(std::string uri);
  void FillHeader(int status, long len, std::string type);
  void Handle200(int ct_len, const char *local_uri);
  void Handle404(void);
  void Handle500(void);

  void ComposeResponse(int status, const char *status_text, int len,
                       std::string type);
  std::string GetResponse(void);

 private:
  std::string request_;
  std::string response_;
};

#endif  // RESPONSE_HPP
