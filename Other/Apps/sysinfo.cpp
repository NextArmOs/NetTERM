#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>
#include <csignal>
#include <sys/statvfs.h>
#include <sys/utsname.h>

const std::string BG = "\033[48;2;20;24;36m";
const std::string FG = "\033[38;2;205;214;244m";
const std::string BORDER = "\033[38;2;137;180;250m";
const std::string TITLE = "\033[1;38;2;245;194;231m";
const std::string ACCENT = "\033[38;2;148;226;213m";
const std::string GREEN = "\033[38;2;166;227;161m";
const std::string YELLOW = "\033[38;2;249;226;175m";
const std::string RED = "\033[38;2;243;139;168m";
const std::string MUTED = "\033[38;2;108;112;134m";
const std::string RESET = "\033[0m";

const int BOX_WIDTH = 62;

struct CPUStats {
    unsigned long long user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0;
};

void cleanup(int) {
    std::cout << RESET << "\033[?25h\033[?1049l" << std::flush;
    std::exit(0);
}

CPUStats readCPUStats() {
    std::ifstream file("/proc/stat");
    std::string line;
    CPUStats stats;
    if (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string cpu;
        ss >> cpu >> stats.user >> stats.nice >> stats.system >> stats.idle >> stats.iowait >> stats.irq >> stats.softirq >> stats.steal;
    }
    return stats;
}

double calculateCPUUsage(const CPUStats& prev, const CPUStats& curr) {
    unsigned long long prevIdle = prev.idle + prev.iowait;
    unsigned long long currIdle = curr.idle + curr.iowait;
    unsigned long long prevNonIdle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    unsigned long long currNonIdle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    unsigned long long prevTotal = prevIdle + prevNonIdle;
    unsigned long long currTotal = currIdle + currNonIdle;

    double totald = static_cast<double>(currTotal - prevTotal);
    double idled = static_cast<double>(currIdle - prevIdle);
    if (totald == 0) return 0.0;
    return (totald - idled) / totald * 100.0;
}

