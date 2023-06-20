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


size_t readn(int fd, void *vptr, size_t n);
void error(int status, int err, char *fmt, ...);