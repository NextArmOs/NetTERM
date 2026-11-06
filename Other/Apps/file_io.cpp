#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string filename = "netterm_test.txt";

    std::cout << "1. Creating file and writing data...\n";
    
    std::ofstream out_file(filename);

    if (out_file.is_open()) {
        out_file << "Hello World!\n";
        out_file << "This file was successfully created by our application.\n";
        out_file << "The number of lines and apps in NetTERM is growing, and that is awesome! 🚀\n";
        
        out_file.close();
        std::cout << "✅ File \"" << filename << "\" successfully written!\n\n";
    } else {
        std::cout << "❌ Error creating file for writing!\n\n";
        return 1;
    }
    std::cout << "2. Opening file and reading data...\n";

    std::ifstream in_file(filename);
    std::string line;

    if (in_file.is_open()) {
        std::cout << "=== FILE CONTENT ===\n";
        
        while (std::getline(in_file, line)) {
            std::cout << line << "\n";
        }
        
        in_file.close();
        std::cout << "====================\n";
        std::cout << "✅ File reading completed successfully!\n";
    } else {
        std::cout << "❌ Error opening file for reading!\n";
        return 1;
    }

    return 0;
}
