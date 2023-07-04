#ifndef MY_MESSAGE_OBJ
#define MY_MESSAGE_OBJ

struct message_obj{
    size_t type;
    char data[1024];
};

#define MSG_PING 1
#define MSG_PONG 2
#define MSG_TYPE1 11
#define MSG_TYPE2 21

#endif