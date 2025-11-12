#include "../network.h"
#include "../protocol.h"
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
        printf("%s", buf);
        fflush(stdout);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    pthread_t recv_thread;
    struct Client client;

    client.fd = connect_to(argv[1], argv[2]);
    if (client.fd < 0) {
        fprintf(stderr, "Failed to connect.\n");
        exit(EXIT_FAILURE);
    }

    printf("Choose a name: ");
    fgets(client.name, 10, stdin);

    printf("Connected successfully (name = %s)\n", client.name);
    

    // TODO: add ncurses ui

    pthread_create(&recv_thread, NULL, receiving_routine, (void*) &client.fd);
    

    char buf[MAX_MESSAGE_LENGTH];
    int bytes_sent;
    while (1) {
        fflush(stdout);
        fgets(buf, MAX_MESSAGE_LENGTH, stdin);
        bytes_sent = send_message(client.fd, buf, strlen(buf));
        if (bytes_sent < 0) {
            perror("failed to send msg");
            exit(1);
        }
    }

    close(client.fd);
    return 0;
}