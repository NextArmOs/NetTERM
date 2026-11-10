#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>
#include <sys/statvfs.h>
#include <sys/utsname.h>

void printProgressBar(double percentage, const std::string& color) {
    int width = 20;
    int progress = static_cast<int>(percentage / 100.0 * width);
    
    std::cout << color << "[";
    for (int i = 0; i < width; ++i) {
        if (i < progress) std::cout << "#";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << std::setw(5) << percentage << "%\033[0m\n";
}

std::string getCPUModel() {
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("model name", 0) == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                return line.substr(pos + 2);
            }
        }
    }
    return "Unknown CPU";
}

double getCPUTemperature() {
    for (const auto& entry : std::filesystem::directory_iterator("/sys/class/hwmon")) {
        std::ifstream nameFile(entry.path() / "name");
        std::string name;
        if (nameFile >> name) {
            if (name == "k10temp" || name == "coretemp" || name == "cpu_thermal") {
                std::ifstream tempFile(entry.path() / "temp1_input");
                double tempRaw;
                if (tempFile >> tempRaw) {
                    return tempRaw / 1000.0;
                }
            }
        }
    }
    return 0.0;
}

void showSystemInfo() {
    struct utsname buffer;
    std::ifstream uptimeFile("/proc/uptime");
    double uptimeSeconds = 0;

    if (uptimeFile >> uptimeSeconds) {
        int days = static_cast<int>(uptimeSeconds) / 86400;
        int hours = (static_cast<int>(uptimeSeconds) % 86400) / 3600;
        int minutes = (static_cast<int>(uptimeSeconds) % 3600) / 60;
        int seconds = static_cast<int>(uptimeSeconds) % 60;

        if (uname(&buffer) == 0) {
            std::cout << "\033[1;34m=== SYSTEM INFO ===\033[0m\n";
            std::cout << std::left << std::setw(13) << "OS Kernel:" << buffer.release << "\033[K\n";
            std::cout << std::left << std::setw(13) << "Architecture:" << buffer.machine << "\033[K\n";
            std::cout << std::left << std::setw(13) << "Uptime:" << days << "d " << hours << "h " << minutes << "m " << seconds << "s\033[K\n\n";
        }
    }
}

void showCPU() {
    double temp = getCPUTemperature();
    std::cout << "\033[1;33m=== HARDWARE ===\033[0m\n";
    std::cout << std::left << std::setw(13) << "CPU Model:" << getCPUModel() << "\033[K\n";
    std::cout << std::left << std::setw(13) << "CPU Temp:" << std::fixed << std::setprecision(1) << temp << " °C\033[K\n\n";
}

void showMemory() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) return;

    std::string key;
    unsigned long value;
    unsigned long totalMem = 0;
    unsigned long freeMem = 0;
    unsigned long buffers = 0;
    unsigned long cached = 0;

    while (file >> key >> value) {
        if (key == "MemTotal:") totalMem = value;
        else if (key == "MemFree:") freeMem = value;
        else if (key == "Buffers:") buffers = value;
        else if (key == "Cached:") cached = value;
        std::string dummy;
        file >> dummy; 
    }

    unsigned long usedMem = totalMem - (freeMem + buffers + cached);
    double totalGB = totalMem / 1024.0 / 1024.0;
    double usedGB = usedMem / 1024.0 / 1024.0;
    double cachedGB = cached / 1024.0 / 1024.0;
    double percentage = (static_cast<double>(usedMem) / totalMem) * 100.0;

    std::cout << "\033[1;36m=== MEMORY USAGE ===\033[0m\n";
    std::cout << std::left << std::setw(13) << "Total RAM:" << std::fixed << std::setprecision(2) << totalGB << " GB\033[K\n";
    std::cout << std::left << std::setw(13) << "Used RAM:" << usedGB << " GB\033[K\n";
    std::cout << std::left << std::setw(13) << "Cached RAM:" << cachedGB << " GB\033[K\n";
    printProgressBar(percentage, "\033[32m");
    std::cout << "\n";
}

void showDisk(const std::string& path, const std::string& name) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) return;

    unsigned long long totalSpace = stat.f_blocks * stat.f_frsize;
    unsigned long long freeSpace = stat.f_bfree * stat.f_frsize;
    unsigned long long usedSpace = totalSpace - freeSpace;

    double totalGB = totalSpace / 1024.0 / 1024.0 / 1024.0;
    double usedGB = usedSpace / 1024.0 / 1024.0 / 1024.0;
    double percentage = (static_cast<double>(usedSpace) / totalSpace) * 100.0;

    std::cout << "\033[1;35m=== DISK USAGE (" << name << ") ===\033[0m\n";
    std::cout << std::left << std::setw(13) << "Mount Point:" << path << "\033[K\n";
    std::cout << std::left << std::setw(13) << "Total Space:" << std::fixed << std::setprecision(2) << totalGB << " GB\033[K\n";
    std::cout << std::left << std::setw(13) << "Used Space:" << usedGB << " GB\033[K\n";
    printProgressBar(percentage, "\033[33m");
    std::cout << "\n";
}

int main() {
    std::cout << "\033[2J"; 
    
    while (true) {
        std::cout << "\033[H"; 
        showSystemInfo();
        showCPU();
        showMemory();
        showDisk("/", "Root");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
