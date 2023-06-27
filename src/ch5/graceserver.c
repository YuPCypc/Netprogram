#include "lib/common.h"
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/socket.h>

static int count;

static void sig_int(int signo){
    printf("\nreceived %d datagrams\n",count);
    exit(0);
}

int main(int argc,char** argv){
    int listenfd;
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd<0){
        error(1,errno,"socket fail");
    }

    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
    serveraddr.sin_port=htons(SERV_PORT);

    int res1=bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(res1<0){
        error(1,errno,"bind failed");
    }
    int res2=listen(listenfd,LISTENQ);
    if(res2<0){
        error(1,errno,"listen failed");
    }

    signal(SIGINT,sig_int);
    signal(SIGPIPE,SIG_DFL);

    int connfd;
    struct sockaddr_in clientaddr;
    socklen_t clientlen=sizeof(clientaddr);

    if((connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen))<0){
        error(1,errno,"accept failed");
    }
    char message[MAX_LINE];
    count=0;

    while(1){
        int n=read(connfd,message,MAX_LINE);
        if(n<0){
            error(1,errno,"read fail");
        }
        else if(n==0){
            error(1,0,"client terminated");
        }
        message[n]=0;
        printf("received %d bytes: %s\n",n,message);

        char send_line[MAX_LINE];
        sprintf(send_line,"Hi, %s",message);

        sleep(5);

        int wr=send(connfd,send_line,strlen(send_line),0);
        if(wr<0){
            error(1,errno,"error send");
        }
        printf("send bytes:%d \n",wr);
        
    }
}