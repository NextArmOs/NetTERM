#include <iostream>
#include <random>
#include <chrono>

int main() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(1, 100);
    int secret_number = distribution(generator);

    int user_guess = 0;
    int attempts = 0;

    std::cout << "=========================================\n";
    std::cout << "    Welcome to the C++ Guess Game! 🎉   \n";
    std::cout << "=========================================\n";
    std::cout << "I have generated a number between 1 and 100. Try to guess it!\n\n";

    while (user_guess != secret_number) {
        std::cout << "Enter your guess: ";
        std::cin >> user_guess;
        attempts++;

        if (user_guess > secret_number) {
            std::cout << "❌ Too high! Try a lower number.\n\n";
        } 
        else if (user_guess < secret_number) {
            std::cout << "❌ Too low! Try a higher number.\n\n";
        } 
        else {
            std::cout << "🎉 CONGRATULATIONS! You guessed it! 🎉\n";
            std::cout << "The secret number was: " << secret_number << "\n";
            std::cout << "It took you " << attempts << " attempts.\n";
        }
    }

    std::cout << "=========================================\n";
    return 0;
}
