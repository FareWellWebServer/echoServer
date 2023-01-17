#include "Response.hpp"

Response::Response(char* request) : request_(std::string(request)) {}

Response::~Response(void) {}

void	Response::ResponseHandler(void) {
    if (request_.size() == 0) {
        std::cerr << "[ERROR] Failed to read request" << std::endl;
        Handle500();
        return;
    }

    std::size_t method_end_idx = request_.find_first_of(" ");
    std::cout << "method_end_idx: " << method_end_idx << std::endl;
    std::string method = request_.substr(0, method_end_idx);
    std::size_t uri_end_idx = request_.find_first_of(" ", method_end_idx + 1);
    std::cout << "uri_end_idx: " << uri_end_idx << std::endl;
    std::string uri = request_.substr(method_end_idx + 1, uri_end_idx - method_end_idx - 1);

    // char* method = strtok(const_cast<char*>(request_.c_str()), " ");
    // char* uri = strtok(NULL, " ");
    if (method.size() == 0 || uri.size() == 0) {
        std::cerr << "[ERROR] Failed to identify method, URI" << std::endl;
        Handle500();
        return;
    }

    std::cout << "[INFO] Request: method=" << method << ", URI=" << uri << std::endl;

    char safe_uri[BUFSIZE];
    char* local_uri;
    struct stat st;

    strcpy(safe_uri, uri);
    if (!strcmp(safe_uri, "/"))
		strcpy(safe_uri, "/index.html");

    local_uri = safe_uri + 1;
    if (stat(local_uri, &st) < 0) {
        std::cerr << "[WARN] No file found matching URI" << std::endl;
        Handle404();
        return ;
    }

    Handle200(st.st_size, local_uri);
    return ;
}

void	Response::Handle200(int ct_len, char* local_uri) {
    char ct_type[40];
    int r;
    int fd = open(local_uri, O_RDONLY);
    char buf[BUFSIZE];

    FindMime(ct_type, local_uri);
    FillHeader(200, ct_len, ct_type);
    while ((r = read(fd, buf, BUFSIZE)) > 0)
        response_.append(buf);
}

void	Response::Handle404(void) {
    char uri[9] = "404.html";
    struct stat st;
    char ct_type[40];
    int r;
    char buf[BUFSIZE];
    int fd = open(uri, O_RDONLY);

    stat(uri, &st);
    FindMime(ct_type, uri);
    FillHeader(404, st.st_size, ct_type);
    while ((r = read(fd, buf, BUFSIZE)) > 0)
        response_.append(buf);
}

void	Response::Handle500(void) {
    char uri[9] = "500.html";
    struct stat st;
    char ct_type[40];
    int r;
    char buf[BUFSIZE];
    int fd = open(uri, O_RDONLY);

    stat(uri, &st);
    FindMime(ct_type, uri);
    FillHeader(500, st.st_size, ct_type);
    while ((r = read(fd, buf, BUFSIZE)) > 0)
        response_.append(buf);
}

void	Response::FindMime(char* ct_type, char* uri) {
    char *ext = strrchr(uri, '.');

    if (!strcmp(ext, ".html"))
		strcpy(ct_type, "text/html");
    else if (!strcmp(ext, ".jpg") || !strcmp(ext, ".jpeg"))
        strcpy(ct_type, "image/jpeg");
    else if (!strcmp(ext, ".png"))
        strcpy(ct_type, "image/png");
    else if (!strcmp(ext, ".css"))
        strcpy(ct_type, "text/css");
    else if (!strcmp(ext, ".js"))
        strcpy(ct_type, "text/javascript");
    else strcpy(ct_type, "text/plain");
}

void	Response::FillHeader(int status, long len, std::string type) {
    char header[BUFSIZE];
    char status_text[42];

    switch (status) {
        case 200:
            strcpy(status_text, "OK");
			break;
        case 404:
            strcpy(status_text, "Not Found");
			break;
        case 500:
            strcpy(status_text, "Internal Server Error");
			break;
        default:
			break;
    }
    sprintf(header, HEADER_FORMAT, status, status_text, len, type.c_str());
	// stream >> status_str;
	// response_ = "HTTP/1.1" + status_str + " " + status_text + "\r\nContent-Length: " \
				//+ len_str + "\nContent-Type: " + type + "\r\n";
    response_ = std::string(header);
}

std::string	Response::GetResponse(void){
    return response_;
}
