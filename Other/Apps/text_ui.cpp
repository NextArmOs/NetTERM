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

int main() {
    std::vector<std::string> options = {"Option 1", "Option 2", "Option 3", "Exit"};
    int selected = 0;

    while (true) {
        drawMenu(options, selected);
        std::cout << "Enter your choice: ";
        int ch;
        std::cin >> ch;

        switch (ch) {
            case 1:
                selected = 0;
                break;
            case 2:
                selected = 1;
                break;
            case 3:
                selected = 2;
                break;
            case 4:
                std::cout << "Exiting program." << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}