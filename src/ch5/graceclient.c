#include "lib/common.h"

int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"usage: graceclient <IPaddress>");
    }
    int sockfd;
    sockfd=socket(PF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"sock failed");
    }
    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof serveraddr);
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&serveraddr.sin_addr.s_addr);
    
    int connres=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(connres<0){
        error(1,errno,"connect failed");
    }
    char sendline[MAX_LINE],recvline[MAX_LINE+1];
    int n;

    fd_set allreads;
    fd_set readmask;

    FD_ZERO(&allreads);
    FD_SET(0,&allreads);
    FD_SET(sockfd,&allreads);

    while(1){
        readmask=allreads;
        int res=select(sockfd+1,&readmask,NULL,NULL,NULL);
        if(res<=0){
            error(1,errno,"select failed");
        }
        if(FD_ISSET(sockfd,&readmask)){
            n=read(sockfd,recvline,MAX_LINE);
            if(n<0){
                error(1,errno,"read failed");
            }
            else if(n==0){
                error(1,0,"server terminated\n");
            }
            recvline[n]=0;
            fputs(recvline,stdout);
            fputs("\n",stdout);
        }
        else if(FD_ISSET(0,&readmask)){
            if(fgets(sendline,MAX_LINE,stdin)!=NULL){
                if(strncmp(sendline,"close",5)==0){
                    FD_CLR(0,&allreads);
                    if(close(sockfd)==-1){
                        error(1,errno,"close failed");
                    }
                    sleep(5);
                    exit(0);
                }
                else if(strncmp(sendline,"shutdown",8)==0){
                    FD_CLR(0,&allreads);
                    if(shutdown(sockfd,1)==-1){
                        error(1,errno,"shutdown failed");
                    }
                }
                else{
                    int len=strlen(sendline);
                    if(sendline[len-1]=='\n'){
                        sendline[len-1]=0;
                    }
                    printf("now sending %s\n",sendline);
                    int res=write(sockfd,sendline,len);
                    if(res<0){
                        error(1,errno,"write failed");
                    }
                    printf("send byte:%d\n",res);
                }
            }
        }
    }
}