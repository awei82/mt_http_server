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
#include <dirent.h>

#include "http_server.h"
#include "gf_handler.h"


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

#define PATHLIM 1024

/* Main ========================================================= */
int main(int argc, char **argv) {
    int option_char = 0;
    unsigned short portnum = 6200;
    char ip_addr[PATHLIM];
    strncpy(ip_addr, "0.0.0.0", PATHLIM);

    char directory[PATHLIM];
    strncpy(directory, ".", PATHLIM);

    setbuf(stdout, NULL); // disable caching

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "p:b:d:hx", gLongOptions, NULL)) != -1) {
        switch (option_char) {
            case 'p': // listen-port
                portnum = atoi(optarg);
                if ((portnum < 1025) || (portnum > 65535)) {
                fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portnum);
                exit(1);
                } 
                break;
            case 'b': // bind address
                if (strlen(optarg) >= PATHLIM/4) {
                    fprintf(stdout, "invalid bind address");
                    exit(1);
                }
                strncpy(ip_addr, optarg, PATHLIM);

                break;
            case 'd':
                // limit directory path to 256 chars
                if (strlen(optarg) >= PATHLIM/4) {
                    fprintf(stdout, "directory path over %d limit", PATHLIM/4);
                    exit(1);
                }
                strncpy(directory, optarg, PATHLIM);

                // check directory exists
                DIR* dir = opendir(directory);
                if (dir) {                      /* Directory exists. */
                    closedir(dir);
                } else if (ENOENT == errno) {   /* Directory does not exist. */
                    fprintf(stdout, "directory %s does not exist\n", directory);
                    exit(0);
                } else {                        /* opendir() failed for some other reason. */
                    fprintf(stdout, "opendir() error\n");
                    exit(1);
                }

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

    // set up GET FILE handler
    http_handler_t *gf_handler = gf_handler_create();
    gf_handler_set_dir(gf_handler, directory);


    /*Initialize server*/
    http_server_t *getfile_server = http_server_create();

    /*Setting options*/
    http_server_set_port(getfile_server, portnum);
    http_server_set_address(getfile_server, ip_addr);
    http_server_set_maxpending(getfile_server, 6);
    http_server_set_handler(getfile_server, gf_handler);

    // /* this implementation does not pass any extra state, so it uses NULL. */
    // /* this value could be non-NULL.  You might want to test that in your own code. */
    // http_server_set_handlerarg(getfile_server, directory);

    /*Loops forever*/
    http_server_serve(getfile_server);
}
