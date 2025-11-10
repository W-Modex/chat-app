#include "network.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int client_fd = connect_to(argv[1], argv[2]);
    if (client_fd < 0) {
        fprintf(stderr, "Failed to connect.\n");
        exit(EXIT_FAILURE);
    }

    printf("Connected successfully (fd = %d)\n", client_fd);
    close(client_fd);
    return 0;
}