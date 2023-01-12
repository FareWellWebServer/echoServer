#include "../include/Http.hpp"

Http::Http(char* request) : request_(std::string(request)) {}

Http::~Http(void) {}

void    Http::FillHeader(long len, std::string type) {
    char header[BUFSIZE];
    char status_text[40];
    switch (status_) {
        case 200:
            strcpy(status_text, "OK"); break;
        case 404:
            strcpy(status_text, "Not Found"); break;
        case 500:
        default:
            strcpy(status_text, "Internal Server Error"); break;
    }
    sprintf(header, HEADER_FORMAT, status_, status_text, len, type.c_str());
    header_ = std::string(header);
}

void    Http::Handle404(void) {
    status_ = 404;
	std::string t("text/html");
    FillHeader(sizeof(NOT_FOUND_CONTENT), t);
}

void    Http::Handle500(void) {
    status_ = 500;
	std::string t("text/html");
    FillHeader(sizeof(SERVER_ERROR_CONTENT), t);
}

void    Http::FindMime(char* ct_type, char* uri) {
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

void    Http::HttpHandler(void) {
    char safe_uri[BUFSIZE];
    char* local_uri;
    struct stat st;

	strcpy(safe_uri, "/index.html");

    local_uri = safe_uri + 1;
    if (stat(local_uri, &st) < 0) {
        perror("[WARN] No file found matching URI.\n");
        Handle404(); return;
    }

    fd_ = open(local_uri, O_RDONLY);
    if (fd_ < 0) {
        perror("[ERR] Failed to open file.\n");
        Handle500(); return;
    }

    int ct_len = st.st_size;
    char ct_type[40];
    FindMime(ct_type, local_uri);
    status_ = 200;
    FillHeader(ct_len, ct_type);
}

std::string	Http::GetHtml(int socket) const {
    if (status_ == 200) {
        char buf[BUFSIZE];
        int r;
        std::string res;
        while ((r = read(fd_, buf, BUFSIZE)) > 0) {
            write(socket, buf, r);
            res.append(buf);
        }
        return res;
    }
    else if (status_ == 404)
        return std::string(NOT_FOUND_CONTENT);
    else if (status_ == 500)
        return std::string(SERVER_ERROR_CONTENT);
    return std::string("Error");
}

std::string Http::GetHeader(void) const {
    return header_;
}
