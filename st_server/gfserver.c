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





http_server_t* gfserver_create(struct sockaddr_in *sock_addr){
    http_server_t *server = (http_server_t *)malloc(sizeof(http_server_t));
    server->sock_addr = *sock_addr;
    server->maxpending = 1;
    // gfs->handler = NULL;
    // gfs->handlerarg = NULL;
    // gfs->status = GF_INVALID;
    return server;
}




void gfserver_set_handlerarg(gfserver_t *gfs, void* arg){
    gfs->handlerarg = arg;
}

void gfserver_set_handler(gfserver_t *gfs, ssize_t (*handler)(gfcontext_t *, char *, void*)){
    gfs->handler = handler;
}

void gfserver_set_maxpending(gfserver_t *gfs, int max_npending){
    gfs->maxpending = max_npending;
}

void gfserver_set_port(gfserver_t *gfs, unsigned short port){
    gfs->port = port;
}

void gfserver_serve(gfserver_t *gfs){
    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);

    int listenerfd, newsockfd;
    socklen_t clilen;

    listenerfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerfd < 0)
        perror("ERROR opening socket");

    int yes = 1;
    setsockopt(listenerfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
   
    struct sockaddr_in serv_addr, cli_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(gfs->port);
    if (bind(listenerfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        perror("ERROR on binding");
    //printf("# Listener socket %d bound to TCP port %d.\n", listenerfd, gfs->port);

    //printf("# Server is active. Listening for connections...\n");
    if (listen(listenerfd, gfs->maxpending) == -1) {
        perror("listen");
        exit(3);
    }

    clilen = sizeof cli_addr;
    //char *clientIP;

    while(1) {
        newsockfd = accept(listenerfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            continue;
        }
        //clientIP = inet_ntoa(cli_addr.sin_addr);
        //printf("# Accepted a connection from %s on socket %d\n", clientIP, newsockfd);

        //// recv
        memset(buf, 0, BUFSIZE);
        int nbytes;
        nbytes = recv(newsockfd, buf, BUFSIZE, 0);
        if (nbytes == 0) {
            //printf("# socket %d hung up\n", newsockfd);
            close(newsockfd);
            continue;
        } else if (nbytes < 0) {
            perror("recv");
            close(newsockfd);
            continue;
        }

        //// parse request
        char* ptr = buf;
        if (strncmp(ptr, "GETFILE", 7) != 0) {
            perror("# Unknown <scheme> request.\n");
            gfs->status = GF_INVALID;
            nbytes = send(newsockfd, "GETFILE INVALID\r\n\r\n", 20, 0);
            continue;
        }

        ptr += 8;
        if (strncmp(ptr, "GET", 3) != 0) {
            perror("# Unknown GETFILE method.\n");
            gfs->status = GF_INVALID;
            nbytes = send(newsockfd, "GETFILE INVALID\r\n\r\n", 20, 0);
            continue;
        }

        ptr+= 4;
        char* ptr2 = ptr;
        int pathlen = 0;
        while ((*ptr2 != 13) && (*ptr2 != 32)) { //while not ("\r" or " ")
            pathlen++;
            ptr2++;
        }
        char path[PATHLIM];
        memset(path, '\0', PATHLIM);
        strncpy(path, ptr, pathlen);
        //printf("# path: %s\n", path);
        
        if (path[0] != 47) {    // check for '/' in beginning of path
            perror("# path does not start with '/'\n");
            gfs->status = GF_INVALID;
            nbytes = send(newsockfd, "GETFILE INVALID\r\n\r\n", 20, 0);
            continue;
        }
        //printf("# received: %s", buf);

        gfcontext_t *ctx = (gfcontext_t *)malloc(sizeof(gfcontext_t));
        ctx->sockfd = newsockfd;

        gfs->handler(ctx, path, gfs->handlerarg);
        free(ctx);
        close(newsockfd);
    }
}