#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>
#include <csignal>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "ui.cpp"
#include "settings.cpp"
#include "logger.cpp"
#include "cpu.cpp"
#include "network.cpp"
#include "gpu.cpp"

struct termios origTermios;

void restoreTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &origTermios);
    std::cout << RESET << "\033[?25h\033[?1049l" << std::flush;
}

void cleanup(int) {
    restoreTerminal();
    std::exit(0);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &origTermios);
    struct termios raw = origTermios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

std::string unitToString(SizeUnit u) {
    switch (u) {
        case SizeUnit::TB: return "TB";
        case SizeUnit::GB: return "GB";
        case SizeUnit::MB: return "MB";
        case SizeUnit::KB: return "KB";
        default: return "AUTO";
    }
}

unsigned long long getRootFilesCount() {
    static bool calculated = false;
    static unsigned long long cachedCount = 0;
    if (calculated) return cachedCount;

    struct statvfs stat;
    if (statvfs("/", &stat) == 0 && stat.f_files > 0 && stat.f_files >= stat.f_ffree) {
        cachedCount = stat.f_files - stat.f_ffree;
        calculated = true;
        return cachedCount;
    }

    namespace fs = std::filesystem;
    std::error_code ec;
    auto it = fs::recursive_directory_iterator("/", fs::directory_options::skip_permission_denied, ec);
    
    for (auto end = fs::recursive_directory_iterator(); it != end; it.increment(ec)) {
        if (ec) {
            ec.clear();
            continue;
        }
        std::string p = it->path().string();
        if (p == "/proc" || p == "/sys" || p == "/dev" || p == "/run" || p == "/tmp") {
            it.disable_recursion_pending();
            continue;
        }
        cachedCount++;
    }

    calculated = true;
    return cachedCount;
}

void renderSystem() {
    struct utsname buffer;
    std::ifstream uptimeFile("/proc/uptime");
    double uptimeSeconds = 0;
    std::string uptimeStr = "0d 0h 0m 0s";

    if (uptimeFile >> uptimeSeconds) {
        unsigned long long days = static_cast<unsigned long long>(uptimeSeconds) / 86400;
        int hours = (static_cast<int>(uptimeSeconds) % 86400) / 3600;
        int minutes = (static_cast<int>(uptimeSeconds) % 3600) / 60;
        int seconds = static_cast<int>(uptimeSeconds) % 60;
        uptimeStr = formatNumber(days) + "d " + std::to_string(hours) + "h " 
                  + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    }

    drawBoxTop("SYSTEM");
    if (uname(&buffer) == 0) {
        drawRow("Kernel", buffer.release);
        drawRow("Arch", buffer.machine);
    }
    drawRow("Uptime", uptimeStr, GREEN);
    drawRow("Units Mode [u]", unitToString(g_settings.memoryUnit), YELLOW);
    drawBoxBottom();
}

unsigned long long renderMemory() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) return 0;

    std::string key;
    unsigned long long value, totalMem = 0, memAvailable = 0, cached = 0;

    while (file >> key >> value) {
        if (key == "MemTotal:") totalMem = value * 1024ULL;
        else if (key == "MemAvailable:") memAvailable = value * 1024ULL;
        else if (key == "Cached:") cached = value * 1024ULL;
        std::string dummy;
        file >> dummy;
    }

    unsigned long long usedMem = (totalMem > memAvailable) ? (totalMem - memAvailable) : 0;
    double percentage = (totalMem > 0) ? (static_cast<double>(usedMem) / totalMem) * 100.0 : 0.0;

    std::string ramStr = formatSizeWithSettings(usedMem, g_settings.memoryUnit) + " / " 
                       + formatSizeWithSettings(totalMem, g_settings.memoryUnit);

    drawBoxTop("MEMORY");
    drawRow("Used / Total", ramStr);
    drawRow("Cached", formatSizeWithSettings(cached, g_settings.memoryUnit), YELLOW);
    drawBarRow("Usage", percentage, ACCENT);
    drawBoxBottom();

    return usedMem;
}

void renderSingleDisk(const std::string& path, const std::string& device, bool isRoot) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) return;

    unsigned long long totalSpace = static_cast<unsigned long long>(stat.f_blocks) * stat.f_frsize;
    if (totalSpace == 0) return;

    unsigned long long freeSpace = static_cast<unsigned long long>(stat.f_bfree) * stat.f_frsize;
    unsigned long long usedSpace = totalSpace - freeSpace;
    double percentage = (static_cast<double>(usedSpace) / totalSpace) * 100.0;

    std::string title = isRoot ? "DISK (Root)" : ("DISK (" + path + ")");
    if (title.length() > 40) title = title.substr(0, 37) + "...)";

    std::string diskStr = formatSizeWithSettings(usedSpace, g_settings.memoryUnit) + " / " 
                        + formatSizeWithSettings(totalSpace, g_settings.memoryUnit);

    drawBoxTop(title);
    drawRow("Device / Mount", device + " -> " + path);
    if (isRoot) {
        drawRow("Total Files (/)", formatNumber(getRootFilesCount()), GREEN);
    }
    drawRow("Used / Total", diskStr);
    drawBarRow("Usage", percentage, TITLE);
    drawBoxBottom();
}

void renderAllDisks() {
    std::ifstream mounts("/proc/mounts");
    std::string device, mountPoint, fsType, options;
    int dump, pass;

    std::set<std::string> seenDevices;
    const std::set<std::string> validFs = {
        "ext4", "ext3", "ext2", "btrfs", "xfs", "zfs", "ntfs", "ntfs3", "vfat", "exfat", "f2fs", "nfs", "nfs4", "cifs"
    };

    renderSingleDisk("/", "/dev/root", true);
    seenDevices.insert("/");

    while (mounts >> device >> mountPoint >> fsType >> options >> dump >> pass) {
        if (mountPoint == "/" || mountPoint.rfind("/boot", 0) == 0 || mountPoint.rfind("/snap", 0) == 0) continue;
        if (validFs.find(fsType) == validFs.end()) continue;
        if (seenDevices.count(device)) continue;

        seenDevices.insert(device);
        renderSingleDisk(mountPoint, device, false);
    }
}

int main() {
    std::signal(SIGINT, cleanup);
    enableRawMode();
    std::cout << "\033[?1049h\033[?25l" << BG << "\033[2J";

    CPUMonitor cpuMonitor;
    NetworkMonitor netMonitor;
    GPUMonitor gpuMonitor;
    UptimeLogger logger;

    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    while (true) {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            if (ch == 'q' || ch == 'Q') cleanup(0);
            else if (ch == 'u' || ch == 'U') toggleSettingsMenu();
            else if (ch == '+' || ch == '=') g_settings.logIntervalSec++;
            else if ((ch == '-' || ch == '_') && g_settings.logIntervalSec > 1) g_settings.logIntervalSec--;
        }

        std::cout << "\033[H";
        renderSystem();
        cpuMonitor.render();
        unsigned long long usedRam = renderMemory();
        renderAllDisks();
        netMonitor.render();
        gpuMonitor.render();

        logger.update(cpuMonitor.getLastLoad(), cpuMonitor.getLastTemp(), usedRam, 
                      netMonitor.getTotalRx(), netMonitor.getTotalTx());
        logger.render();

        std::cout << BG << MUTED << " [u] Units (AUTO/TB/GB/MB/KB)  [+/-] Log Interval  [q] Exit\033[K\033[J" << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}