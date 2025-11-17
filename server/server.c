#include "../network.h"
#include "../protocol.h"
#include "clients.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>
#include <poll.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <port>", argv[0]);
        exit(1);
    }
    
    int fdsize = STARTER_SERVER_SIZE;
    int fdcount = 0;
    
    struct pollfd *pfds = malloc(sizeof(struct pollfd) * fdsize);
    Client *clients = malloc(sizeof(Client) * fdsize);
    
    int listener = get_listener_fd(argv[1]);
    if (listener < 0) {
        fprintf(stderr, "failed to connect\n");
        exit(EXIT_FAILURE);
    }
    printf("server listening to port %s\n", argv[1]);
    
    clients[0].active = 1;
    clients[0].fd = listener;
    strcpy(clients[0].name, "server");
    pfds[0].fd = listener;
    pfds[0].events = POLLIN;
    fdcount++;

    while (1) {
        int poll_count = poll(pfds, fdcount, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }
        process_connections(&pfds, &clients, listener, &fdcount, &fdsize);
    }

    free(pfds);
    return 0;
}