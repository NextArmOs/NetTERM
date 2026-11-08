#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include <string>
#include <vector>

class TaskManager {
public:
    TaskManager();
    std::vector<std::string> get_display_content(size_t active_windows_count) const;
};

#endif
