#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <chrono>
#include <atomic>
#include <mutex>
#include <ncurses.h>

extern std::atomic<bool> g_running;
extern std::chrono::high_resolution_clock::time_point g_start_time;
extern std::mutex g_screen_mutex;

void drawUptime();
void processCommand(const std::string& cmd);
void inputThreadLoop();

#endif
