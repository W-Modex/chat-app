#ifndef UI_H
#define UI_H

#include <ncurses.h>

extern WINDOW *output;
extern WINDOW *input;

void init_ui(int);
void handle_input(int);
void ui_add_message(const char *msg, bool from_self);
void handle_receiving(char* msg);
void handle_sending(char* msg, int client_fd);

#endif