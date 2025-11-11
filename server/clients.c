#include "clients.h"
#include "../network.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>

void process_connections(struct pollfd **pfds, int listener, int *fdcount, int *fdsize) {
    for (int i = 0; i < *fdcount; i++) {
        if ((*pfds)[i].revents & (POLLIN | POLLHUP)) {
            if ((*pfds)[i].fd == listener) {
                add_connection(pfds, listener, fdcount, fdsize);
            } else {
                handle_client(pfds, listener, (*pfds)[i].fd, fdcount);
            }
        }
    }
}

void add_connection(struct pollfd **pfds, int listener, int *fdcount, int *fdsize) {
    int new_clientfd = accept_client(listener);
    if (new_clientfd < 0) {
        perror("failed to accept client");
        exit(1);
    }

    if (*fdcount == *fdsize) {
        *fdsize *= 2;
        *pfds = realloc(*pfds, *fdsize * sizeof(struct pollfd));
    }
    (*pfds)[*fdcount].fd = new_clientfd;
    (*pfds)[*fdcount].events = POLLIN;
    (*pfds)[*fdcount].revents = 0;
    (*fdcount)++;
    printf("client added successfully\n");
}

void handle_client(struct pollfd **pfds, int listener, int client_fd, int *fdcount) {
    char buf[256];
    int bytes_recv = recv_message(client_fd, buf, 255);

    if (bytes_recv < 0) {
        perror("failed to recv message");
        exit(1);
    }

    //connection closed
    if (bytes_recv == 0) {
        close_connection(pfds, client_fd, fdcount);
        return;
    }

    buf[bytes_recv] = '\0';

    //broadcast for every client
    for (int i = 0; i < *fdcount; i++) {
        if ((*pfds)[i].fd == client_fd || (*pfds)[i].fd == listener) continue;
        int bytes_sent = send_message((*pfds)[i].fd, buf, bytes_recv);
        if (bytes_sent == -1) {
            perror("failed to send message");
            exit(1);
        }
    }
    printf("the message: \"%s\"has been broadcasted\n", buf);
    fflush(stdout);
}

void close_connection(struct pollfd **pfds, int client_fd, int *fdcount) {
    for (int i = 0; i < *fdcount; i++) {
        if ((*pfds)[i].fd == client_fd) {
            (*pfds)[i] = (*pfds)[*fdcount-1];
            (*fdcount)--;
            break;
        }
    }
    printf("client (fd = %d) has closed connection", client_fd);
    fflush(stdout);
}