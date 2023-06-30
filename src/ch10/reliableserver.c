#include "lib/common.h"

int main(int argc,char** argv)
{
    int listenfd;
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

    signal(SIGPIPE,SIG_DFL);

    socklen_t clientlen;
    int connectfd;
    connectfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
    if(connectfd<0){
        error(1,errno,"connect error");
    }
    char message[1024];
    int time=0;

    while(1){
        int n=recv(connectfd,message,1024,0);
        if(n<0){
            error(1,errno,"recv error");
        }
        else if(n==0){
            error(1,0,"client close");
        }
        time++;
        
        fprintf(stdout,"1k read for %d \n",time);
        sleep(1);
    }
    exit(0);
}