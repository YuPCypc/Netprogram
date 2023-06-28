#include "lib/common.h"

static int count;

static void sig_int(){
    printf("\nreceived %d datagrams\n",count);
    exit(0);
}

int main(int argc,char** argv)
{
    int sockfd;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"socket error");
    }

    struct sockaddr_in serveraddr,clientaddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);

    //重用套接字
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

    signal(SIGINT,sig_int);
    
    int connectfd;
    socklen_t clientlen=sizeof(clientaddr);
    connectfd=accept(sockfd,(struct sockaddr*)&clientaddr,&clientlen);
    if(connectfd<0){
        error(1,errno,"connect error");
    }

    count=0;
    char send_line[MAX_LINE];

    while(1){
        int n=read(connectfd,send_line,MAX_LINE);
        if(n<0){
            error(1,errno,"read error");
        }
        else if(n==0){
            error(1,0,"client terminated");
        }
        else{
            send_line[n]=0;
            printf("received %d bytes: %s\n",n,send_line);
            count++;
        }
    }
    exit(0);
}