#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#include "gf_handler.h"

#include "logger/logger.h"

http_handler_t* gf_handler_create() {
    http_handler_t* handler = (http_handler_t *)malloc(sizeof(http_handler_t));
    handler->handle = gf_handler_handle;
    return handler;
}

void gf_handler_set_dir(http_handler_t* handler, char* directory) {
    strncpy(handler->directory, directory, PATHLIM);
}

ssize_t gf_handler_do_HEAD(char* url, int client_fd) {
    //printf("do_HEAD\n");
    ssize_t bytes_sent = 0;
    FILE* f = _get_file_ptr(url);
    if (f == NULL) {
        char *err_str = "HTTP/1.0 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        bytes_sent = send(client_fd, err_str, strlen(err_str), 0);
    } else {
        bytes_sent = _send_head(f, client_fd);
        fclose(f);
    }
    return bytes_sent;
}

ssize_t gf_handler_do_GET(char* url, int client_fd) {
    //printf("do_GET\n");
    ssize_t bytes_sent = 0;

    // check if path is a directory
    DIR* dir = opendir(url);
    struct dirent *dir_ent;
    if (dir) {     
        char dir_buf[BUFSIZE] = {0};
        while ((dir_ent = readdir(dir)) != NULL) {
            strncat(dir_buf,  dir_ent->d_name, BUFSIZE - strlen(dir_buf) - 1);
            strncat(dir_buf, "\n", BUFSIZE - strlen(dir_buf) - 1);
        }
        char header_buf[100];
        snprintf(header_buf, 100, "HTTP/1.0 200 OK\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n", strlen(dir_buf));
        bytes_sent = send(client_fd, header_buf, strlen(header_buf), 0);
        bytes_sent += send(client_fd, dir_buf, strlen(dir_buf), 0);
    } else {
        FILE* f = _get_file_ptr(url);
        if (f == NULL) {
            char *err_str = "HTTP/1.0 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            bytes_sent = send(client_fd, err_str, strlen(err_str), 0);
        } else {
            int r = _send_head(f, client_fd);
            if (r >= 0) {
                bytes_sent = _send_file(f, client_fd);
            }
            fclose(f);
        }
    }
    return bytes_sent;
}

int is_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return (stat(path, &path_stat) == 0) && S_ISREG(path_stat.st_mode);
}

FILE* _get_file_ptr(char* url) {
    if ( is_file(url) ) {
        return fopen(url, "r");
    }
    log_info("file %s doesn't exist", url);
    return NULL;
}

int _send_head(FILE* f, int client_fd) {
    //printf("_send_head\n");
    ssize_t bytes_sent = 0;
    struct stat buf;
    fstat(fileno(f), &buf);
    off_t filelen = buf.st_size;

    if (filelen < 0) {
        log_error("filesize check error");
        char *err_str = "HTTP/1.0 500 Internal Server Error\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        bytes_sent = send(client_fd, err_str, strlen(err_str), 0);
    } else {
        char header_buf[100];
        snprintf(header_buf, 100, "HTTP/1.0 200 OK\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n", filelen);
        bytes_sent = send(client_fd, header_buf, strlen(header_buf), 0);
    }
    return bytes_sent;
}

int _send_file(FILE* f, int client_fd) {
    int nbytesSent = 0;
    int nbytesRead;
    char data[BUFSIZE];
    memset(data, 0, BUFSIZE);

    nbytesRead = fread(&data, sizeof(char), BUFSIZE, f);
    while ((nbytesRead) > 0) {
        nbytesSent += send(client_fd, data, nbytesRead, 0);
        if (nbytesSent < 0) {
            perror("send error\n");
            return nbytesSent;
        }
        nbytesRead = fread(&data, sizeof(char), BUFSIZE, f);
    }
    return nbytesSent;
}


ssize_t gf_handler_handle(http_handler_t* handler, char* buf, int client_fd) {  
    clock_t begin = clock();
    ssize_t bytes_sent = 0;

    // Parse request method
    char* method = strtok(buf, " "); 

    // Parse URL
    char* url = strtok(NULL, " "); 
 
    // strtok will set url to NULL if url is missing
    if (url == NULL) {
        char *err_str = "HTTP/1.0 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        bytes_sent = send(client_fd, err_str, strlen(err_str), 0);
        log_error("Bad request");
    // strip path traversal attempts
    } else if (strstr(url, "../") != NULL) {
        char *err_str = "HTTP/1.0 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        bytes_sent = send(client_fd, err_str, strlen(err_str), 0);
        log_error("**directory traversal attempt**");
    } else {
        char full_url[PATHLIM] = {0};
        strncpy(full_url, handler->directory, PATHLIM-1);
        strncat(full_url, url, PATHLIM-strlen(full_url)-1);

        // Handle request
        if (strncmp(method, "GET", 3) == 0) {
            bytes_sent = gf_handler_do_GET(full_url, client_fd);
        } else if (strncmp(method, "HEAD", 4) == 0) {
            bytes_sent = gf_handler_do_HEAD(full_url, client_fd);
        } else {
            char *err_str = "HTTP/1.0 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            bytes_sent = send(client_fd, err_str, strlen(err_str), 0);
            log_error("# Unknown HTTP method.");
        }
    }

    // Finish - close fd
    clock_t end = clock();
    clock_t seconds_taken = (end - begin)*1000 / CLOCKS_PER_SEC;
    log_info("%lu bytes sent to socket %d. request handled in %dms.", bytes_sent, client_fd, seconds_taken);
    return bytes_sent;
}
