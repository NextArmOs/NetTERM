#pragma once
#include <string>
#include <chrono>

class UptimeLogger {
private:
    std::string filename = "uptime.txt";
    unsigned long long lineCount = 0;
    std::chrono::steady_clock::time_point lastWriteTime;

    void countExistingLines();

public:
    UptimeLogger();
    void update(double cpuLoad, double cpuTemp, unsigned long long ramUsedBytes, unsigned long long rxBytes, unsigned long long txBytes);
    void render();
};