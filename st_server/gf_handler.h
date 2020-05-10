#ifndef __GF_HANDLER_H__
#define __GF_HANDLER_H__

#define BUFSIZE 4096
#define PATHLIM 1024

typedef struct http_handler_t http_handler_t;

struct http_handler_t {
    char directory[PATHLIM];
    ssize_t (*handle)(http_handler_t *, char *, int);
};

http_handler_t* gf_handler_create();

void gf_handler_set_dir(http_handler_t* handler, char* directory);

ssize_t gf_handler_do_HEAD(char* uri, int client_fd);

ssize_t gf_handler_do_GET(char* uri, int client_fd);

int is_file(const char *path);

FILE* _get_file_ptr(char* uri);

int _send_head(FILE* f, int client_fd);

int _send_file(FILE* f, int client_fd);

ssize_t gf_handler_handle(http_handler_t* handler, char* buf, int client_fd);

#endif