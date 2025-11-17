#include "../network.h"
#include "clients.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_connections(struct pollfd **pfds, Client **clients, int listener, int *fdcount, int *fdsize) {
    for (int i = 0; i < *fdcount; i++) {
        if ((*pfds)[i].revents & (POLLIN | POLLHUP)) {
            if ((*pfds)[i].fd == listener) {
                add_connection(pfds, clients, listener, fdcount, fdsize);
            } else {
                handle_client(pfds, clients, listener, i, fdcount);
            }
        }
    }
}

void broadcast(struct pollfd ** pfds, int listener, int idx, int* fdcount, char* msg) {
    for (int i = 0; i < *fdcount; i++) {
        if ((*pfds)[i].fd == listener || idx == i) continue;
        int bytes_sent = send_message((*pfds)[i].fd, msg, strlen(msg));
        if (bytes_sent == -1) {  
            perror("failed to send message");
            exit(1);
        }
    }
}

void add_connection(struct pollfd **pfds, Client** clients, int listener, int *fdcount, int *fdsize) {
    Client new_client = accept_client(listener);
    if (new_client.fd < 0) {
        perror("failed to accept client");
        exit(1);
    }

    if (*fdcount == *fdsize) {
        *fdsize *= 2;
        *pfds = realloc(*pfds, *fdsize * sizeof(struct pollfd));
        *clients = realloc(*clients, *fdsize * sizeof(Client));
    }

    (*clients)[*fdcount] = new_client;
    (*pfds)[*fdcount].fd = new_client.fd;
    (*pfds)[*fdcount].events = POLLIN;
    (*pfds)[*fdcount].revents = 0;
    (*fdcount)++;
    char msg[MAX_NAME_LENGTH+25];
    sprintf(msg, "SERVER: %s has joined the chat", new_client.name);
    broadcast(pfds, listener, -1, fdcount, msg);
}

void handle_client(struct pollfd **pfds, Client** clients, int listener, int idx, int *fdcount) {
    char buf[MAX_MESSAGE_LENGTH];
    int bytes_recv = recv_message((*clients)[idx].fd, buf,   MAX_MESSAGE_LENGTH-1);

    if (bytes_recv < 0) {
        perror("failed to recv message");
        exit(1);
    }

    //connection closed
    if (bytes_recv == 0) {
        close_connection(pfds, clients, (*clients)[idx].fd, fdcount);
        return;
    }

    buf[bytes_recv] = '\0';

    char msg[MAX_NAME_LENGTH+MAX_MESSAGE_LENGTH];
    sprintf(msg, "%s: %s", (*clients)[idx].name, buf);

    broadcast(pfds, listener, idx, fdcount, msg);

    printf("the message: \"%s\" has been broadcasted\n", buf);
    fflush(stdout);
}

void close_connection(struct pollfd **pfds, Client** clients, int client_fd, int *fdcount) {
    char name[MAX_NAME_LENGTH];
    for (int i = 0; i < *fdcount; i++) {
        if ((*pfds)[i].fd == client_fd) {
            (*pfds)[i] = (*pfds)[*fdcount-1];
            (*clients)[i] = (*clients)[*fdcount-1];
            strcpy(name, (*clients)[i].name);
            (*fdcount)--;
            break;
        }
    }
    char msg[MAX_MESSAGE_LENGTH];
    sprintf(msg, "SERVER: %s has left the chat", name);
    printf("%s has closed the connection", name);
    broadcast(pfds, (*clients)[0].fd,  -1, fdcount, msg);
    fflush(stdout);
}