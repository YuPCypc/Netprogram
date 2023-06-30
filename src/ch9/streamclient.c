 #include "lib/common.h"

 struct Message{
    u_int32_t msg_len;
    u_int32_t msg_type;
    char data[128];
 };

 int main(int argc,char** argv){
    if(argc!=2){
        error(1,0,"USAGE: streamclient <IPaddress>");
    }
    int sockfd;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"socket error");
    }

    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);

    int res;
    res=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(res<0){
        error(1,errno,"connect error");
    }
    struct Message message;
    int n;
    while(fgets(message.data,sizeof(message.data),stdin)!=NULL){
        n=strlen(message.data);
        message.msg_len=htonl(n);
        message.msg_type=htonl(1);
        if(send(sockfd,(char*)&message,8+n,0)<0){
            error(1,errno,"send error");
        }
    }
    exit(0);
 }