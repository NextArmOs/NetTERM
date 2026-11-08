#ifndef CALC_HPP
#define CALC_HPP

#include <string>
#include <vector>

class NetCalc {
private:
    std::string current_expr;
    std::string result_str;

public:
    NetCalc();
    void append_char(char ch);
    void clear();
    void calculate();
    std::vector<std::string> get_display_content() const;
};

#endif
