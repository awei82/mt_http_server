
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>

#define BUFSIZE 4096

typedef struct http_handler_t {
    char* directory;
} http_handler_t;

http_handler_t* http_handler_create(char* directory) {
    http_handler_t* handler = (http_handler_t *)malloc(sizeof(http_handler_t));
    handler->directory = directory;
    return handler;

    // self.request = request
    //     self.client_address = client_address
    //     self.server = server
    //     self.setup()
    //     try:
    //         self.handle()
    //     finally:
    //         self.finish()
}

/*
 * This class is instantiated for each request to be handled.  The
    constructor sets the instance variables request, client_address
    and server, and then calls the handle() method.  To implement a
    specific service, all you need to do is to derive a class which
    defines a handle() method.
  */

// setup 
// - open file buffers

// handle

// finish
// - close file buffers

ssize_t http_handler_do_HEAD(char* uri, int client_fd) {
    FILE* f = _get_file_obj(uri);
    if (f == NULL) {
        send(client_fd, "HTTP/1.1 404 Not Found\r\n\r\n", 30, 0);
    } else {
        _send_head(f, client_fd);
        close(f);
    }
    close(client_fd);
}

ssize_t http_handler_do_GET(char* uri, int client_fd) {
    FILE* f = _get_file_obj(uri);
    if (f == NULL) {
        send(client_fd, "HTTP/1.1 404 Not Found\r\n\r\n", 30, 0);
    } else {
        int r = _send_head(f, client_fd);
        if (r >= 0) {
            _send_file(f, client_fd);
        }
        close(f);
    }
    close(client_fd);
}

FILE* _get_file_obj(char* uri) {
    if( access(uri, F_OK) != -1 ) {
        // file exists
        return fopen(uri, "r");
    } else {
        // file doesn't exist
        return NULL;
    }
}

int _send_head(FILE* f) {
    struct stat buf;
    fstat(fd, &buf);
    size_t filelen = buf.st_size;

    if (filelen < 0) {
        perror("filesize check error");
        send(client_fd, "HTTP/1.1 500 Internal Server Error\r\n\r\n", 100, 0);
        return -1;
    } else {
        char buf[100];
        snprintf(buf, 100, "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n", filelen);
        send(client_fd, buf, 100, 0);
        return 0;
    }
}

int _send_file(FILE* f) {
    int nbytesSent;
    int nbytesRead;
    char data[BUFFER_SIZE];
    memset(data, 0, BUFFER_SIZE);

    off_t offset = 0;

    nbytesRead = pread(f, data, BUFFER_SIZE, offset);
    while ((nbytesRead) > 0) {
        offset += nbytesRead;
        nbytesSent = send(client_fd, data, BUFFER_SIZE, 0);
        if (nbytesSent < 0) {
            perror("nbytesSent < 0");
            gfs_abort(ctx);
            return NULL;
        }
        nbytesRead = pread(f, data, BUFFER_SIZE, offset);
    }
}


