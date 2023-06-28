#include "lib/common.h"
#include "message_obj.h"


#define KEEP_ALIVE_TIME 10
#define KEPP_ALIVE_INTERVAL 3
#define KEEP_ALIVE_PROBETIME 3

int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"USAGE: pingclient <IPADDRESS>");
    }
    int sockfd;
    sockfd=socket(PF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"sock failed");
    }

    struct sockaddr_in serveraddr,clientaddr;
    bzero(&serveraddr,sizeof serveraddr);
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);

    socklen_t serverlen=sizeof(serveraddr);
    int connect_res=connect(sockfd,(struct sockaddr*)&serveraddr,serverlen);
    if(connect_res<0){
        error(1,errno,"connect failed");
    }

    char recv_line[MAX_LINE];
    int n;

    fd_set allreads;
    fd_set readmask;
    FD_ZERO(&allreads);
    FD_SET(0,&allreads);
    FD_SET(sockfd,&allreads);

    struct timeval tv;
    int heartbeats=0;
    tv.tv_sec=KEEP_ALIVE_TIME;
    tv.tv_usec=0;

    struct message_obj messobj;

    while(1){
        readmask=allreads;
        
        int res=select(sockfd+1,&readmask,NULL,NULL,&tv);
        if(res<0){
            error(1,errno,"select failed");
        }
        if(res==0){
            if(++heartbeats>KEEP_ALIVE_PROBETIME){
                error(1,0,"connect failed");
            }
            printf("send heartbeat #%d\n",heartbeats);
            messobj.type=htonl(MSG_PING);
            res=send(sockfd,&messobj,sizeof(messobj),0);//tag
            if(res<0){
                error(1,errno,"send failed");
            }
            tv.tv_sec=KEPP_ALIVE_INTERVAL;
            continue;
        }
        if(FD_ISSET(sockfd,&readmask)){
            n=read(sockfd,recv_line,MAX_LINE);
            if(n<0){
                error(1,errno,"read error");
            }
            else if(n==0){
                error(1,errno,"server terminated");
            }
            printf("received heartbeat,make heatbeats to 0\n");
            heartbeats=0;
            tv.tv_sec=KEEP_ALIVE_TIME;
        }
    }

}