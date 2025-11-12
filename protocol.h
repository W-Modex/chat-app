#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_MESSAGE_LENGTH 256

struct Client {
    char name[10];
    int fd;
};

#endif