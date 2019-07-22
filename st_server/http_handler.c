
#include <arpa/inet.h>
#include <ctype.h>

#define BUFSIZE 4096

typedef struct http_handler_t {
    char* directory;
} http_handler_t;

http_handler_t* http_handler_create(char* directory){
    http_handler_t* handler = (http_handler_t *)malloc(sizeof(http_handler_t));
    handler->directory = directory;
    return handler;
}


int http_handler_handle(http_handler_t* handler, struct sockaddr_in* cli_addr, int client_fd) {
    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);



    //// Receive data from socket
    memset(buf, 0, BUFSIZE);
    int nbytes;
    nbytes = recv(client_fd, buf, BUFSIZE, 0);
    if (nbytes == 0) {
        printf("# socket %d hung up\n", client_fd);
        close(client_fd);
    } else if (nbytes < 0) {
        perror("recv");
        close(client_fd);
    }

    
    //// Parse Request
    char* token = strtok(buf, " "); 

    // Get method
    char* method;

    if ((strncmp(token, "GET", 3) == 0) ||
        (strncmp(token, "HEAD", 3) == 0) ||
        (strncmp(token, "POST", 3) == 0) ||
        (strncmp(token, "PUT", 3) == 0) ||
        (strncmp(token, "DELETE", 3) == 0) ||
        (strncmp(token, "CONNECT", 3) == 0) ||
        (strncmp(token, "OPTIONS", 3) == 0) ||
        (strncmp(token, "TRACE", 3) == 0) ||
        (strncmp(token, "PATCH", 3) == 0)) {
        method = token;
    }
    else {
        perror("Invalid request method.");
        // send error code response.

    }

    // Get URI
    token = strtok(buf, " "); 
    char* uri = token;

    // Parse HTTP version, CRLF
    token = strtok(buf, " ");
    token = strtok(buf, " ");

    //// Parse Header Fields
    // Only parse Host field - ignore all others
    // TODO: enable case-insensitive parsing
    while (strncmp(token, "Host", 3) == 0) {
        token = strtok(buf, "\n");
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







}