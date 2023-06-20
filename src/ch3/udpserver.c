#include "lib/common.h"
#include <sys/signal.h>
#include <sys/socket.h>

static int count;

static void recvfrom_int(int signo){
    printf("\nreceived %d datagrams\n",count);
    exit(0);
}

int main(int argc,char** argv){
    int sockfd;
    sockfd=socket(PF_INET,SOCK_DGRAM,0);

    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERV_PORT);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

    bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

    socklen_t clilen;
    char message[MAX_LINE];
    count=0;

    signal(SIGINT, recvfrom_int);

    struct sockaddr_in clientaddr;

    clilen=sizeof(clientaddr);

    while(1){
        int n=recvfrom(sockfd,message,MAX_LINE,0,(struct sockaddr*)&clientaddr,&clilen);
        message[n]=0;
        printf("\nreceived %d bytes: %s\n",n,message);
        char sendline[MAX_LINE];
        sprintf(sendline,"Hi, %s",message);
        sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr*)&clientaddr,clilen);
        count++;
    }
    return 0;
}
