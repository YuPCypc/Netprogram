#include "lib/common.h"


static int count;

static void sig_int(){
    printf("\nreceived %d datagrams\n",count);
    exit(0);
}

int main(int argc,char** argv){
    int sockfd;
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0){
        error(1,errno,"socket error");
    }

    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);

    int res1=bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(res1<0){
        error(1,errno,"bind error");
    }

    socklen_t clientlen;
    clientlen=sizeof(serveraddr);
    char message[MAX_LINE];
    count=0;
    signal(SIGINT,sig_int);

    struct sockaddr_in clientaddr;
    int n=recvfrom(sockfd,message,MAX_LINE,0,(struct sockaddr*)&clientaddr,&clientlen);
    if(n<0){
        error(1,errno,"recvfrom error");
    }
    message[n]=0;
    printf("received: %dbyte  %s\n",n,message);

    if(connect(sockfd,(struct sockaddr*)&clientaddr,clientlen)){
        error(1,errno,"connect failed");
    }

    while(strncmp(message,"goodbye",7)!=0){
        char send_line[MAX_LINE];
        sprintf(send_line,"Hi, %s",message);

        int res=send(sockfd,send_line,strlen(send_line),0);
        if(res<0){
            error(1,errno,"send error");
        }
        printf("send %d bytes\n",res);

        res=recv(sockfd,message,MAX_LINE,0);
        if(res<0){
            error(1,errno,"recv error");
        }
        count++;
    }
    exit(0);
}