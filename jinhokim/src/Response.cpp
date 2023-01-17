#include "Response.hpp"

Response::Response(char* request) : request_(std::string(request)) {}

Response::~Response(void) {}

void Response::ResponseHandler(void) {
  if (request_.size() == 0) {
    std::cerr << "[ERROR] Failed to read request" << std::endl;
    Handle500();
    return;
  }

  std::size_t method_end_idx = request_.find_first_of(" ");
  std::string method = request_.substr(0, method_end_idx);
  std::size_t uri_end_idx = request_.find_first_of(" ", method_end_idx + 1);
  std::string uri =
      request_.substr(method_end_idx + 1, uri_end_idx - method_end_idx - 1);

  if (method.size() == 0 || uri.size() == 0) {
    std::cerr << "[ERROR] Failed to identify method, URI" << std::endl;
    Handle500();
    return;
  }

  std::cout << "[INFO] Request: method=" << method << ", URI=" << uri
            << std::endl;

  std::string local_uri;
  if (!uri.compare("/"))
    local_uri = "html/index.html";
  else
    local_uri = uri.substr(1, uri.size());

  struct stat st;

  if (stat(local_uri.c_str(), &st) < 0) {
    std::cerr << "[WARN] No file found matching URI" << std::endl;
    Handle404();
    return;
  }

  Handle200(st.st_size, local_uri.c_str());
  return;
}

void Response::Handle200(int ct_len, const char* local_uri) {
  std::string ct_type;
  char buf[BUFSIZE];
  int r;
  int fd = open(local_uri, O_RDONLY);

  ct_type = FindMime(local_uri);
  FillHeader(200, ct_len, ct_type);
  while ((r = read(fd, buf, BUFSIZE)) > 0) response_.append(buf);
}

void Response::Handle404(void) {
  struct stat st;
  std::string ct_type;
  char buf[BUFSIZE];
  char uri[14] = "html/404.html";
  int r;
  int fd = open(uri, O_RDONLY);

  stat(uri, &st);
  ct_type = FindMime(uri);
  FillHeader(404, st.st_size, ct_type);
  while ((r = read(fd, buf, BUFSIZE)) > 0) response_.append(buf);
}

void Response::Handle500(void) {
  struct stat st;
  std::string ct_type;
  char buf[BUFSIZE];
  char uri[14] = "html/500.html";
  int r;
  int fd = open(uri, O_RDONLY);

  stat(uri, &st);
  ct_type = FindMime(uri);
  FillHeader(500, st.st_size, ct_type);
  while ((r = read(fd, buf, BUFSIZE)) > 0) response_.append(buf);
}

std::string Response::FindMime(std::string uri) {
  std::size_t dot_idx = uri.find_last_of(".");
  std::string mime_type = uri.substr(dot_idx, uri.size());

  if (!mime_type.compare(".html"))
    return "text/html";
  else if (!mime_type.compare(".jpg") || !mime_type.compare(".jpeg"))
    return "image/jpeg";
  else if (!mime_type.compare(".png"))
    return "image/png";
  else if (!mime_type.compare(".css"))
    return "text/css";
  else if (!mime_type.compare(".js"))
    return "text/javascript";
  else
    return "text/plain";
}

void Response::FillHeader(int status, long len, std::string type) {
  switch (status) {
    case 200:
      ComposeResponse(200, "OK", len, type);
      break;
    case 404:
      ComposeResponse(404, "Not Found", len, type);
      break;
    case 500:
      ComposeResponse(500, "Internal Server Error", len, type);
      break;
    default:
      break;
  }
  return;
}

void Response::ComposeResponse(int status, const char* status_text, int len,
                               std::string type) {
  response_ = "HTTP/1.1" + std::to_string(status) + " " + status_text +
              "\r\nContent-Length: " + std::to_string(len) +
              "\nContent-Type: " + type + "\r\n\r\n";
}

std::string Response::GetResponse(void) { return response_; }
