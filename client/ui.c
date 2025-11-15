#include "ui.h"
#include <ncurses.h>
#include <stdbool.h>

void init_ui() {
    initscr();
    cbreak();
    noecho();

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    WINDOW *sidebar = newwin(yMax, xMax / 4, 0, 0);
    box(sidebar, 0, 0);
    mvwprintw(sidebar, 1, 1, "Users");
    wrefresh(sidebar);

    WINDOW *output = newwin(yMax - 5, (3 * xMax / 4) - 2, 0, (xMax / 4) + 2);
    box(output, 0, 0);
    mvwprintw(output, 1, 1, "hello");
    wrefresh(output);

    WINDOW *input = newwin(3, (3 * xMax / 4) - 2, yMax - 4, (xMax / 4) + 2);
    box(input, 0, 0);
    mvwprintw(input, 1, 1, ">");
    keypad(input, true);
    wrefresh(input);
    handle_input(input);
    endwin();
}

void handle_input(WINDOW* input) {
    while (1) {
        char c = wgetch(input);
        switch(c) {
            case KEY_DL:
                break;
            case KEY_LEFT:
                break;
            case KEY_RIGHT:
                break;
        }
    }
}