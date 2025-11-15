#ifndef UI_H
#define UI_H

#include <ncurses.h>

void init_ui();
void handle_input(WINDOW*);
void update_sidebar();
void handle_receiving();
void handle_sending();

#endif