#include "lib/common.h"
#include <sys/socket.h>

int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"USAGE: pollclient <IPaddress>");
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
    inet_pton(AF_INET,argv[1],&serveraddr.sin_addr.s_addr);

    int connres=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(connres<0){
        error(1,errno,"connect error");
    }

    char buf[MAX_LINE];
    while(fgets(buf,sizeof(buf),stdin)){
        int n=strlen(buf);
        if(buf[n-1]=='\n'){
            buf[n-1]=0;
        }
        int res=write(sockfd,buf,strlen(buf));
        if(res<0){
            error(1,errno,"write error");
        }
        n=read(sockfd,buf,sizeof(buf));
        if(n<0){
            error(1,errno,"read error");
        }
        else if(n==0){
            error(1,0,"server closed");
        }
        fputs(buf,stdout);
    }
    exit(0);
}