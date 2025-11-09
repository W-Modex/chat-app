#include "network.h"
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int connect_to(char *ip, char *port) {
    struct addrinfo hints, *res, *p;
    int socket_fd;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(ip, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_fd == -1)
            continue;

        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == 0)
            break;

        close(socket_fd);
    }

    freeaddrinfo(res);

    if (p == NULL) {
        perror("connect");
        return -1;
    }

    return socket_fd;
}