std::string getCPUModel() {
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("model name", 0) == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string model = line.substr(pos + 2);
                if (model.length() > 42) model = model.substr(0, 39) + "...";
                return model;
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

void drawBoxTop(const std::string& title) {
    std::cout << BG << BORDER << "╭─ " << TITLE << title << BORDER << " ";
    int dashCount = BOX_WIDTH - 5 - static_cast<int>(title.length());
    for (int i = 0; i < dashCount; ++i) std::cout << "─";
    std::cout << "╮\033[K\n";
}

void drawBoxBottom() {
    std::cout << BG << BORDER << "╰";
    for (int i = 0; i < BOX_WIDTH - 2; ++i) std::cout << "─";
    std::cout << "╯\033[K\n";
}

void drawRow(const std::string& label, const std::string& value, const std::string& valColor = FG) {
    int innerWidth = BOX_WIDTH - 4;
    int labelLen = static_cast<int>(label.length());
    int valLen = static_cast<int>(value.length());
    int spaces = innerWidth - labelLen - valLen;
    if (spaces < 1) spaces = 1;

    std::cout << BG << BORDER << "│ " << ACCENT << label << std::string(spaces, ' ') << valColor << value << BORDER << " │\033[K\n";
}

void drawBarRow(const std::string& label, double percentage, const std::string& barColor) {
    int barWidth = 34;
    int filled = static_cast<int>(percentage / 100.0 * barWidth);
    if (filled > barWidth) filled = barWidth;
    if (filled < 0) filled = 0;

    std::ostringstream pctStream;
    pctStream << std::fixed << std::setprecision(1) << std::setw(5) << percentage << "%";
    std::string pctStr = pctStream.str();

    int innerWidth = BOX_WIDTH - 4;
    int usedLen = static_cast<int>(label.length()) + barWidth + static_cast<int>(pctStr.length()) + 2;
    int spaces = innerWidth - usedLen;
    if (spaces < 1) spaces = 1;

    std::cout << BG << BORDER << "│ " << ACCENT << label << std::string(spaces, ' ') << barColor;
    for (int i = 0; i < filled; ++i) std::cout << "█";
    std::cout << MUTED;
    for (int i = filled; i < barWidth; ++i) std::cout << "░";
    std::cout << " " << FG << pctStr << BORDER << " │\033[K\n";
}

void renderSystem() {
    struct utsname buffer;
    std::ifstream uptimeFile("/proc/uptime");
    double uptimeSeconds = 0;
    std::string uptimeStr = "0d 0h 0m 0s";

    if (uptimeFile >> uptimeSeconds) {
        int days = static_cast<int>(uptimeSeconds) / 86400;
        int hours = (static_cast<int>(uptimeSeconds) % 86400) / 3600;
        int minutes = (static_cast<int>(uptimeSeconds) % 3600) / 60;
        int seconds = static_cast<int>(uptimeSeconds) % 60;
        uptimeStr = std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    }

    drawBoxTop("SYSTEM");
    if (uname(&buffer) == 0) {
        drawRow("Kernel", buffer.release);
        drawRow("Architecture", buffer.machine);
    }
    drawRow("Uptime", uptimeStr, GREEN);
    drawBoxBottom();
}

void renderCPU(const std::string& model, double usage) {
    double temp = getCPUTemperature();
    std::ostringstream tempStream;
    tempStream << std::fixed << std::setprecision(1) << temp << " C";

    std::string tempColor = GREEN;
    if (temp > 75.0) tempColor = RED;
    else if (temp > 55.0) tempColor = YELLOW;

    std::string loadColor = GREEN;
    if (usage > 80.0) loadColor = RED;
    else if (usage > 50.0) loadColor = YELLOW;

    drawBoxTop("CPU");
    drawRow("Model", model);
    drawRow("Temp", tempStream.str(), tempColor);
    drawBarRow("Load", usage, loadColor);
    drawBoxBottom();
}

void renderMemory() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) return;

    std::string key;
    unsigned long value, totalMem = 0, freeMem = 0, buffers = 0, cached = 0;

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

    std::ostringstream ramStr, cacheStr;
    ramStr << std::fixed << std::setprecision(2) << usedGB << " / " << totalGB << " GB";
    cacheStr << std::fixed << std::setprecision(2) << cachedGB << " GB";

    drawBoxTop("MEMORY");
    drawRow("Used / Total", ramStr.str());
    drawRow("Cached", cacheStr.str(), YELLOW);
    drawBarRow("Usage", percentage, ACCENT);
    drawBoxBottom();
}

void renderDisk(const std::string& path, const std::string& title) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) return;

    unsigned long long totalSpace = stat.f_blocks * stat.f_frsize;
    unsigned long long freeSpace = stat.f_bfree * stat.f_frsize;
    unsigned long long usedSpace = totalSpace - freeSpace;

    double totalGB = totalSpace / 1024.0 / 1024.0 / 1024.0;
    double usedGB = usedSpace / 1024.0 / 1024.0 / 1024.0;
    double percentage = (static_cast<double>(usedSpace) / totalSpace) * 100.0;

    std::ostringstream diskStr;
    diskStr << std::fixed << std::setprecision(2) << usedGB << " / " << totalGB << " GB";

    drawBoxTop("DISK (" + title + ")");
    drawRow("Mount", path);
    drawRow("Used / Total", diskStr.str());
    drawBarRow("Usage", percentage, TITLE);
    drawBoxBottom();
}

int main() {
    std::signal(SIGINT, cleanup);
    std::cout << "\033[?1049h\033[?25l" << BG << "\033[2J";

    std::string cpuModel = getCPUModel();
    CPUStats prevStats = readCPUStats();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    while (true) {
        CPUStats currStats = readCPUStats();
        double cpuUsage = calculateCPUUsage(prevStats, currStats);
        prevStats = currStats;

        std::cout << "\033[H";
        renderSystem();
        renderCPU(cpuModel, cpuUsage);
        renderMemory();
        renderDisk("/", "Root");
        std::cout << BG << MUTED << " Press Ctrl+C to exit\033[K\033[J" << std::flush;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
