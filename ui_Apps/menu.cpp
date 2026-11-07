#include "main.h"
#include <vector>

static std::vector<std::string> cmd_log;

void processCommand(const std::string& cmd) {
    if (cmd.empty()) return;

    std::lock_guard<std::mutex> lock(g_screen_mutex);

    if (cmd == "exit" || cmd == "quit") {
        g_running = false;
    } 
    else if (cmd == "help") {
        cmd_log.push_back("Available commands: help, sysinfo, clear, exit");
    } 
    else if (cmd == "clear") {
        cmd_log.clear();
        wclear(stdscr);
    } 
    else if (cmd == "sysinfo") {
#if defined(__x86_64__)
        cmd_log.push_back("CPU Architecture: x86_64");
#else
        cmd_log.push_back("CPU Architecture: ARM64/Other");
#endif
        cmd_log.push_back("OS: Linux"); //Works only with linux ^-^
    } 
    else {
        cmd_log.push_back("netsh: command not found: " + cmd);
    }

    int start_row = 3;
    for (size_t i = 0; i < cmd_log.size(); ++i) {
        if (start_row + i < (size_t)LINES - 2) {
            mvprintw(start_row + i, 2, "%s", cmd_log[i].c_str());
        }
    }
}
