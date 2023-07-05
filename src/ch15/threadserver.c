#include "lib/common.h"

extern void loop_echo(int);

void *thread_run(void *arg)
{
    pthread_detach(pthread_self());
    int fd = *(int *)arg;
    loop_echo(fd);
}

int main(int argc, char **argv)
{
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        error(1, errno, "socket failed");
    }

    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERV_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int res = bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (res < 0)
    {
        error(1, errno, "bind error");
    }

    res = listen(listenfd, LISTENQ);
    if (res < 0)
    {
        error(1, errno, "listen error");
    }

    pthread_t tid;
    while (1)
    {
        struct sockaddr_storage ss;
        socklen_t slen;
        int fd = accept(listenfd, (struct sockaddr *)&ss, &slen);
        if (fd < 0)
        {
            error(1, errno, "accept error");
        }
        else
        {
            pthread_create(&tid, NULL, thread_run, &fd);
        }
    }
    exit(0);
}