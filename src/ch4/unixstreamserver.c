#include "lib/common.h"

int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"usage: unixstreamserver <local_path>");
    }

    int listenfd,connfd;
    socklen_t clientlen;
    struct sockaddr_un cliaddr,servaddr;

    listenfd=socket(AF_LOCAL,SOCK_STREAM,0);
    if(listenfd<0)
        error(1,errno,"socket fail");

    char* local_path=argv[1];
    unlink(local_path);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sun_family=AF_LOCAL;
    strcpy(servaddr.sun_path,local_path);

    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
        error(1,errno,"bind fail");
    }

    if(listen(listenfd,LISTENQ)<0){
        error(1,errno,"listen fail");
    }
    clientlen=sizeof(cliaddr);
    if((connfd=accept(listenfd, (struct sockaddr *)&cliaddr, &clientlen))<0){
        error(1, errno, "accept failed");
    }

    char buf[BUFFER_SIZE];

    while(1){
        bzero(buf,sizeof buf);
        if(read(connfd,buf,BUFFER_SIZE)==0){
            printf("client quit");
            break;
        }
        printf("Receive: %s",buf);

        char send_line[MAX_LINE];
        bzero(send_line,MAX_LINE);
        sprintf(send_line,"Hi, %s",buf);

        int n=sizeof(send_line);
        if(write(connfd,send_line,n)!=n){
            error(1,errno,"write error");
        }
    }
    close(listenfd);
    close(connfd);
    return 0;
}