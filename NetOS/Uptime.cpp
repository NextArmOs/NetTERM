#include "Uptime.hpp"
#include <iomanip>
#include <sstream>

Uptime::Uptime() {
    start_time = std::chrono::steady_clock::now();
}

long long Uptime::get_elapsed_seconds() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
}

std::string Uptime::get_formatted_time() const {
    long long total_seconds = get_elapsed_seconds();
    long long hours = total_seconds / 3600;
    long long minutes = (total_seconds % 3600) / 60;
    long long seconds = total_seconds % 60;

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":"
       << std::setfill('0') << std::setw(2) << minutes << ":"
       << std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
}

std::vector<std::string> Uptime::get_display_content() const {
    return {
        " System Performance Status ",
        " ------------------------- ",
        " Live Uptime: " + get_formatted_time(),
        " Status: Running smoothly ",
        " Host: root "
    };
}
