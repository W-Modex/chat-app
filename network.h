#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>
#include "protocol.h"

int get_listener_fd(char* port);
Client accept_client(int listener);
int send_message(int client_fd, const char* msg, int size);
int recv_message(int client_fd, char* buffer, int size);
int connect_to(char* ip, char* port);

#endif
