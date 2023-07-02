#include "lib/common.h"

char* run_cmd(char* cmd){
    char* data=malloc(65536);
    bzero(data,65536);
    FILE* fdp;
    fdp=popen(cmd,"r");
    char* ans=data;
    char buf[1024];
    if(fdp){
        while(fgets(buf,sizeof(buf),fdp)){
            strcat(ans,buf);
        }
    }
    pclose(fdp);
    return ans;
}

int main(int argc,char** argv)
{
    int sockfd;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"socket error");
    }

    struct sockaddr_in serveraddr,clientaddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);

    int on=1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    int res=bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(res<0){
        error(1,errno,"bind error");
    }
    res=listen(sockfd,LISTENQ);
    if(res<0){
        error(1,errno,"listen error");
    }
    signal(SIGPIPE,SIG_IGN);
    socklen_t clientlen;
    char recv_line[256];
    while(1){
        int connectfd=accept(sockfd,(struct sockaddr*)&clientaddr,&clientlen);
        if(connectfd<0){
            error(1,errno,"connect error");
        }
        while(1){
            bzero(recv_line,sizeof(recv_line));
            int n=read(connectfd,recv_line,sizeof(recv_line));
            if(n<0){
                error(1,errno,"read error");
            }
            else if(n==0){
                printf("client close");
                close(connectfd);
                break;
            }
            recv_line[n]=0;
            if(strncmp(recv_line,"ls",2)==0||
            strncmp(recv_line,"pwd",3)==0){
                char *result=run_cmd(recv_line);
                printf("%s",result);
                int res=send(connectfd,result,strlen(result),0);
                free(result);
                if(res<0){
                    error(1,errno,"send failed");
                }
            }
            else if(strncmp(recv_line,"cd ",3)==0){
                printf("%s\n",recv_line);
                char target[256];
                bzero(target, sizeof(target));
                memcpy(target, recv_line + 3, strlen(recv_line) - 3);
                if (chdir(target) == -1) {
                    printf("change dir failed, %s\n", target);
                }
            }
            else{
                char *result="unknow cmd";
                int res=send(connectfd,result,strlen(result),0);
                if(res<0){
                    error(1,errno,"send failed");
                }
            }
        }
    }
    exit(0);
}