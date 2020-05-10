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
    http_handler_t *handler;
    void* handlerarg;
} http_server_t;


http_server_t* http_server_create(){
    http_server_t *server = (http_server_t *)malloc(sizeof(http_server_t));
    memset(&(server->serv_addr), 0, sizeof(server->serv_addr));

    server->serv_addr.sin_family = AF_INET;
    server->serv_addr.sin_addr.s_addr = INADDR_ANY;
    server->serv_addr.sin_port = htons(6200);

    server->maxpending = 1;
    server->handler = NULL;
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
    printf("# Listener socket %d bound to %s:%d.\n", listenerfd, inet_ntoa(server->serv_addr.sin_addr) , ntohs(server->serv_addr.sin_port));

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
        printf("- Accepted a connection from %s on socket %d\n", clientIP, newsockfd);

        // Receive the request
        memset(buf, 0, BUFSIZE);
        int nbytes;
        nbytes = recv(newsockfd, buf, BUFSIZE, 0);
        if (nbytes == 0) {
            printf("socket %d hung up\n", newsockfd);
            close(newsockfd);
            continue;
        } else if (nbytes < 0) {
            perror("recv");
            close(newsockfd);
            continue;
        }
        printf("received request: %s\n", strtok(buf, "\n"));

        // handle the request
        server->handler->handle(server->handler, buf, newsockfd);
    }
}

