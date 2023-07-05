#include "lib/common.h"

char rot13(char c)
{
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    return c;
}

void sigchild_handler()
{
    while (waitpid(-1, NULL, WNOHANG))
        ;
    return;
}

void child_run(int fd)
{
    char buf[MAX_LINE];
    int index;
    int res;

    while (1)
    {
        index = read(fd, buf, sizeof(buf));
        if (index == 0)
        {
            error(1, 0, "peer close");
        }
        else if (index < 0)
        {
            error(1, errno, "read error");
        }
        for (int i = 0; i < index; i++)
        {
            buf[i] = rot13(buf[i]);
        }
        write(fd, buf, index);
    }
}

int main(int argc, char **arhv)
{
    int listenfd, connectfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        error(1, errno, "listen error");
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

    signal(SIGCHLD, sigchild_handler);
    while (1)
    {
        struct sockaddr_storage clientaddr;
        socklen_t clientlen;
        int fd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
        if (fd < 0)
        {
            error(1, errno, "fork error");
        }
        if (fork() == 0)
        {
            close(listenfd);
            child_run(fd);
            exit(0);
        }
        else
        {
            close(fd);
        }
    }
    exit(0);
}