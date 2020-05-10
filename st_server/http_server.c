#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include "http_server.h"
#include "http_status.h"
#include "gf_handler.h"

typedef struct http_server_t {
    struct sockaddr_in serv_addr;

    int maxpending;
    //ssize_t (*handler)(char *, int);
    http_handler_t *handler;
    void* handlerarg;
    //server_status_t status;
} http_server_t;

// typedef struct gfcontext_t {
//     int sockfd;
// } gfcontext_t;

http_server_t* http_server_create(){
    http_server_t *server = (http_server_t *)malloc(sizeof(http_server_t));
    memset(&(server->serv_addr), 0, sizeof(server->serv_addr));

    server->serv_addr.sin_family = AF_INET;
    server->serv_addr.sin_addr.s_addr = INADDR_ANY;
    server->serv_addr.sin_port = htons(6200);

    server->maxpending = 1;
    server->handler = NULL;
    // server->handlerarg = NULL;
    //server->status = GF_INVALID;
    return server;
}

void http_server_set_port(http_server_t *server, unsigned short port) {
    server->serv_addr.sin_port = htons(port);
}

void http_server_set_address(http_server_t *server, char* serv_addr) {
    inet_pton(server->serv_addr.sin_family, serv_addr, &(server->serv_addr.sin_addr));
}

void http_server_set_maxpending(http_server_t *server, int max_npending) {
    server->maxpending = max_npending;
}

void http_server_set_handler(http_server_t *server, http_handler_t *handler) {
    server->handler = handler;
}

// void http_server_set_handlerarg(http_server_t *server, void* arg){
//     server->handlerarg = arg;
// }



void http_server_serve(http_server_t *server){
    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);

    // set up listening port
    int listenerfd, newsockfd;
    socklen_t clilen;

    listenerfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerfd < 0)
        perror("ERROR opening socket");

    int yes = 1;
    setsockopt(listenerfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    
    if (bind(listenerfd, (struct sockaddr *) &server->serv_addr, sizeof(server->serv_addr)) < 0)
        perror("ERROR on binding");
    printf("# Listener socket %d bound to TCP port %d.\n", listenerfd, ntohs(server->serv_addr.sin_port));

    printf("# Server is active. Listening for connections...\n");
    if (listen(listenerfd, server->maxpending) == -1) {
        perror("listen");
        exit(3);
    }

    struct sockaddr_in cli_addr;
    clilen = sizeof cli_addr; 

    // serve
    while(1) {
        // ACCEPT
        newsockfd = accept(listenerfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            continue;
        }
        char *clientIP;
        clientIP = inet_ntoa(cli_addr.sin_addr);
        printf("# Accepted a connection from %s on socket %d\n", clientIP, newsockfd);

        // Receive the request
        memset(buf, 0, BUFSIZE);
        int nbytes;
        nbytes = recv(newsockfd, buf, BUFSIZE, 0);
        if (nbytes == 0) {
            printf("# socket %d hung up\n", newsockfd);
            close(newsockfd);
            continue;
        } else if (nbytes < 0) {
            perror("recv");
            close(newsockfd);
            continue;
        }
        printf("# received request %s", buf);

        // handle the request
        server->handler->handle(server->handler, buf, newsockfd);
        //close(newsockfd);
        // END





        // // parse request
        // char* ptr = buf;
        // if (strncmp(ptr, "GET", 7) != 0) {
        //     perror("# Unknown <scheme> request.\n");
        //     gfs->status = GF_INVALID;
        //     nbytes = send(newsockfd, "GETFILE INVALID\r\n\r\n", 20, 0);
        //     continue;
        // }

        // ptr += 8;
        // if (strncmp(ptr, "GET", 3) != 0) {
        //     perror("# Unknown GETFILE method.\n");
        //     gfs->status = GF_INVALID;
        //     nbytes = send(newsockfd, "GETFILE INVALID\r\n\r\n", 20, 0);
        //     continue;
        // }

        // ptr+= 4;
        // char* ptr2 = ptr;
        // int pathlen = 0;
        // while ((*ptr2 != 13) && (*ptr2 != 32)) { //while not ("\r" or " ")
        //     pathlen++;
        //     ptr2++;
        // }
        // char path[PATHLIM] = {0};
        // strncpy(path, ptr, PATHLIM);
        // //printf("# path: %s\n", path);
        
        // if (path[0] != 47) {    // check for '/' in beginning of path
        //     perror("# path does not start with '/'\n");
        //     gfs->status = GF_INVALID;
        //     nbytes = send(newsockfd, "GETFILE INVALID\r\n\r\n", 20, 0);
        //     continue;
        // }
        // //printf("# received: %s", buf);

        // gfcontext_t *ctx = (gfcontext_t *)malloc(sizeof(gfcontext_t));
        // ctx->sockfd = newsockfd;

        // gfs->handler(ctx, path, gfs->handlerarg);
        // free(ctx);
        // close(newsockfd);

    }
}


void http_server_abort(int client_fd){
    printf("# http server abort\n");
    close(client_fd);
}

