#include "os.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>

AsciiOS::AsciiOS() : term_width(80), term_height(24), running(true), 
                     system_status("Ready"), active_win_idx(-1), 
                     is_dragging(false), drag_offset_x(0), drag_offset_y(0),
                     is_resizing(false), start_width(0), start_height(0) {}

void AsciiOS::update_terminal_size() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    term_width = w.ws_col;
    term_height = w.ws_row;
}

void AsciiOS::init() {
    update_terminal_size();
    Window welcomeWin;
    welcomeWin.title = " Welcome to NetOS 0.0.7 ";
    welcomeWin.x = 5;
    welcomeWin.y = 4;
    welcomeWin.width = 44;
    welcomeWin.height = 10;
    welcomeWin.content = {
        "OS: NetOS v0.0.7",
        "DE: NetUI",
        "New: NetNotes app included!",
        "Try: Type in notepad directly",
        "------------------------------------------",
        "Click '[ Start ]' to open Menu!"
    };
    windows.push_back(welcomeWin);
}

void AsciiOS::clear_screen() {
    std::cout << "\033[2J\033[H";
}

void AsciiOS::draw_background() {
    for (int y = 0; y < term_height - 2; ++y) {
        for (int x = 0; x < term_width; ++x) {
            if (y % 2 == 0 && x % 4 == 0) std::cout << ".";
            else std::cout << " ";
        }
        std::cout << "\n";
    }
}

void AsciiOS::draw_window(const Window& win) {
    if (win.y >= term_height - 2 || win.x >= term_width) return;
    
    std::cout << "\033[" << win.y << ";" << win.x << "H+";
    for (int i = 0; i < win.width - 2; ++i) {
        if (win.x + i + 1 < term_width) std::cout << "-";
    }
    if (win.x + win.width - 1 < term_width) std::cout << "+";

    int title_pos = win.x + (win.width - win.title.length()) / 2;
    if (title_pos > 0 && title_pos < term_width) {
        std::cout << "\033[" << win.y << ";" << title_pos << "H" << win.title;
    }

    if (win.x + win.width - 4 < term_width) {
        std::cout << "\033[" << win.y << ";" << (win.x + win.width - 4) << "H[X]";
    }

    for (int i = 0; i < win.height - 2; ++i) {
        if (win.y + 1 + i >= term_height - 2) break;
        
        std::cout << "\033[" << (win.y + 1 + i) << ";" << win.x << "H|";
        if (win.x + win.width - 1 < term_width) {
            std::cout << "\033[" << (win.y + 1 + i) << ";" << (win.x + win.width - 1) << "H|";
        }
        
        if (i < win.content.size()) {
            std::cout << "\033[" << (win.y + 1 + i) << ";" << (win.x + 2) << "H" << win.content[i];
        }
    }

    int bottom_y = win.y + win.height - 1;
    if (bottom_y < term_height - 2) {
        std::cout << "\033[" << bottom_y << ";" << win.x << "H+";
        for (int i = 0; i < win.width - 2; ++i) {
            if (win.x + i + 1 < term_width) std::cout << "-";
        }
        if (win.x + win.width - 1 < term_width) std::cout << "+";
    }
}

void AsciiOS::draw_desktop() {
    update_terminal_size();
    clear_screen();
    draw_background();
    
    for (const auto& win : windows) {
        draw_window(win);
    }
    
    taskbar.draw_menu(term_height);
    taskbar.draw(term_width, term_height, system_status, uptime_monitor.get_formatted_time());
    
    std::cout << "\033[" << (term_height - 2) << ";1H\033[Knetos@user > " << current_input;
    std::flush(std::cout);
}

