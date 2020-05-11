#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "gf_handler.h"

/*
 * http_server is a server library implementing 
 * a simplified version of the GET and HEAD methods 
 * of the HTTP/1.1 protocol.
 * https://www.w3.org/Protocols/rfc2616/rfc2616.html
 */

#define BUFSIZE 4096
#define QUEUESIZE 256

typedef struct http_server_t http_server_t;

/* 
 * This function must be the first one called as part of 
 * setting up a server. It returns a http_server_t handle which should be
 * passed into all subsequent library calls of the form http_server_*.
 */
http_server_t *http_server_create();

/*
 * Sets the port at which the server will listen for connections.
 */
void http_server_set_port(http_server_t *server, unsigned short port);

/*
 * Sets the address at which the server will bind to.
 */
void http_server_set_address(http_server_t *server, char* serv_addr);

/*
 * Sets the maximum number of pending connections which the server
 * will tolerate before rejecting connection requests.
 */
void http_server_set_maxpending(http_server_t *server, int max_npending);


/*
 * Sets the handler callback
 */
void http_server_set_handler(http_server_t *server, http_handler_t *handler);

/*
 * Starts the server.  Does not return.
 */
void http_server_serve(http_server_t *server);

typedef struct worker_args_t worker_args_t;

typedef struct worker_args_t {
    long thread_id;
    http_handler_t *handler;
} worker_args_t;

void *http_server_worker(void *args);

#endif
