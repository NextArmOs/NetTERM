#include "main.h"
#include <thread>

std::atomic<bool> g_running(true);
std::chrono::high_resolution_clock::time_point g_start_time;
std::mutex g_screen_mutex;

void inputThreadLoop() {
    std::string current_cmd = "";
    
    while (g_running) {
        {
            std::lock_guard<std::mutex> lock(g_screen_mutex);
            move(LINES - 1, 0);
            clrtoeol();
            attron(COLOR_PAIR(1));
            printw("root ~> ");
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            printw("%s", current_cmd.c_str());
            attroff(COLOR_PAIR(2));
            curs_set(1);
            refresh();
        }

        
        int ch = wgetch(stdscr);
        
        if (ch != ERR) {
            if (ch == '\n' || ch == '\r') {
                processCommand(current_cmd);
                current_cmd = "";
            } 
            else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                if (!current_cmd.empty()) {
                    current_cmd.pop_back();
                }
            } 
            else if (ch >= 32 && ch <= 126) {
                current_cmd += static_cast<char>(ch);
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    g_start_time = std::chrono::high_resolution_clock::now();

    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);

    bkgd(COLOR_PAIR(4));

    auto const frame_duration = std::chrono::milliseconds(33);

    std::thread input_thread(inputThreadLoop);

    while (g_running) {
        auto frame_start = std::chrono::high_resolution_clock::now();

        {
            std::lock_guard<std::mutex> lock(g_screen_mutex);
            attron(COLOR_PAIR(3));
            mvprintw(0, 1, "NetTERM System Shell Runtime v4.0.3");
            attroff(COLOR_PAIR(3));
            mvhline(1, 0, ACS_HLINE, COLS);
        }

        drawUptime();

        {
            std::lock_guard<std::mutex> lock(g_screen_mutex);
            refresh();
        }

        auto frame_end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start);

        if (elapsed < frame_duration) {
            std::this_thread::sleep_for(frame_duration - elapsed);
        }
    }

    g_running = false;
    if (input_thread.joinable()) {
        input_thread.join();
    }

    endwin();
    return 0;
}
