/*
 * HTTP status codes and reason phrases
 * Status codes from the following RFCs are all observed:
 *   RFC 7231: Hypertext Transfer Protocol (HTTP/1.1), obsoletes 2616
 *   RFC 6585: Additional HTTP Status Codes
 *   RFC 3229: Delta encoding in HTTP
 *   RFC 4918: HTTP Extensions for WebDAV, obsoletes 2518
 *   RFC 5842: Binding Extensions to WebDAV
 *   RFC 7238: Permanent Redirect
 *   RFC 2295: Transparent Content Negotiation in HTTP
 *   RFC 2774: An HTTP Extension Framework
 *   RFC 7540: Hypertext Transfer Protocol Version 2 (HTTP/2)
 * 
 * Codes gathered from:
 * https://github.com/python/cpython/blob/d1524148cd08f00c0b7c1dfdf698bf96c246350d/Lib/http/__init__.py
*/

typedef int http_status_t;

// informational
#define  CONTINUE 100 //'Continue', 'Request received, please continue'
#define  SWITCHING_PROTOCOLS 101 // 'Switching Protocols', 'Switching to new protocol; obey Upgrade header')
#define  PROCESSING 102 // 'Processing'

// success
#define  OK 200 // 'OK', 'Request fulfilled, document follows'
#define  CREATED 201 // 'Created', 'Document created, URL follows'
#define  ACCEPTED 202 // 'Accepted','Request accepted // processing continues off-line'
#define  NON_AUTHORITATIVE_INFORMATION 203 //'Non-Authoritative Information' // 'Request fulfilled from cache'
#define  NO_CONTENT 204 // 'No Content', 'Request fulfilled, nothing follows'
#define  RESET_CONTENT 205 // 'Reset Content', 'Clear input form for further input'
#define  PARTIAL_CONTENT 206 // 'Partial Content', 'Partial content follows'
#define  MULTI_STATUS 207 // 'Multi-Status'
#define  ALREADY_REPORTED 208 // 'Already Reported'
#define  IM_USED 226 // 'IM Used'

// redirection
#define  MULTIPLE_CHOICES 300 // 'Multiple Choices','Object has several resources -- see URI list'
#define  MOVED_PERMANENTLY 301 // 'Moved Permanently','Object moved permanently -- see URI list'
#define  FOUND 302 // 'Found', 'Object moved temporarily -- see URI list'
#define  SEE_OTHER 303 // 'See Other', 'Object moved -- see Method and URL list'
#define  NOT_MODIFIED 304 // 'Not Modified','Document has not changed since given time'
#define  USE_PROXY 305 // 'Use Proxy','You must use proxy specified in Location to access this resource'
#define  TEMPORARY_REDIRECT 307 // 'Temporary Redirect','Object moved temporarily -- see URI list'
#define  PERMANENT_REDIRECT 308 // 'Permanent Redirect','Object moved temporarily -- see URI list'

// client error
#define  BAD_REQUEST 400 // 'Bad Request','Bad request syntax or unsupported method'
#define  UNAUTHORIZED 401 // 'Unauthorized','No permission -- see authorization schemes'
#define  PAYMENT_REQUIRED 402 // 'Payment Required','No payment -- see charging schemes'
#define  FORBIDDEN 403 // 'Forbidden','Request forbidden -- authorization will not help'
#define  NOT_FOUND 404 // 'Not Found','Nothing matches the given URI'
#define  METHOD_NOT_ALLOWED 405 // 'Method Not Allowed','Specified method is invalid for this resource'
#define  NOT_ACCEPTABLE 406 // 'Not Acceptable','URI not available in preferred format'
#define  PROXY_AUTHENTICATION_REQUIRED 407 //'Proxy Authentication Required', 'You must authenticate with this proxy before proceeding'
#define  REQUEST_TIMEOUT 408 // 'Request Timeout','Request timed out; try again later'
#define  CONFLICT 409 // 'Conflict', 'Request conflict'
#define  GONE 410 // 'Gone','URI no longer exists and has been permanently removed'
#define  LENGTH_REQUIRED 411 // 'Length Required', 'Client must specify Content-Length'
#define  PRECONDITION_FAILED 412 // 'Precondition Failed', 'Precondition in headers is false'
#define  REQUEST_ENTITY_TOO_LARGE 413 // 'Request Entity Too Large','Entity is too large'
#define  REQUEST_URI_TOO_LONG 414 // 'Request-URI Too Long','URI is too long'
#define  UNSUPPORTED_MEDIA_TYPE 415 // 'Unsupported Media Type', 'Entity body in unsupported format'
#define  REQUESTED_RANGE_NOT_SATISFIABLE 416 // 'Requested Range Not Satisfiable', 'Cannot satisfy request range'
#define  EXPECTATION_FAILED 417 // 'Expectation Failed', 'Expect condition could not be satisfied'
#define  MISDIRECTED_REQUEST 421 // 'Misdirected Request', 'Server is not able to produce a response'
#define  UNPROCESSABLE_ENTITY 422 // 'Unprocessable Entity'
#define  LOCKED 423 // 'Locked'
#define  FAILED_DEPENDENCY 424 // 'Failed Dependency'
#define  UPGRADE_REQUIRED 426 // 'Upgrade Required'
#define  PRECONDITION_REQUIRED 428 // 'Precondition Required', 'The origin server requires the request to be conditional'
#define  TOO_MANY_REQUESTS 429 // 'Too Many Requests', 'The user has sent too many requests in a given amount of time "rate limiting"'
#define  REQUEST_HEADER_FIELDS_TOO_LARGE 431 // 'Request Header Fields Too Large', 'The server is unwilling to process the request because its header fields are too large'

// server errors
#define  INTERNAL_SERVER_ERROR 500 // 'Internal Server Error', 'Server got itself in trouble'
#define  NOT_IMPLEMENTED 501 // 'Not Implemented', 'Server does not support this operation'
#define  BAD_GATEWAY 502 // 'Bad Gateway', 'Invalid responses from another server/proxy'
#define  SERVICE_UNAVAILABLE 503 // 'Service Unavailable', 'The server cannot process the request due to a high load'
#define  GATEWAY_TIMEOUT 504 // 'Gateway Timeout', 'The gateway server did not receive a timely response'
#define  HTTP_VERSION_NOT_SUPPORTED 505 // 'HTTP Version Not Supported', 'Cannot fulfill request'
#define  VARIANT_ALSO_NEGOTIATES 506 // 'Variant Also Negotiates'
#define  INSUFFICIENT_STORAGE 507 // 'Insufficient Storage'
#define  LOOP_DETECTED 508 // 'Loop Detected'
#define  NOT_EXTENDED 510 // 'Not Extended'
#define  NETWORK_AUTHENTICATION_REQUIRED 511 //'Network Authentication Required', 'The client needs to authenticate to gain network access'
