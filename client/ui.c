#include "ui.h"
#include "../network.h"
#include <ctype.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

WINDOW* input = NULL;
WINDOW* output = NULL;
WINDOW* sidebar = NULL;

void init_ui(int client_fd) {
    initscr();
    cbreak();
    noecho();

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    sidebar = newwin(yMax, xMax / 4, 0, 0);
    box(sidebar, 0, 0);
    mvwprintw(sidebar, 1, 1, "Users");
    wrefresh(sidebar);

    output = newwin(yMax - 5, (3 * xMax / 4) - 2, 0, (xMax / 4) + 2);
    box(output, 0, 0);
    int output_yMax, output_xMax;
    getmaxyx(output, output_yMax, output_xMax);
    mvwprintw(output, 1, (output_xMax/2)-2, "Chat");
    wrefresh(output);

    input = newwin(3, (3 * xMax / 4) - 2, yMax - 4, (xMax / 4) + 2);
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
    int out_h, out_w;
    getmaxyx(output, out_h, out_w);

    // Create or reuse inner text window (does not include border)
    static WINDOW *text = NULL;
    if (!text) {
        text = derwin(output, out_h - 2, out_w - 2, 1, 1);
        scrollok(text, TRUE);
        idlok(text, TRUE);
    }

    // If output resized between calls, we should ideally recreate 'text'.
    // (Skipping resize handling here for brevity.)

    // Prepare prefix
    char prefix[32];
    if (from_self) strcpy(prefix, "You: ");
    else prefix[0] = '\0';

    int usable_width = out_w - 2; // space inside border
    int current_len = 0;

    // Print prefix
    if (prefix[0]) {
        wprintw(text, "%s", prefix);
        current_len = strlen(prefix);
    }

    const char *p = msg;
    while (*p) {
        // Extract next word (sequence of non-space chars)
        char word[MAX_MESSAGE_LENGTH];
        int wi = 0;
        while (*p && *p != ' ' && wi < (int)sizeof(word) - 1) {
            word[wi++] = *p++;
        }
        word[wi] = '\0';
        int word_len = wi;

        // If the word itself is longer than usable_width, we will break it.
        if (word_len >= usable_width) {
            // If there's room in current line, print a part; otherwise newline first
            if (current_len > 0) {
                wprintw(text, "\n");
                current_len = 0;
            }
            // print the long word in chunks of usable_width
            int start = 0;
            while (start < word_len) {
                int chunk = usable_width;
                if (start + chunk > word_len) chunk = word_len - start;
                // write chunk
                for (int k = 0; k < chunk; k++) waddch(text, word[start + k]);
                start += chunk;
                // if still more, newline and continue
                if (start < word_len) {
                    wprintw(text, "\n");
                    current_len = 0;
                } else {
                    current_len = chunk;
                }
            }
        } else {
            // If the word doesn't fit on the current line, wrap before printing it
            if (current_len + word_len > usable_width) {
                wprintw(text, "\n");
                current_len = 0;
            }
            // print the word
            if (word_len > 0) {
                wprintw(text, "%s", word);
                current_len += word_len;
            }
        }

        // Skip and print single space if it fits; otherwise newline
        if (*p == ' ') {
            // lookahead: if next word can fit after a space, print space; else newline
            const char *q = p + 1;
            int next_len = 0;
            while (*q && *q != ' ') { next_len++; q++; }
            if (current_len + 1 + next_len <= usable_width) {
                wprintw(text, " ");
                current_len += 1;
            } else {
                wprintw(text, "\n");
                current_len = 0;
            }
            // consume the space(s)
            while (*p == ' ') p++;
        }
    }

    // Ensure the message ends with a newline so next message starts on a new line
    // (If current_len == 0, we're already at start of line)
    if (current_len != 0) {
        wprintw(text, "\n");
    }

    // Redraw border and refresh both windows
    box(output, 0, 0);
    wrefresh(text);
    wrefresh(output);
}



void handle_sending(char *msg, int client_fd) {
    ui_add_message(msg, true);
    send_message(client_fd, msg, strlen(msg));
}

void handle_receiving(char *msg) {
    ui_add_message(msg, false);
}