void AsciiOS::set_nonblocking_mode(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

void AsciiOS::enable_mouse(bool enable) {
    if (enable) {
        std::cout << "\033[?1000h\033[?1003h";
    } else {
        std::cout << "\033[?1003l\033[?1000l";
    }
    std::flush(std::cout);
}

void AsciiOS::handle_mouse_click(int btn, int mx, int my, bool is_release) {
    if (is_release || btn == 3 || btn == 67) {
        is_dragging = false;
        is_resizing = false;
        active_win_idx = -1;
        return;
    }

    if (btn == 0) { 
        if (taskbar.check_start_click(mx, my, term_height)) {
            taskbar.toggle_menu();
            system_status = taskbar.is_menu_open() ? "Menu opened" : "Ready";
            return;
        }

        int menu_action = taskbar.check_menu_click(mx, my, term_height);
        if (menu_action != 0) {
            taskbar.close_menu();
            if (menu_action == 1) handle_command("calc");
            if (menu_action == 2) handle_command("uptime");
            if (menu_action == 3) handle_command("taskmgr");
            if (menu_action == 4) handle_command("notes");
            if (menu_action == 5) handle_command("help");
            if (menu_action == 6) handle_command("clear");
            if (menu_action == 7) handle_command("exit");
            return;
        }

        taskbar.close_menu();

        for (int i = windows.size() - 1; i >= 0; --i) {
            const auto& win = windows[i];
            
            if (my == win.y && mx >= (win.x + win.width - 4) && mx < (win.x + win.width)) {
                system_status = "Closed window: " + win.title;
                windows.erase(windows.begin() + i);
                return;
            }
            
            if (mx == (win.x + win.width - 1) && my == (win.y + win.height - 1)) {
                is_resizing = true;
                active_win_idx = i;
                system_status = "Resizing: " + win.title;
                return;
            }

            if (mx >= win.x && mx < win.x + win.width && my == win.y) {
                is_dragging = true;
                active_win_idx = i;
                drag_offset_x = mx - win.x;
                drag_offset_y = my - win.y;
                system_status = "Dragging: " + win.title;
                return;
            }
        }
    }
}

void AsciiOS::handle_mouse_move(int mx, int my) {
    if (active_win_idx >= 0 && active_win_idx < windows.size()) {
        if (is_dragging) {
            windows[active_win_idx].x = mx - drag_offset_x;
            windows[active_win_idx].y = my - drag_offset_y;
        } 
        else if (is_resizing) {
            int new_width = mx - windows[active_win_idx].x + 1;
            int new_height = my - windows[active_win_idx].y + 1;
            
            if (new_width >= 15) windows[active_win_idx].width = new_width;
            if (new_height >= 5) windows[active_win_idx].height = new_height;
        }
    }
}

void AsciiOS::update_dynamic_windows() {
    for (auto& win : windows) {
        if (win.is_calc) {
            win.content = calculator.get_display_content();
        }
        if (win.is_uptime) {
            win.content = uptime_monitor.get_display_content();
        }
        if (win.is_taskmgr) {
            win.content = task_manager.get_display_content(windows.size());
        }
        if (win.is_notes) {
            win.content = notepad.get_display_content(win.width);
        }
    }
}

void AsciiOS::handle_command(const std::string& cmd) {
    if (cmd == "exit" || cmd == "shutdown") {
        running = false;
    } else if (cmd == "clear") {
        windows.clear();
        system_status = "Desktop cleared";
    } else if (cmd == "help") {
        Window helpWin = {" Help Menu ", 42, 4, 32, 9, {"Commands:", "help    - show this", "clear   - close windows", "calc    - open NetCalc", "uptime  - open Timer", "taskmgr - open Monitor", "notes   - NetNotes Notepad", "exit    - close OS"}};
        windows.push_back(helpWin);
    } else if (cmd == "calc") {
        Window calcWin = {" NetCalc 3.0 ", 25, 8, 27, 7, {}, true, false, false, false};
        windows.push_back(calcWin);
        update_dynamic_windows();
        system_status = "Calculator active.";
    } else if (cmd == "uptime") {
        Window upWin = {" NetUptime ", 15, 6, 31, 7, {}, false, true, false, false};
        windows.push_back(upWin);
        update_dynamic_windows();
        system_status = "Uptime App active.";
    } else if (cmd == "taskmgr") {
        Window tmWin = {" Task Manager ", 35, 10, 29, 9, {}, false, false, true, false};
        windows.push_back(tmWin);
        update_dynamic_windows();
        system_status = "Task Manager active.";
    } else if (cmd == "notes") {
        Window ntWin = {" NetNotes 1.0 ", 20, 5, 35, 10, {}, false, false, false, true};
        windows.push_back(ntWin);
        update_dynamic_windows();
        system_status = "Notepad active. Just type!";
    }
}

void AsciiOS::run() {
    set_nonblocking_mode(true);
    enable_mouse(true);
    init();
    
    while (running) {
        update_dynamic_windows();
        draw_desktop();
        
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &fds)) {
            char c;
            if (read(STDIN_FILENO, &c, 1) > 0) {
                if (c == '\033') { 
                    char next1, next2;
                    if (read(STDIN_FILENO, &next1, 1) > 0 && read(STDIN_FILENO, &next2, 1) > 0) {
                        if (next1 == '[' && next2 == 'M') { 
                            char b, x, y;
                            read(STDIN_FILENO, &b, 1);
                            read(STDIN_FILENO, &x, 1);
                            read(STDIN_FILENO, &y, 1);
                            
                            int btn = b - 32;
                            int mx = x - 32;
                            int my = y - 32;
                            
                            bool is_release = (btn == 3 || btn == 67); 
                            bool is_move = (btn & 32);
                            if (is_move) {
            handle_mouse_move(mx, my);
            } else {
            handle_mouse_click(btn & 3, mx, my, is_release);
    }
  }
}
} else if (c == '\n' || c == '\r') {
    handle_command(current_input);
  current_input = "";
} else if (c == 127) {
bool notes_active = false;
for (const auto& w : windows) { if (w.is_notes) notes_active = true; }
if (notes_active) {
notepad.handle_backspace();
} else {
if (!current_input.empty()) current_input.pop_back();
}
} else {
bool notes_active = false;
for (const auto& w : windows) { if (w.is_notes) notes_active = true; }
if (notes_active) {
if (c == 'i') notepad.clear();
else notepad.append_char(c);
update_dynamic_windows();
} else if (c == 'c' || c == '=' || (c >= '0' && c <= '9') || c == '+' || c == '-' || c == '*' || c == '/') {
if (c == 'c') calculator.clear();
else if (c == '=') calculator.calculate();
else calculator.append_char(c);
update_dynamic_windows();
} else {
current_input += c;
}
}
}
}
}
enable_mouse(false);
set_nonblocking_mode(false);
clear_screen();
std::cout << "Goodbye! Returning shell...\n";
}
