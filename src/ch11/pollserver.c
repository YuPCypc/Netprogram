#include "lib/common.h"

#define INIT_SIZE 128

int main(int argc,char** argv){
    int listenfd,connfd;
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd<0){
        error(1,errno,"socket error");
    }

    struct sockaddr_in serveraddr,clientaddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);

    int on=1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    
    int res=bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(res<0){
        error(1,errno,"bind error");
    }

    res=listen(listenfd,LISTENQ);
    if(res<0){
        error(1,errno,"listen error");
    }
    signal(SIGPIPE,SIG_IGN);

    struct pollfd event_set[INIT_SIZE];
    event_set[0].fd=listenfd;
    event_set[0].events=POLLRDNORM;

    for(int i=1;i<INIT_SIZE;i++){
        event_set[i].fd=-1;
    }
    int ready_num;
    char buf[MAX_LINE];
    while(1){
        if((ready_num=poll(event_set,INIT_SIZE,-1))<0){
            error(1,errno,"poll error");
        }

        if(event_set[0].revents & POLLRDNORM){
            socklen_t clientlen;
            connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
            if(connfd<0){
                error(1,errno,"connfd error");
            }
            int i;
            for(i=1;i<INIT_SIZE;i++){
                if(event_set[i].fd<0){
                    event_set[i].fd=connfd;
                    event_set[i].events=POLLRDNORM;
                    break;
                }
            }
            if(i==INIT_SIZE){
                error(1,0,"can not hold so many client");
            }

            if(--ready_num<=0){
                continue;
            }
        }
        for(int i=1;i<INIT_SIZE;i++){
            int fd;
            if((fd=event_set[i].fd)<0){
                continue;
            }
            if(event_set[i].revents&(POLLRDNORM|POLLERR)){
                int n;
                if((n=read(fd,buf,sizeof(buf)))>0){
                    fputs(buf,stdout);
                    if(write(fd,buf,n)<0){
                        error(1,errno,"write error");
                    }
                }
                else if(n==0||errno==ECONNREFUSED){
                    close(fd);
                    event_set[i].fd=-1;
                }
                else{
                    error(1,errno,"read error");
                }
                if(--ready_num<=0){
                    break;
                }
            }
        }
    }
    exit(0);
}