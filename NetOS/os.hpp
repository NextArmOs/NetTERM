#ifndef OS_HPP
#define OS_HPP

#include <string>
#include <vector>
#include "calc.hpp"
#include "Uptime.hpp"
#include "Taskbar.hpp"
#include "TaskManager.hpp"
#include "NetNotes.hpp"

struct Window {
    std::string title;
    int x, y;
    int width, height;
    std::vector<std::string> content;
    bool is_calc = false;
    bool is_uptime = false;
    bool is_taskmgr = false;
    bool is_notes = false;
};

class AsciiOS {
private:
    int term_width;
    int term_height;
    bool running;
    std::string current_input;
    std::string system_status;
    std::vector<Window> windows;
    NetCalc calculator;
    Uptime uptime_monitor;
    Taskbar taskbar;
    TaskManager task_manager;
    NetNotes notepad;
    
    int active_win_idx;
    bool is_dragging;
    int drag_offset_x;
    int drag_offset_y;

    bool is_resizing;
    int start_width;
    int start_height;

    void update_terminal_size();
    void clear_screen();
    void draw_background();
    void draw_window(const Window& win);
    void draw_desktop();
    void set_nonblocking_mode(bool enable);
    void enable_mouse(bool enable);
    void handle_mouse_click(int btn, int x, int y, bool is_release);
    void handle_mouse_move(int x, int y);
    
public:
    AsciiOS();
    void init();
    void run();
    void handle_command(const std::string& cmd);
    void update_dynamic_windows();
};

#include "TaskManager.cpp"
#include "NetNotes.cpp"

#endif
