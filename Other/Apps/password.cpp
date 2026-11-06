#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <algorithm>

int main() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);

    std::string uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string lowercase = "abcdefghijklmnopqrstuvwxyz";
    std::string digits = "0123456789";
    std::string special = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    
    std::string all_chars = uppercase + lowercase + digits + special;
    std::uniform_int_distribution<int> distribution(0, all_chars.length() - 1);

    int length = 0;
    std::cout << "=========================================\n";
    std::cout << "      C++ Password Generator 🔐          \n";
    std::cout << "=========================================\n";
    std::cout << "Enter desired password length: ";
    std::cin >> length;

    if (length < 4) {
        std::cout << "❌ Error: Length must be at least 4 characters.\n";
        return 1;                                                                                                              
    }

    std::string password = "";
    
    password += uppercase[std::uniform_int_distribution<int>(0, uppercase.length() - 1)(generator)];
    password += lowercase[std::uniform_int_distribution<int>(0, lowercase.length() - 1)(generator)];
    password += digits[std::uniform_int_distribution<int>(0, digits.length() - 1)(generator)];
    password += special[std::uniform_int_distribution<int>(0, special.length() - 1)(generator)];

    for (int i = 4; i < length; i++) {
        password += all_chars[distribution(generator)];
    }

    std::shuffle(password.begin(), password.end(), generator);

    std::cout << "\n─────────────────────────────────────────\n";
    std::cout << " Generated Password : " << password << "\n";
    std::cout << "─────────────────────────────────────────\n";

    return 0;
}
