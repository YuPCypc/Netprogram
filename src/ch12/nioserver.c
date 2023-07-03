#include "lib/common.h"


#define FD_SIZE 128

struct Buffer{
    int connectfd;
    char buf[MAX_LINE];
    size_t readindex;
    size_t writeindex;
    int readable;
};

struct Buffer* alloc_buf(){
    struct Buffer *buff=malloc(sizeof(struct Buffer));
    if(buff==NULL){
        return NULL;
    }
    buff->connectfd=buff->readindex=buff->writeindex=buff->readable=0;
    return buff;
}
char rot13(char c){
    if((c>='a'&&c<='m')||(c>='A'&&c<='M')) c+=13;
    else if((c>='n'&&c<='z')||(c>='N'&&c<='Z')) c-=13;
    else return c;
    return c;
}

int Socketread(int fd,struct Buffer* buf){
    char line[MAX_LINE];
    int i;
    int res;
    while(1){
        res=recv(fd,line,MAX_LINE,0);
        if(res<=0){
            break;
        }
        for(i=0;i<res;i++){
            if(buf->writeindex<MAX_LINE){
                buf->buf[buf->writeindex++]=rot13(line[i]);
            }
            if(line[i]=='\n'){
                buf[i].readable=1;
            }
        }
    }
    if(res==0){
        return 1;
    }
    else if(res<0){
        if(errno==EAGAIN){
            return 0;
        }
        return -1;
    }
    return 0;
}

int Socketwrite(int fd,struct Buffer* buf){
    while(buf->readindex<buf->writeindex){
        int res=send(fd,buf->buf+buf->readindex,buf->writeindex-buf->readindex,0);
        if(res<0){
            if(errno==EAGAIN){
                return 0;
            }
            return -1;
        }
        buf->readindex+=res;
    }
    if(buf->readindex==buf->writeindex){
        buf->readindex=buf->writeindex=0;
        buf->readable=0;
    }
    return 0;
}

int main(int argc,char** argv){
    int sockfd;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error(1,errno,"socket error");
    }
    fcntl(sockfd,F_SETFL,O_NONBLOCK);
    struct sockaddr_in serveraddr;
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
    int i,maxfd;
    struct Buffer* buffer[FD_SIZE];
    for(i=0;i<FD_SIZE;i++){
        buffer[i]=alloc_buf();
    }

    fd_set readsets;
    fd_set writesets;
    fd_set exceptsets;

    FD_ZERO(&readsets);
    FD_ZERO(&writesets);
    FD_ZERO(&exceptsets);

    while(1){
        maxfd=sockfd;
        FD_ZERO(&readsets);
        FD_ZERO(&writesets);
        FD_ZERO(&exceptsets);

        FD_SET(sockfd,&readsets);

        for(i=0;i<FD_SIZE;i++){
            if(buffer[i]->connectfd>0){
                maxfd=buffer[i]->connectfd;
            }
            FD_SET(buffer[i]->connectfd,&readsets);
            if(buffer[i]->readable){
                FD_SET(buffer[i]->connectfd,&writesets);
            }
        }

        if(select(maxfd+1,&readsets,&writesets,&exceptsets,0)<0){
            error(1,errno,"select error");
        }

        if(FD_ISSET(sockfd,&readsets)){
            printf("listening socket readable\n");
            sleep(5);
            struct sockaddr_storage ss;
            socklen_t sslen;
            int fd=accept(sockfd,(struct sockaddr*)&ss,&sslen);
            if(fd<0){
                error(1,errno,"accept error");
            }
            else if(fd>=FD_SIZE){
                error(1,0,"too many connections");
                close(fd);
            }
            else{
                fcntl(fd,F_SETFL,O_NONBLOCK);
                if(buffer[fd]->connectfd==0){
                    buffer[fd]->connectfd=fd;
                }
            }
        }
    }//end while
    for(i=0;i<maxfd+1;i++){
        int res=0;
        if(i==sockfd) continue;
        if(FD_ISSET(i,&readsets)){
            res=Socketread(i,buffer[i]);
        }
        if(res==0&&FD_ISSET(i,&writesets)){
            res=Socketwrite(i,buffer[i]);
        }
        if(res){
            buffer[i]->connectfd=0;
            close(i);
        }
    }

    exit(0);
}