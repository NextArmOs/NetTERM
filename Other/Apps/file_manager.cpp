#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

void displayMenu() {
    std::cout << "Menu:\n";
    std::cout << "1. Read a file\n";
    std::cout << "2. Write to a file\n";
    std::cout << "3. Append to a file\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter your choice: ";
}

void readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    file << content;
    file.close();
}

void appendToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }
    file << content;
    file.close();
}

int main() {
    int choice;
    std::string filename, content;

    while (true) {
        displayMenu();
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Enter filename to read: ";
                std::cin >> filename;
                readFile(filename);
                break;
            case 2:
                std::cout << "Enter filename to write: ";
                std::cin >> filename;
                std::cout << "Enter content to write: ";
                std::cin.ignore();
                std::getline(std::cin, content);
                writeFile(filename, content);
                break;
            case 3:
                std::cout << "Enter filename to append: ";
                std::cin >> filename;
                std::cout << "Enter content to append: ";
                std::cin.ignore();
                std::getline(std::cin, content);
                appendToFile(filename, content);
                break;
            case 4:
                std::cout << "Exiting program." << std::endl;
                return 0;
            default:
                std::cerr << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}