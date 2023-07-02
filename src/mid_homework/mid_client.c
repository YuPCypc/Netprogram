#include "lib/common.h"


int main(int argc,char** argv)
{
    if(argc!=3){
        error(1,0,"USAGE: mid_client <IPaddress> <port>");
    }

    int sockfd;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"socket error");
    }
    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&serveraddr.sin_addr);

    int connectres=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(connectres<0){
        error(1,errno,"connect error");
    }
    signal(SIGPIPE,SIG_IGN);

    fd_set readmask;
    fd_set allreads;
    FD_ZERO(&allreads);
    FD_SET(0,&allreads);
    FD_SET(sockfd,&allreads);

    char send_line[MAX_LINE],recv_line[MAX_LINE];

    while(1){
        readmask=allreads;
        int res=select(sockfd+1,&readmask,NULL,NULL,NULL);
        if(res<=0){
            error(1,errno,"select error");
        }

        if(FD_ISSET(sockfd,&readmask)){
            int n=read(sockfd,recv_line,MAX_LINE);
            if(n<0){
                error(1,errno,"read error");
            }
            else if(n==0){
                error(1,0,"server closed");
            }
            recv_line[n]=0;
            fputs(recv_line,stdout);
            fputs("\n",stdout);
        }
        if(FD_ISSET(0,&readmask)){
            if(fgets(send_line,MAX_LINE,stdin)){
                int len=strlen(send_line);
                if(send_line[len-1]=='\n'){
                    send_line[len-1]=0;
                }
                if(strncmp(send_line,"quit",4)==0){
                    int res=shutdown(sockfd,1);
                    if(res<0){
                        error(1,errno,"shutdown failed");
                    }
                }
                int rc=send(sockfd,send_line,len,0);
                if(rc<0){
                    error(1,errno,"send error");
                }
            }
        }
    }
    exit(0);
}