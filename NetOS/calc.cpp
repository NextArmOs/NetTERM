#include "calc.hpp"
#include <sstream>

NetCalc::NetCalc() : current_expr(""), result_str("0") {}

void NetCalc::append_char(char ch) {
    if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == '*' || ch == '/') {
        current_expr += ch;
    }
}

void NetCalc::clear() {
    current_expr = "";
    result_str = "0";
}

void NetCalc::calculate() {
    if (current_expr.empty()) return;
    
    std::stringstream ss(current_expr);
    double total = 0;
    double next_num = 0;
    char op = '+';

    if (ss >> total) {
        while (ss >> op >> next_num) {
            if (op == '+') total += next_num;
            else if (op == '-') total -= next_num;
            else if (op == '*') total *= next_num;
            else if (op == '/') {
                if (next_num != 0) total /= next_num;
                else {
                    result_str = "Error: Div by 0";
                    return;
                }
            }
        }
        std::stringstream res_ss;
        res_ss << total;
        result_str = res_ss.str();
    } else {
        result_str = "Error";
    }
}

std::vector<std::string> NetCalc::get_display_content() const {
    return {
        " Expr: " + current_expr,
        " Result: " + result_str,
        "---------------------",
        " [0-9] Num  [+-*/] Op",
        " [=] Eval   [c] Clear"
    };
}
