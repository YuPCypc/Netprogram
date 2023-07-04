#include "lib/common.h"
#include  <sys/epoll.h>

#define MAXEVENT 128

char rot13(char c){
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

int main(int argc,char** argv){
    int sockfd;
    int n,i;
    int efd;
    struct epoll_event event;
    struct epoll_event* events;

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"socket error");
    }
    fcntl(sockfd,F_SETFL,O_NONBLOCK);
    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);

    int on=1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    int res=bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(res<0){
        error(1,errno,"bind error");
    }
    res=listen(sockfd,LISTENQ);
    if(res<0){
        error(1,errno,"listen error");
    }

    efd=epoll_create1(0);
    if(efd==-1){
        error(1,errno,"epoll create error");
    }
    event.data.fd=sockfd;
    event.events=EPOLLIN|EPOLLET;
    if(epoll_ctl(efd,EPOLL_CTL_ADD,sockfd,&event)==-1){
        error(1,errno,"epoll_ctl error");
    }

    events=calloc(MAXEVENT,sizeof(event));

    while(1){
        n=epoll_wait(efd,events,MAXEVENT,-1);
        printf("epoll_wait wake\n");
        for(i=0;i<n;i++){
            if((events[i].events&EPOLLERR)||
            (events[i].events&EPOLLHUP)||
            (!(events[i].events&EPOLLIN))){
                error(0,0,"epoll error");
                close(events[i].data.fd);
            }
            else if(sockfd==events[i].data.fd){
                struct sockaddr_storage ss;
                socklen_t slen;
                int fd=accept(sockfd,(struct sockaddr*)&ss,&slen);
                if(fd<0){
                    error(1,errno,"accept failed");
                }
                else{
                    fcntl(fd,F_SETFL,O_NONBLOCK);
                    event.data.fd=fd;
                    event.events=EPOLLET|EPOLLIN;
                    if(epoll_ctl(efd,EPOLL_CTL_ADD,fd,&event)==-1){
                        error(1,errno,"epoll_ctl error");
                    }
                }
            }
            else{
                int fd=events[i].data.fd;
                printf("get event on socket fd=%d\n",fd);
                while(1){
                    char buf[512];
                    if((n=read(fd,buf,sizeof(buf)))<0){
                        if(errno!=EAGAIN){
                            error(1,errno,"read error");
                            close(fd);
                        }
                        break;
                    }
                    else if(n==0){
                        epoll_ctl(efd,EPOLL_CTL_DEL,fd,&events[i]);
                        close(fd);
                        break;
                    }
                    else{
                        for(int i=0;i<n;i++){
                            buf[i]=rot13(buf[i]);
                        }
                        buf[n-1]='\n';
                        if(write(fd,buf,strlen(buf))<0){
                            error(1,errno,"write error");
                        }
                    }
                }
            }
        }
    }
    free(events);
    close(sockfd);
    close(efd);
    exit(0);
}