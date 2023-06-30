#include "common.h"

int readn(int fd,void* buf,size_t size){
    char *buff=buf;
    int len=size;
    while(len>0){
        int res=read(fd,buff,len);
        if(res<0){
            if(errno==EINTR){
                continue;
            }
            else return -1;
        }
        else if(res==0){
            break;
        }
        len-=res;
        buff+=res;
    }
    return size-len;
}

int read_msg(int fd, void *buf, size_t n){
    u_int32_t msg_len;
    u_int32_t msg_type;
    int res;
    
    res=readn(fd,&msg_len,sizeof(msg_len));
    if(res!=4){
        return res<0? -1:0;
    }
    msg_len=ntohl(msg_len);

    res=readn(fd,&msg_type,sizeof(msg_type));
    if(res!=4){
        return res<0? -1:0;
    }
    if(msg_len>n){
        return -1;
    }
    res=readn(fd,buf,msg_len);
    if(res!=msg_len){
        return res<0? -1:0;
    }
    return res;
}