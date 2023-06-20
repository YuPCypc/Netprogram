#include "common.h"

size_t readn(int fd,void* buf,size_t size){
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