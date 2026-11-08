#ifndef UPTIME_HPP
#define UPTIME_HPP

#include <chrono>
#include <string>
#include <vector>

class Uptime {
private:
    std::chrono::steady_clock::time_point start_time;

public:
    Uptime();
    long long get_elapsed_seconds() const;
    std::string get_formatted_time() const;
    std::vector<std::string> get_display_content() const;
};

#endif
