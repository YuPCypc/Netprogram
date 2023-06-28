#include "lib/common.h"
#include "message_obj.h"


static int count;

static void sig_int(){
    printf("received %d datagrams\n",count);
    exit(0);
}

int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"USAGE: pingserver <time>");
    }
    int sleepTime=atoi(argv[1]);
    int listenfd;
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd<0){
        error(1,errno,"socket failed");
    }
    struct sockaddr_in serveraddr,clientaddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);

    int res1=bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(res1<0){
        error(1,errno,"bind failed");
    }

    int res2=listen(listenfd,LISTENQ);
    if(res2<0){
        error(1,errno,"listen failed");
    }

    signal(SIGINT,sig_int);
    signal(SIGPIPE,SIG_DFL);

    int connfd;
    socklen_t clientlen=sizeof(clientaddr);
    if((connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen))<0){
        error(1,errno,"accept failed");
    }

    struct message_obj messageobj;
    count=0;
    while(1){
        int n=read(connfd,&messageobj,sizeof(messageobj));
        if(n<0){
            error(1,errno,"read error");
        }
        else if(n==0){
            error(1,errno,"client terminated");
        }
        printf("received %d bytes\n",n);
        count++;
        switch (ntohl(messageobj.type)){
            case MSG_TYPE1 :
                printf("process  MSG_TYPE1 \n");
                break;

            case MSG_TYPE2 :
                printf("process  MSG_TYPE2 \n");
                break;
            case MSG_PING :{
                struct message_obj pong_message;
                pong_message.type=MSG_PONG;
                sleep(sleepTime);
                int rc=send(connfd,&pong_message,sizeof(pong_message),0);
                if(rc<0){
                    error(1,errno,"send error");
                }
                break;
            }
            
            default:
                error(1,0,"unknowed type :%d\n",ntohl(messageobj.type));
            
        }
    }


}