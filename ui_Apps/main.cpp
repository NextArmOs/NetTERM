#include "main.h"
#include <iostream>
#include <string>
#include <vector>
#include <ncurses.h>

void drawMenu(const std::vector<std::string>& options, int selected) {
    clear();
    int y = 10;
    for (size_t i = 0; i < options.size(); ++i) {
        if (i == selected) {
            attron(A_REVERSE);
        }
        mvprintw(y, 10, options[i].c_str());
        attroff(A_REVERSE);
        ++y;
    }
    refresh();
}

int getUserChoice(const std::vector<std::string>& options) {
    int ch;
    while (true) {
        drawMenu(options, 0);
        ch = getch();
        if (ch == KEY_UP) {
            return 1;
        } else if (ch == KEY_DOWN) {
            return 2;
        } else if (ch == 10) {
            return 3;
        } else if (ch == 'q' || ch == 'Q') {
            return 4;
        }
        std::cout << "Invalid choice. Please try again." << std::endl;
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(BUTTON1_CLICKED, NULL);

    std::vector<std::string> options = {"Option 1", "Option 2", "Option 3", "Exit"};
    int selected = 0;

    while (true) {
        drawMenu(options, selected);
        int ch = getch();

        if (ch == KEY_UP) {
            selected = 0;
        } else if (ch == KEY_DOWN) {
            selected = 1;
        } else if (ch == 10) {
            selected = 2;
        } else if (ch == 'q' || ch == 'Q') {
            selected = 3;
        }

        if (selected == 2) {
            mousemask(BUTTON1_CLICKED, NULL);
            int mouse_x, mouse_y;
            MEVENT event;
            while (getmouse(&event) == OK) {
                mouse_x = event.x;
                mouse_y = event.y;
                if (mouse_y == 12) {
                    selected = 0;
                } else if (mouse_y == 14) {
                    selected = 1;
                } else if (mouse_y == 16) {
                    selected = 2;
                } else if (mouse_y == 18) {
                    selected = 3;
                }
            }
            mousemask(BUTTON1_CLICKED, NULL);
        }

        if (selected == 3) {
            endwin();
            return 0;
        }
    }

    endwin();
    return 0;
}
