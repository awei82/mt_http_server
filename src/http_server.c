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
#include <pthread.h>

#include "http_server.h"
#include "gf_handler.h"

#include "logger/logger.h"
#include "logger/logger.c"

// FIFO queue implementation
struct client_ctx {
    int fd;
    char request[PATHLIM];
    char* clientIP;
} client_ctx;
struct client_ctx client_queue[QUEUESIZE];

int queueBack = 0; // queueBack tracks where handler will insert to queue
int queueFront = 0; // queueFront tracks where workers will remove from queue
int queueCount = 0; // queueCount tracks current # of items in queue

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;	
// condition var to control handler adding to the queues
pthread_cond_t cv_in = PTHREAD_COND_INITIALIZER;
// condition var to control workers removing from queues
pthread_cond_t cv_out = PTHREAD_COND_INITIALIZER;


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
    int listenerfd;
    socklen_t clilen;

    listenerfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerfd < 0)
        perror("ERROR opening socket");

    int yes = 1;
    setsockopt(listenerfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    // bind
    if (bind(listenerfd, (struct sockaddr *) &server->serv_addr, sizeof(server->serv_addr)) < 0)
        perror("ERROR on binding");
    log_info("# Listener socket %d bound to %s:%d.\n", listenerfd, inet_ntoa(server->serv_addr.sin_addr) , ntohs(server->serv_addr.sin_port));

    log_info("Server is active. Listening for connections...\n", clientIP, newsockfd);
    if (listen(listenerfd, server->maxpending) == -1) {
        perror("listen");
        exit(3);
    }

    struct sockaddr_in cli_addr;
    clilen = sizeof cli_addr; 
    char *clientIP;

    // Serve
    while(1) {
        // ACCEPT
        int newsockfd = accept(listenerfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            continue;
        }
        clientIP = inet_ntoa(cli_addr.sin_addr);
        log_info("accepted a connection from %s on socket %d\n", clientIP, newsockfd);

        // Receive the request
        memset(buf, 0, BUFSIZE);
        int nbytes;
        nbytes = recv(newsockfd, buf, BUFSIZE, 0);          // where everything hangs
        if (nbytes == 0) {
            log_info("socket %d hung up\n", newsockfd);
            close(newsockfd);
            continue;
        } else if (nbytes < 0) {
            perror("recv");
            close(newsockfd);
            continue;
        }
        log_info("received request: %s\n", strtok(buf, "\n"));

        /* handle the request - multithreaded */
        // get lock, wait for queue to be available
        pthread_mutex_lock(&queue_mutex);
        while (queueCount >= QUEUESIZE) {
            printf("### queue maxed. handler waiting...\n");
            pthread_cond_wait(&cv_in, &queue_mutex);
        }
        queueCount++;

        client_queue[queueBack].fd = newsockfd;
        strncpy(client_queue[queueBack].request, buf, PATHLIM);
        client_queue[queueBack].clientIP = clientIP;

        // increment queueBack pointer
        if (queueBack == (QUEUESIZE - 1)) {
            queueBack = 0;
        } else {
            queueBack ++;
        }
        // release lock
        pthread_mutex_unlock(&queue_mutex);
        pthread_cond_signal(&cv_out);

        //server->handler->handle(server->handler, buf, newsockfd);
    }
}

void *http_server_worker(void *args) {
    //long tid = ((worker_args_t *)args)->thread_id;
  	//printf("# Thread #%ld created.\n", tid);

    http_handler_t *handler = ((worker_args_t *)args)->handler;
    while (1) {
        // lock and wait for a request on the queue
        pthread_mutex_lock(&queue_mutex);
		while (queueCount == 0) {
			//printf("# Thread #%ld: queueCount = 0. Waiting for work...\n", tid);
			pthread_cond_wait(&cv_out, &queue_mutex);
		}
		queueCount--;

        int client_fd;
		char *requestbuf;
        client_fd = client_queue[queueFront].fd;
        requestbuf = client_queue[queueFront].request;
        char *clientIP = client_queue[queueFront].clientIP;

        // point queueFront to next queue position
		if (queueFront == (QUEUESIZE-1)) {
			queueFront = 0;
		} else {
			queueFront ++;
		}
		pthread_mutex_unlock(&queue_mutex);
		pthread_cond_signal(&cv_in);

        // do work
        handler->handle(handler, requestbuf, client_fd);

        // close socket
        log_info("Closing connection to %s on socket %d", clientIP, client_fd);
        close(client_fd);
    }
}