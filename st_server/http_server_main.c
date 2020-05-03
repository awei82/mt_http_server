#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "http_server.h"


#define USAGE                                                                 \
"usage:\n"                                                                    \
"  http_server_main [options]\n"                                              \
"options:\n"                                                                  \
"  -p [listen_port]    Listen port (Default: 8888)\n"                         \
"  -b [bind_address]   Address to bind to (Default: 0.0.0.0)\n"               \
"  -d [directory]      Alternate directory (Default: current directory)\n"    \
"  -h                  Show this help message.\n"                             \

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
  {"port",          required_argument,      NULL,           'p'},
  {"bind_address",  required_argument,      NULL,           'b'},
  {"directory",     required_argument,      NULL,           'd'},
  {"help",          no_argument,            NULL,           'h'},
  {NULL,            0,                      NULL,             0}
};



/* Main ========================================================= */
int main(int argc, char **argv) {
  int option_char = 0;
  unsigned short portnum = 6200;

  // struct sockaddr_in serv_addr;
  // memset(&serv_addr, 0, sizeof(serv_addr));
  // serv_addr.sin_family = AF_INET;
  // serv_addr.sin_addr.s_addr = INADDR_ANY;
  // serv_addr.sin_port = htons(portnum);

  char *directory = ".";
  http_server_t *server;

  setbuf(stdout, NULL); // disable caching

  // Parse and set command line arguments
  while ((option_char = getopt_long(argc, argv, "p:b:d:hx", gLongOptions, NULL)) != -1) {
    switch (option_char) {
      case 'p': // listen-port
        portnum = atoi(optarg);
        if ((portnum < 1025) || (portnum > 65535)) {
          fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
          exit(1);
        } 
        // else {
        //   serv_addr.sin_port = htons(portnum);
        // }
        break;
      case 'b': // bind address
        int rv = inet_pton(serv_addr.sin_family, optarg, *serv_addr.sin_addr.s_addr) != 1;
        if (rv != 1) 
          fprintf(stdout, "Error binding to address. Defaulting to 0.0.0.0");
        break;
      case 'd':
        directory = optarg;   // TODO - review this
        break;
      case 'h': // help
        fprintf(stdout, "%s", USAGE);
        exit(0);
        break;       
      default:
        fprintf(stderr, "%s", USAGE);
        exit(1);                                       
    }
  }

  /*Initializing server*/
  gfs = gfserver_create();

  /*Setting options*/
  gfserver_set_port(gfs, portnum);
  gfserver_set_maxpending(gfs, 6);
  gfserver_set_handler(gfs, getfile_handler);

  /* this implementation does not pass any extra state, so it uses NULL. */
  /* this value could be non-NULL.  You might want to test that in your own code. */
  gfserver_set_handlerarg(gfs, NULL);

  /*Loops forever*/
  gfserver_serve(gfs);
}
