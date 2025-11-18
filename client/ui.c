#include "ui.h"
#include "../network.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

WINDOW* input = NULL;
WINDOW* output = NULL;

void init_ui(int client_fd) {
    initscr();
    cbreak();
    noecho();

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    output = newwin(yMax - 3, xMax, 0, 0);
    box(output, 0, 0);
    int output_yMax, output_xMax;
    getmaxyx(output, output_yMax, output_xMax);
    mvwprintw(output, 1, (output_xMax/2)-2, "Chat");
    wrefresh(output);

    input = newwin(3, xMax, yMax - 3, 0);
    box(input, 0, 0);
    mvwprintw(input, 1, 1, ">");
    keypad(input, true);
    wrefresh(input);
    handle_input(client_fd);
    endwin();
}

void handle_input(int client_fd) {
    char buf[MAX_MESSAGE_LENGTH];
    int pos = 0;
    int yMax, xMax;
    getmaxyx(input, yMax, xMax);
    while (1) {
        int c = wgetch(input);
        if (c == '\n') {
            if (pos == 0) continue;
            buf[pos] = '\0';
            handle_sending(buf, client_fd);
            werase(input);
            box(input, 0, 0);
            mvwprintw(input, 1, 1, "> ");
            wmove(input, 1, 2);
            pos = 0;
            buf[0] = '\0';
            wrefresh(input);
        } else if (c == KEY_BACKSPACE || c == 127 || c == '\b') {
            if (pos <= 0) continue;
            pos--;
            buf[pos] = '\0';
            int y, x;
            getyx(input, y, x);
            mvwaddch(input, y, x - 1, ' ');
            wmove(input, y, x - 1);
            wrefresh(input);
        } else if (isprint(c) && pos < xMax-3 && pos < MAX_MESSAGE_LENGTH) {
            buf[pos++] = c;
            waddch(input, c);
            wrefresh(input);
        }
    }
}

void ui_add_message(const char *msg, bool from_self) {
    int yMax, xMax;
    getmaxyx(output, yMax, xMax);

    static WINDOW *text = NULL;
    if (!text) {
        text = derwin(output, yMax - 2, xMax - 2, 2, 1);
        scrollok(text, TRUE);
        idlok(text, TRUE);
    }

    // prefix
    if (from_self) wprintw(text, "You: ");

    int usable = xMax - 2;
    int col = from_self ? 5 : 0;

    const char *p = msg;

    while (*p) {
        char word[MAX_MESSAGE_LENGTH];
        int i = 0;

        while (p[i] && p[i] != ' ' && i < usable - 1) {
            word[i] = p[i];
            i++;
        }
        word[i] = '\0';

        if (i == 0) break;
        p += i;

        if (col + i > usable) {
            wprintw(text, "\n");
            col = 0;
        }
        wprintw(text, "%s", word);
        col += i;

        while (*p == ' ') {
            const char *q = p + 1;
            int next = 0;
            while (q[next] && q[next] != ' ') next++;
            if (col + 1 + next > usable) {
                wprintw(text, "\n");
                col = 0;
            } else {
                wprintw(text, " ");
                col++;
            }
            p++;
        }
    }

    wprintw(text, "\n");

    wrefresh(text);
    box(output, 0, 0);
    wrefresh(output);
}


void handle_sending(char *msg, int client_fd) {
    ui_add_message(msg, true);
    send_message(client_fd, msg, strlen(msg));
}

void handle_receiving(char *msg) {
    ui_add_message(msg, false);
}