#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__


/*
 * http_server is a server library implementing the GET and HEAD methods of the HTTP/1.1 protocol.
 * https://www.w3.org/Protocols/rfc2616/rfc2616.html
 */

#define MAX_REQUEST_LEN 4096
#define BUFSIZE 4096

typedef struct http_server_t http_server_t;
typedef struct gfcontext_t gfcontext_t;

/* 
 * This function must be the first one called as part of 
 * setting up a server.  It returns a http_server_t handle which should be
 * passed into all subsequent library calls of the form http_server_*.  It
 * is not needed for the gfs_* call which are intended to be called from
 * the handler callback.
 */
http_server_t *http_server_create(struct sockaddr_in *sock_addr);

/*
 * Sets the port at which the server will listen for connections.
 */
void http_server_set_port(http_server_t *gfs, unsigned short port);

/*
 * Sets the address at which the server will bind to.
 */
void http_server_set_address(http_server_t *gfs, struct sockaddr_in serv_addr);

/*
 * Sets the maximum number of pending connections which the server
 * will tolerate before rejecting connection requests.
 */
void http_server_set_maxpending(http_server_t *gfs, int max_npending);


/*
 * Sets the handler callback, a function that will be called for each each
 * request.  As arguments, this function receives:
 * - a gfcontext_t handle which it must pass into the gfs_* functions that 
 * 	 it calls as it handles the response.
 * - the requested path
 * - the pointer specified in the http_server_set_handlerarg option.
 * The handler should only return a negative value to signal an error.
 */
void http_server_set_handler(http_server_t *gfs, ssize_t (*handler)(gfcontext_t *, char *, void*));

/*
 * Sets the third argument for calls to the handler callback.
 */
void http_server_set_handlerarg(http_server_t *gfs, void* arg);

/*
 * Starts the server.  Does not return.
 */
void http_server_serve(http_server_t *gfs);
// serve_forever?

/*
 * Sends to the client the Getfile header containing the appropriate 
 * status and file length for the given inputs.  This function should
 * only be called from within a callback registered http_server_set_handler.
 */
ssize_t gfs_sendheader(gfcontext_t *ctx, gfstatus_t status, size_t file_len);

/*
 * Sends size bytes starting at the pointer data to the client 
 * This function should only be called from within a callback registered 
 * with http_server_set_handler.  It returns once the data has been
 * sent.
 */
ssize_t gfs_send(gfcontext_t *ctx, void *data, size_t size);

/*
 * Aborts the connection to the client associated with the input
 * gfcontext_t.
 */
void gfs_abort(gfcontext_t *ctx);

/*
 * this routine is used to handle the getfile request
 */
ssize_t getfile_handler(gfcontext_t *ctx, char *path, void* arg);

#endif
