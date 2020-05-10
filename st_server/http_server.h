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

typedef struct http_server_t http_server_t;
//typedef struct gfcontext_t gfcontext_t;

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

// /*
//  * Sets the third argument for calls to the handler callback.
//  */
// void http_server_set_handlerarg(http_server_t *server, void* arg);

/*
 * Starts the server.  Does not return.
 */
void http_server_serve(http_server_t *server);

// /*
//  * Sends to the client the Getfile header containing the appropriate 
//  * status and file length for the given inputs.  This function should
//  * only be called from within a callback registered http_server_set_handler.
//  */
// ssize_t gfs_sendheader(gfcontext_t *ctx, gfstatus_t status, size_t file_len);

// /*
//  * Sends size bytes starting at the pointer data to the client 
//  * This function should only be called from within a callback registered 
//  * with http_server_set_handler.  It returns once the data has been
//  * sent.
//  */
// ssize_t gfs_send(gfcontext_t *ctx, void *data, size_t size);

/*
 * Aborts the connection to the client associated with the input
 * gfcontext_t.
 */
void http_server_abort(int client_fd);

// /*
//  * this routine is used to handle the getfile request
//  */
// ssize_t getfile_handler(gfcontext_t *ctx, char *path, void* arg);

#endif