ssize_t http_handler(char* buf, int client_fd) {
    ssize_t bytes_sent = 0;

    // parse method
    //http_request_t* request = _split_request(request_str);

    // if (request->path[0] != 47) {    // check for '/' in beginning of path
    //     perror("# path does not start with '/'\n");
    //     gfs->status = GF_INVALID;
    //     nbytes = send(newsockfd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
    //     continue;
    // }

    // Parse Request method
    char* token = strtok(buf, " "); 
    char* method = token;

    // Get URI
    token = strtok(buf, " "); 
    char* uri = token;

    if (strncmp(method, "GET", 3) == 0) {
        return http_handler_do_GET(uri, client_fd);
    } else if (strncmp(method, "HEAD", 4) == 0) {
        return http_handler_do_HEAD(uri, client_fd);
    } else {
        perror("# Unknown HTTP method.\n");
        gfs->status = GF_INVALID;
        nbytes = send(newsockfd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
        continue;
    }

    
    close(fd);

    // END

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

/*
    HTTP request handler base class.
    The following explanation of HTTP serves to guide you through the
    code as well as to expose any misunderstandings I may have about
    HTTP (so you don't need to read the code to figure out I'm wrong
    :-).

    HTTP (HyperText Transfer Protocol) is an extensible protocol on
    top of a reliable stream transport (e.g. TCP/IP).  The protocol
    recognizes three parts to a request:

    1. One line identifying the request type and path
    2. An optional set of RFC-822-style headers
    3. An optional data part

    The headers and data are separated by a blank line.

    The first line of the request has the form

    <command> <path> <version>

    where <command> is a (case-sensitive) keyword such as GET or POST,
    <path> is a string containing path information for the request,
    and <version> should be the string "HTTP/1.0" or "HTTP/1.1".
    <path> is encoded using the URL encoding scheme (using %xx to signify
    the ASCII character with hex code xx).

    The specification specifies that lines are separated by CRLF but
    for compatibility with the widest range of clients recommends
    servers also handle LF.  Similarly, whitespace in the request line
    is treated sensibly (allowing multiple spaces between components
    and allowing trailing whitespace).

    Similarly, for output, lines ought to be separated by CRLF pairs
    but most clients grok LF characters just fine.

    If the first line of the request has the form

    <command> <path>

    (i.e. <version> is left out) then this is assumed to be an HTTP
    0.9 request; this form has no optional headers and data part and
    the reply consists of just the data.

    The reply form of the HTTP 1.x protocol again has three parts:

    1. One line giving the response code
    2. An optional set of RFC-822-style headers
    3. The data

    Again, the headers and data are separated by a blank line.

    The response code line has the form

    <version> <responsecode> <responsestring>

    where <version> is the protocol version ("HTTP/1.0" or "HTTP/1.1"),
    <responsecode> is a 3-digit response code indicating success or
    failure of the request, and <responsestring> is an optional
    human-readable string explaining what the response code means.

    This server parses the request and the headers, and then calls a
    function specific to the request type (<command>).  Specifically,
    a request SPAM will be handled by a method do_SPAM().  If no
    such method exists the server sends an error response to the
    client.  If it exists, it is called with no arguments:

    do_SPAM()

    Note that the request name is case sensitive (i.e. SPAM and spam
    are different requests).

    The various request details are stored in instance variables:

    - client_address is the client IP address in the form (host,
    port);
    - command, path and version are the broken-down request line;
    - headers is an instance of email.message.Message (or a derived
    class) containing the header information;
    - rfile is a file object open for reading positioned at the
    start of the optional input data part;
    - wfile is a file object open for writing.

    IT IS IMPORTANT TO ADHERE TO THE PROTOCOL FOR WRITING!
    
    The first thing to be written must be the response line.  Then
    follow 0 or more header lines, then a blank line, and then the
    actual data (if any).  The meaning of the header lines depends on
    the command executed by the server; in most cases, when data is
    returned, there should be at least one header line of the form
    Content-type: <type>/<subtype>
    where <type> and <subtype> should be registered MIME types,
    e.g. "text/html" or "text/plain".
    */



typedef struct http_request_t {
    char method[8];
    char path[PATHLIM];
    char version[8];
} http_request_t;

http_request_t* _split_request(char* request_str) {
    http_request_t *request = calloc(sizeof(http_request_t));

    int init_size = strlen(request);
	char delim[] = " ";

	char *ptr = strtok(request, delim);

    if (ptr == NULL) {
        perror("incorrect HTTP request - not enough args");
        request->method = NULL;
    } else {
        strncpy(request->method, ptr, 8);
        ptr = strtok(NULL, delim);
    }

    if (ptr == NULL) {
        perror("incorrect HTTP request - not enough args");
        request->path = NULL;
    } else {
        strncpy(request->path, ptr, PATHLIM);
        ptr = strtok(NULL, delim);
    }

    if (ptr == NULL) {
        request->version = NULL;
    } else {
        strncpy(request->version, ptr, 8);
    }

    return request;
}