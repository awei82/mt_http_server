# Simple multithreaded HTTP Server
Only implements HTTP/1.1 GET and HEAD methods


## Main

args:
- port
- bind address / interface
- directory?
- help
- config-file?

HandlerClass=BaseHTTPRequestHandler,
ServerClass=ThreadingHTTPServer,
protocol="HTTP/1.0"
port=8000, 
bind=""

server_address = (bind, port)

HandlerClass.protocol_version = protocol

 with ServerClass(server_address, HandlerClass) as httpd:
         sa = httpd.socket.getsockname()
         serve_message = "Serving HTTP on {host} port {port} (http://{host}:{port}/) ..."
         print(serve_message.format(host=sa[0], port=sa[1]))
         httpd.serve_forever()


## HTTPServer

init / create
set port
set address
set max pending
set handler
set handler_arg

serve
send header
send (bytes)
abort

https://github.com/python/cpython/blob/d1524148cd08f00c0b7c1dfdf698bf96c246350d/Lib/socketserver.py#L390

Base class for various socket-based server classes.
    Defaults to synchronous IP stream (i.e., TCP).
    Methods for the caller:
    - __init__(server_address, RequestHandlerClass, bind_and_activate=True)
    - serve_forever(poll_interval=0.5)
    - shutdown()
    - handle_request()  # if you don't use serve_forever()
    - fileno() -> int   # for selector
    Methods that may be overridden:
    - server_bind()
    - server_activate()
    - get_request() -> request, client_address
    - handle_timeout()
    - verify_request(request, client_address)
    - process_request(request, client_address)
    - shutdown_request(request)
    - close_request(request)
    - handle_error()
    Methods for derived classes:
    - finish_request(request, client_address)
    Class variables that may be overridden by derived classes or
    instances:
    - timeout
    - address_family
    - socket_type
    - request_queue_size (only for stream sockets)
    - allow_reuse_address
    Instance variables:
    - server_address
    - RequestHandlerClass
    - socket
bind
activate / listen
serve_forever
- handle_request_noblock
  - get_request (accept)
  - process_request - used for multithreading?
    - finish_request
      - instantiate handler
  - handle_error
  - shutdown_request

## Handler

https://github.com/python/cpython/blob/d1524148cd08f00c0b7c1dfdf698bf96c246350d/Lib/http/server.py#L627

Simple HTTP request handler with GET and HEAD commands.
This serves files from the current directory and any of its
subdirectories.  The MIME type for files is determined by
calling the .guess_type() method.
The GET and HEAD requests are identical except that the HEAD
request omits the actual contents of the file.

init(args, directory, kwargs)
do_GET
do_HEAD
send_head - sends the response code and MIME headers
list_directory
translate_path - decodes URL encoding
collapse_path - needed???

https://github.com/python/cpython/blob/d1524148cd08f00c0b7c1dfdf698bf96c246350d/Lib/http/server.py#L147


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
    and <version> should be the string HTTP/1.0 or HTTP/1.1.
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
    where <version> is the protocol version (HTTP/1.0 or HTTP/1.1),
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
    e.g. text/html or text/plain.

handle
handle_one_request
- read data
- parse request
- do method request
- write flush
parse_request
send_error
send_response
send_header

https://github.com/python/cpython/blob/d1524148cd08f00c0b7c1dfdf698bf96c246350d/Lib/socketserver.py#L696
init
- setup
- handle
- finish
