#include "lib/common.h"
#include <malloc/_malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

int main(int argc,char** argv){
    if (argc != 2) {
        error(1, 0, "usage: udpclient <IPaddress>");

    }
    int sockfd;
    sockfd=socket(PF_INET,SOCK_DGRAM,0);
    
    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&serveraddr.sin_addr.s_addr);

    socklen_t serlen=sizeof(serveraddr);

    struct sockaddr* replyaddr;

    replyaddr=malloc(serlen);

    char sendline[MAX_LINE],recvline[MAX_LINE+1];
    socklen_t len;
    int n;

    while(fgets(sendline,MAX_LINE,stdin)){
        int i=strlen(sendline);
        if(sendline[i-1]=='\n'){
            sendline[i-1]=0;
        }
        printf("now sending %s\n",sendline);

        int res=sendto(sockfd,sendline,i,0,(struct sockaddr*)&serveraddr,serlen);
        if(res<0){
            error(1,errno,"sendto fail");
        }
        printf("send bytes: %d \n",res);

        len=0;
        n=recvfrom(sockfd,recvline,MAX_LINE,0,(struct sockaddr*)&replyaddr,&len);
        if(n<0){
            error(1,errno,"recvfrom fail");
        }
        recvline[n]=0;
        fputs(recvline,stdout);
        fputs("\n",stdout);
    }
    return 0;
}