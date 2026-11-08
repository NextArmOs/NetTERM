#include "NetNotes.hpp"

NetNotes::NetNotes() : text_buffer("") {}

void NetNotes::append_char(char ch) {
    if (ch >= 32 && ch <= 126) {
        text_buffer += ch;
    }
}

void NetNotes::handle_backspace() {
    if (!text_buffer.empty()) {
        text_buffer.pop_back();
    }
}

void NetNotes::clear() {
    text_buffer = "";
}

std::vector<std::string> NetNotes::get_display_content(int win_width) const {
    std::vector<std::string> lines;
    int max_chars = win_width - 4;
    if (max_chars < 5) max_chars = 5;

    lines.push_back(" [i] Clear Notepad ");
    lines.push_back(" ------------------ ");

    if (text_buffer.empty()) {
        lines.push_back(" Type anything...");
        return lines;
    }

    std::string current_line = " ";
    for (char ch : text_buffer) {
        current_line += ch;
        if (current_line.length() >= max_chars) {
            lines.push_back(current_line);
            current_line = " ";
        }
    }
    if (current_line.length() > 1) {
        lines.push_back(current_line);
    }

    return lines;
}
