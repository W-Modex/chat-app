#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_MESSAGE_LENGTH 256
#define STARTER_SERVER_SIZE 10
#define MAX_NAME_LENGTH 15

typedef struct {
    char name[MAX_NAME_LENGTH];
    int fd;
    int active;
} Client;

#endif