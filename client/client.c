#include "../network.h"
#include "ui.h"
#include <bits/pthreadtypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void* receiving_routine(void* arg) {
    int fd =  *((int*)arg);
    char buf[MAX_MESSAGE_LENGTH];
    int bytes_recv;
    while (1) {
        bytes_recv = recv_message(fd, buf, sizeof(buf)-1);
        if (bytes_recv < 0) {
            perror("failed to recv msg\n");
            exit(1);
        }
        if (bytes_recv == 0) {
            printf("Server has been closed\n");
            break;
        }
        buf[bytes_recv] = '\0';
        handle_receiving(buf);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    pthread_t recv_thread;
    Client client;
    char buf[MAX_MESSAGE_LENGTH];

    client.fd = connect_to(argv[1], argv[2]);
    if (client.fd < 0) {
        fprintf(stderr, "Failed to connect.\n");
        exit(EXIT_FAILURE);
    }

    recv_message(client.fd, buf, MAX_MESSAGE_LENGTH);
    if (strncmp(buf, "NAME?", 5) == 0) {
        
        printf("Choose a name: ");
        fgets(client.name, MAX_NAME_LENGTH, stdin);

        client.name[strcspn(client.name, "\n")] = 0;

        char msg[6 + MAX_MESSAGE_LENGTH];
        sprintf(msg, "NAME:%s\n", client.name);

        send_message(client.fd, msg, strlen(msg));

        printf("Connected successfully as [%s]\n", client.name);
    }

    pthread_create(&recv_thread, NULL, receiving_routine, (void*) &client.fd);
    init_ui(client.fd);


    close(client.fd);
    return 0;
}