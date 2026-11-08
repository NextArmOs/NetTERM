#ifndef NETNOTES_HPP
#define NETNOTES_HPP

#include <string>
#include <vector>

class NetNotes {
private:
    std::string text_buffer;

public:
    NetNotes();
    void append_char(char ch);
    void handle_backspace();
    void clear();
    std::vector<std::string> get_display_content(int win_width) const;
};

#endif
