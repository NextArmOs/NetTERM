#include "TaskManager.hpp"
#include <string>
#include <vector>

TaskManager::TaskManager() {}

std::vector<std::string> TaskManager::get_display_content(size_t active_windows_count) const {
    return {
        " NetOS Process Monitor ",
        " ----------------------- ",
        " Active Windows: " + std::to_string(active_windows_count),
        " ",
        " CPU Usage: < ~1% (ASCII)",
        " OS Status: Stable ",
        " ----------------------- "
    };
}
