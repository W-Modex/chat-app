#include "network.h"
#include "protocol.h"
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

Client accept_client(int listener) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    Client new_client;

    new_client.fd = accept(listener, (struct sockaddr*)&client_addr, &client_addr_size);
    new_client.name[0] = '\0';

    send_message(new_client.fd, "NAME?\n", 6);

    char buffer[MAX_MESSAGE_LENGTH];
    int n = recv_message(new_client.fd, buffer, sizeof(buffer));

    if (n > 0 && strncmp(buffer, "NAME:", 5) == 0) {
        strncpy(new_client.name, buffer + 5, MAX_NAME_LENGTH - 1);

        new_client.name[strcspn(new_client.name, "\n")] = 0;
    } else {
        strcpy(new_client.name, "Unknown");
    }
    new_client.active = 1;
    return new_client;
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