#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <thread>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> char_dist(33, 126);
    std::uniform_int_distribution<int> drop_dist(0, 1);

    const int width = 80;
    std::vector<int> library(width, 0);

    std::cout << "\033[2J\033[H";

    while (true) {
        std::string line = "";
        for (int i = 0; i < width; i++) {
            if (library[i] > 0) {
                line += "\033[1;32m";
                line += static_cast<char>(char_dist(generator));
                line += " ";
                library[i]--;
            } else {
                line += "  ";
                if (drop_dist(generator) == 0 && std::uniform_int_distribution<int>(0, 20)(generator) == 0) {
                    library[i] = std::uniform_int_distribution<int>(5, 25)(generator);
                }
            }
        }
        std::cout << line << "\n" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }

    return 0;
}
