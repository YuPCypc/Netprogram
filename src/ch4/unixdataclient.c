#include "lib/common.h"
#include <sys/socket.h>

int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"usage: unixdataclient <local_path>");
    }

    int sockfd;
    struct sockaddr_un clientaddr,serveraddr;

    sockfd=socket(AF_LOCAL,SOCK_DGRAM,0);
    if(sockfd<0){
        error(1,errno,"sock failed");
    }

    bzero(&clientaddr,sizeof clientaddr);
    clientaddr.sun_family=AF_LOCAL;
    strcpy(clientaddr.sun_path, tmpnam(NULL));

    if(bind(sockfd,(struct sockaddr*)&clientaddr,sizeof(clientaddr))<0){
        error(1,errno,"bind failed");
    }

    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sun_family=AF_LOCAL;
    strcpy(serveraddr.sun_path,argv[1]);

    char send_line[MAX_LINE];
    bzero(send_line,MAX_LINE);
    char recv_line[MAX_LINE];

    while(fgets(send_line,MAX_LINE,stdin)!=NULL){
        int i=strlen(send_line);
        if(send_line[i-1]=='\n'){
            send_line[i-1]=0;
        }
        size_t n=strlen(send_line);
        printf("now sending %s\n",send_line);

        if(sendto(sockfd,send_line,n,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr))<0){
            error(1,errno,"sendto error");
        }

        int ns=recvfrom(sockfd,recv_line,MAX_LINE,0,NULL,NULL);
        recv_line[ns]=0;

        fputs(recv_line,stdout);
        fputs("\n",stdout);
    }
    return 0;

}