#ifndef CLIENTS_H
#define CLIENTS_H

#include <stdlib.h>
#include <sys/poll.h>
#include "../protocol.h"

void process_connections(struct pollfd** pfds, Client** clients, int listener, int* fdcount, int* fdsize);
void broadcast(struct pollfd ** pfds, int listener, int idx, int* fdcount, char* msg);
void add_connection(struct pollfd** pfds, Client** clients, int listener, int* fdcount, int* fdsize);
void handle_client(struct pollfd** pfds, Client** clients, int listener, int client_fd, int* fdcount);
void close_connection(struct pollfd** pfds, Client** clients,int client_fd, int* fdcount);

#endif