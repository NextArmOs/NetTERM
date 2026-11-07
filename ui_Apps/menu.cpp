#include "menu.h"
#include <iostream>
#include <string>
#include <vector>

void drawMenu(const std::vector<std::string>& options, int selected) {
    std::cout << "Menu:\n";
    for (size_t i = 0; i < options.size(); ++i) {
        if (i == selected) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
        std::cout << options[i] << std::endl;
    }
}
