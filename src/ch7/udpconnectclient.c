#include "lib/common.h"


int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"USAGE: udpconnectclient <IPADDRESS>");
    }

    int sockfd;
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0){
        error(1,errno,"socket failed");
    }
    struct sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&serveraddr.sin_addr);

    if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))){
        error(1,errno,"connect error");
    }

    char send_line[MAX_LINE],recv_line[MAX_LINE];
    int n;

    while(fgets(send_line,MAX_LINE,stdin)!=NULL){
        int i=strlen(send_line);
        if(send_line[i-1]=='\n'){
            send_line[i-1]=0;
        }
        printf("now sending %s\n",send_line);

        int res=send(sockfd,send_line,i,0);
        if(res<0){
            error(1,errno,"send error");
        }
        printf("send %d bytes\n",res);

        n=recv(sockfd,recv_line,MAX_LINE,0);
        if(n<0){
            error(1,errno,"recv error");
        }
        recv_line[n]=0;
        fputs(recv_line,stdout);
        fputs("\n",stdout);
    }
    exit(0);
}