#include "lib/common.h"
#include <sys/socket.h>
#include <sys/un.h>

int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"usage: unixstreamclient <local_path>");
    }

    int sockfd;
    struct sockaddr_un servaddr;

    sockfd=socket(AF_LOCAL,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"creat sock failed");
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sun_family=AF_LOCAL;
    strcpy(servaddr.sun_path,argv[1]);

    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
        error(1,errno,"connect failed");
    }

    char send_line[MAX_LINE];
    bzero(send_line,sizeof send_line);
    char recv_line[MAX_LINE];

    while(fgets(send_line,MAX_LINE,stdin)!=NULL){
        int n=sizeof(send_line);
        if(write(sockfd,send_line,n)!=n){
            error(1,errno,"write error");
        }
        if(read(sockfd,recv_line,MAX_LINE)==0){
            error(1,errno,"server terminated prematurely");
        }
        fputs(recv_line,stdout);
    }
    return 0;
}