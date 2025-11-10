#include "network.h"
#include <asm-generic/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int get_listener_fd(char* port) {
    struct addrinfo hints, *res, *p;
    int yes = 1;
    int socket_fd;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((getaddrinfo(NULL, port, &hints, &res)) != 0) {
        perror("server: getaddrinfo");
        exit(1);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            perror("socket");
            continue;
        }
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            return -1;
        }
        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("bind");
            close(socket_fd);
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    if (p == NULL) {
        close(socket_fd);
        return -1;
    }
    if (listen(socket_fd, 5) == -1) {
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}

int accept_client(int socket_fd) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);
    return client_fd;
}

int send_message(int client_fd, const char *msg, int msg_size) {
    int bytes_sent = send(client_fd, msg, msg_size, 0);
    if (bytes_sent < 0) {
        return -1;
    }
    return bytes_sent;
}

int recv_message(int client_fd, char *buf, int buf_size) {
    int bytes_recv = recv(client_fd, buf, buf_size, 0);
    if (bytes_recv < 0) {
        return -1;
    }
    return bytes_recv;
}