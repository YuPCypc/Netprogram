#include "lib/common.h"
#include <sys/socket.h>

int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"usage: unixdataclient <local_path>");
    }

    int sockfd;
    sockfd=socket(AF_LOCAL,SOCK_DGRAM,0);
    if(sockfd<0){
        error(1,errno,"create sock failed");
    }

    struct sockaddr_un serveraddr;
    char* local_path=argv[1];
    unlink(local_path);
    bzero(&serveraddr,sizeof (serveraddr));
    serveraddr.sun_family=AF_LOCAL;
    strcpy(serveraddr.sun_path,local_path);

    if(bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))<0){
        error(1,errno,"bind failed");
    }

    char buf[BUFFER_SIZE];

    struct sockaddr_un clientaddr;
    socklen_t clientlen=sizeof(clientaddr);

    while(1){
        bzero(buf,sizeof buf);

        if(recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&clientaddr,&clientlen)==0){
            printf("client quit");
            break;
        }
        printf("Receive from: %s\n",buf);

        char send_line[MAX_LINE];
        bzero(send_line,sizeof send_line);
        sprintf(send_line,"Hi, %s",buf);

        size_t n=strlen(send_line);
        printf("now sending: %s\n",send_line);
        if(sendto(sockfd,send_line,n,0,(struct sockaddr*)&clientaddr,clientlen)!=n){
            error(1,errno,"sendto error");
        }
    }
    close(sockfd);
    return 0;
}