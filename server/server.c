#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <port>", argv[0]);
    }
    int socket_fd = get_listener_fd(argv[1]);
    if (socket_fd < 0) {
        fprintf(stderr, "failed to connect\n");
        exit(EXIT_FAILURE);
    }
    printf("server listening to port %s with fd: %d\n", argv[1], socket_fd);
    int client_fd = accept_client(socket_fd);
    if (client_fd < 0) {
        fprintf(stderr, "failed to accept client");
    }
    printf("a client joined with fd: %d", client_fd);
    close(socket_fd);
    close(client_fd);
    return 0;
}