#include "lib/common.h"

int main(int argc,char** argv)
{
    if(argc!=2){
        error(1,0,"USAGE: nioclient <Ipaddress>");
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
    inet_pton(AF_INET,argv[1],&serveraddr.sin_addr);

    int connres=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(connres<0){
        error(1,errno,"connect error");
    }

    struct linger ling;
    ling.l_linger=0;
    ling.l_onoff=1;
    setsockopt(sockfd,SOL_SOCKET,SO_LINGER,&ling,sizeof(ling));
    close(sockfd);
    exit(0);
}