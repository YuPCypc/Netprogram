#include <malloc/_malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/_types/_socklen_t.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/_types/_timeval.h>
#include <sys/signal.h>


size_t readn(int fd, void *vptr, size_t n);
void error(int status, int err, char *fmt, ...);


#define SERV_PORT 43211
#define MAX_LINE 4096
#define LISTENQ 1024
#define BUFFER_SIZE 4096