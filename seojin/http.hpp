#ifndef HTTP_HPP
#define HTTP_HPP

#include "server.hpp"
#include "robust_inout.hpp"


namespace http
{

void doit(int fd);
void read_requesthdrs(rio_t *rp);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
// void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
int parse_uri(char *uri, char *filename, char *cgiargs);

};


#endif