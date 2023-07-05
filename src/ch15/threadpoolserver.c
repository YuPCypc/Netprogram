#include "lib/common.h"

#define THREAD_SIZE 4
#define BLOCK_QUEUE_SIZE 100

extern void loop_echo(int);

typedef struct
{
    int num;
    int *fd;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Blockqueue;

typedef struct
{
    pthread_t tid;
    long thread_count;
} Thread;
Thread *thread_poll;

void block_queue_init(Blockqueue *blockqueue, int number)
{
    blockqueue->num = number;
    blockqueue->fd = calloc(number, sizeof(int));
    blockqueue->front = blockqueue->rear = 0;
    pthread_mutex_init(&blockqueue->mutex, NULL);
    pthread_cond_init(&blockqueue->cond, NULL);
}

void block_queue_push(Blockqueue *blockqueue, int fd)
{
    pthread_mutex_lock(&blockqueue->mutex);
    blockqueue->fd[blockqueue->rear] = fd;
    if (++blockqueue->rear == blockqueue->num)
    {
        blockqueue->rear = 0;
    }
    printf("push fd: %d\n", fd);
    pthread_cond_signal(&blockqueue->cond);
    pthread_mutex_unlock(&blockqueue->mutex);
}

int block_queue_pop(Blockqueue *blockqueue)
{
    pthread_mutex_lock(&blockqueue->mutex);
    while (blockqueue->front == blockqueue->rear)
    {
        pthread_cond_wait(&blockqueue->cond, &blockqueue->mutex);
    }
    int fd = blockqueue->fd[blockqueue->front];
    if (++blockqueue->front == blockqueue->num)
    {
        blockqueue->front = 0;
    }
    printf("pop fd: %d\n", fd);
    pthread_mutex_unlock(&blockqueue->mutex);
    return fd;
}

void thread_run(void *arg)
{
    pthread_t tid = pthread_self();
    pthread_detach(tid);

    Blockqueue *blockqueue = (Blockqueue *)arg;
    while (1)
    {
        int fd = block_queue_pop(blockqueue);
        printf("get fd:%d from thread:%ld\n", fd, tid);
        loop_echo(fd);
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error(1, errno, "socket failed");
    }

    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERV_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int res = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (res < 0)
    {
        error(1, errno, "bind error");
    }
    res = listen(sockfd, LISTENQ);
    if (res < 0)
    {
        error(1, errno, "listen error");
    }

    Blockqueue blockqueue;
    block_queue_init(&blockqueue, BLOCK_QUEUE_SIZE);

    thread_poll = calloc(THREAD_SIZE, sizeof(Thread));
    for (int i = 0; i < THREAD_SIZE; i++)
    {
        pthread_create(&thread_poll[i].tid, NULL, &thread_run, (void *)&blockqueue);
    }

    while (1)
    {
        struct sockaddr_storage ss;
        socklen_t slen;
        int fd = accept(sockfd, (struct sockaddr *)&ss, &slen);
        if (fd < 0)
        {
            error(1, errno, "accept error");
        }
        else
        {
            block_queue_push(&blockqueue, fd);
        }
    }
    exit(0);
}