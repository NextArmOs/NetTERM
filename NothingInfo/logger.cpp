#include "logger.hpp"
#include "settings.hpp"
#include "ui.hpp"
#include <fstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

UptimeLogger::UptimeLogger() {
    countExistingLines();
    lastWriteTime = std::chrono::steady_clock::now() - std::chrono::seconds(g_settings.logIntervalSec);
}

void UptimeLogger::countExistingLines() {
    std::ifstream in(filename);
    std::string line;
    lineCount = 0;
    while (std::getline(in, line)) {
        if (!line.empty()) lineCount++;
    }
}

void UptimeLogger::update(double cpuLoad, double cpuTemp, unsigned long long ramUsedBytes, unsigned long long rxBytes, unsigned long long txBytes) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastWriteTime).count();

    if (elapsed >= g_settings.logIntervalSec) {
        std::ofstream out(filename, std::ios::app);
        if (out.is_open()) {
            std::time_t t = std::time(nullptr);
            std::tm* tmPtr = std::localtime(&t);

            out << std::put_time(tmPtr, "%Y-%m-%d %H:%M:%S")
                << " | CPU: " << std::fixed << std::setprecision(1) << cpuLoad << "% (" << cpuTemp << "C)"
                << " | RAM: " << formatSizeWithSettings(ramUsedBytes, g_settings.memoryUnit)
                << " | RX: " << formatSizeWithSettings(rxBytes, g_settings.netUnit)
                << " | TX: " << formatSizeWithSettings(txBytes, g_settings.netUnit)
                << "\n";
            out.flush();
            lineCount++;
        }
        lastWriteTime = now;
    }
}

void UptimeLogger::render() {
    unsigned long long fileSize = 0;
    if (fs::exists(filename)) {
        fileSize = fs::file_size(filename);
    }

    drawBoxTop("LOG (uptime.txt)");
    drawRow("Write Interval", formatNumber(g_settings.logIntervalSec) + " sec", YELLOW);
    drawRow("Lines Recorded", formatNumber(lineCount), GREEN);
    drawRow("File Size", formatSizeWithSettings(fileSize, g_settings.memoryUnit), ACCENT);
    drawBoxBottom();
}