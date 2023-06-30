#include "lib/common.h"

int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"USAGE: reliableclient <IPaddress>");
    }

    int sockfd;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"socket error");
    }

    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);

    int res=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(res<0){
        error(1,errno,"connect error");
    }
    signal(SIGPIPE,SIG_IGN);
    char *msg="network programming";
    int count=1000000;

    while(count>0){
        int n=send(sockfd,msg,strlen(msg),0);
        fprintf(stdout,"send into buffer %d\n",n);
        if(n<=0){
            error(1,errno,"send error");
            return -1;
        }
        count--;
    }
    exit(0);
}