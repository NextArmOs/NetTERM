#ifndef TASKBAR_HPP
#define TASKBAR_HPP

#include <string>

class Taskbar {
private:
    bool menu_open;
    int start_btn_width;

public:
    Taskbar();
    void toggle_menu();
    void close_menu();
    bool is_menu_open() const;
    void draw(int term_width, int term_height, const std::string& status, const std::string& uptime_str);
    void draw_menu(int term_height);
    bool check_start_click(int mx, int my, int term_height);
    int check_menu_click(int mx, int my, int term_height); 
};

#endif
