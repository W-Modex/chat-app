#ifndef CLIENTS_H
#define CLIENTS_H

#include <stdlib.h>
#include <sys/poll.h>

void process_connections(struct pollfd** pfds, int listener, int* fdcount, int* fdsize);
void add_connection(struct pollfd** pfds, int listener, int* fdcount, int* fdsize);
void handle_client(struct pollfd** pfds, int listener, int client_fd, int* fdcount);
void close_connection(struct pollfd** pfds, int client_fd, int* fdcount);

#endif