#include "Taskbar.hpp"
#include <iostream>

Taskbar::Taskbar() : menu_open(false), start_btn_width(11) {}

void Taskbar::toggle_menu() {
    menu_open = !menu_open;
}

void Taskbar::close_menu() {
    menu_open = false;
}

bool Taskbar::is_menu_open() const {
    return menu_open;
}

void Taskbar::draw(int term_width, int term_height, const std::string& status, const std::string& uptime_str) {
    std::cout << "\033[" << (term_height - 1) << ";1H";
    for (int i = 0; i < term_width; ++i) std::cout << "=";
    
    std::cout << "\033[" << term_height << ";1H\033[K";
    std::cout << "[ Start ]  |  " << status;
    
    int info_pos = term_width - 45;
    if (info_pos > 30) {
        std::cout << "\033[" << term_height << ";" << info_pos << "HUp: " << uptime_str << " | NetOS 0.0.7";
    }
}

void Taskbar::draw_menu(int term_height) {
    if (!menu_open) return;

    int menu_y = term_height - 10; 
    std::cout << "\033[" << menu_y << ";1H+-----------------+";
    std::cout << "\033[" << (menu_y + 1) << ";1H|  Calculator |";
    std::cout << "\033[" << (menu_y + 2) << ";1H|  Uptime App |";
    std::cout << "\033[" << (menu_y + 3) << ";1H|  Task Manager|";
    std::cout << "\033[" << (menu_y + 4) << ";1H|  NetNotes   |";
    std::cout << "\033[" << (menu_y + 5) << ";1H|  Help Menu  |";
    std::cout << "\033[" << (menu_y + 6) << ";1H|  Clear Desk |";
    std::cout << "\033[" << (menu_y + 7) << ";1H|  ---------------|";
    std::cout << "\033[" << (menu_y + 8) << ";1H|  [X] Shutdown   |";
    std::cout << "\033[" << (menu_y + 9) << ";1H+-----------------+";
}

bool Taskbar::check_start_click(int mx, int my, int term_height) {
    return (my == term_height && mx >= 1 && mx <= start_btn_width);
}

int Taskbar::check_menu_click(int mx, int my, int term_height) {
    if (!menu_open) return 0;
    int menu_y = term_height - 10;
    
    if (mx >= 1 && mx <= 19) {
        if (my == menu_y + 1) return 1; 
        if (my == menu_y + 2) return 2; 
        if (my == menu_y + 3) return 3; 
        if (my == menu_y + 4) return 4; 
        if (my == menu_y + 5) return 5; 
        if (my == menu_y + 6) return 6; 
        if (my == menu_y + 8) return 7; 
    }
    return 0;
}
