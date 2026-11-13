#pragma once
#include <string>
#include <vector>
#include <chrono>

struct CoreTime {
    unsigned long long user = 0, nice = 0, system = 0, idle = 0;
    unsigned long long iowait = 0, irq = 0, softirq = 0, steal = 0;
};

class CPUMonitor {
private:
    std::string modelName;
    CoreTime prevTotal;
    std::vector<CoreTime> prevCores;
    
    unsigned long long prevEnergyUj = 0;
    std::chrono::steady_clock::time_point prevEnergyTime;

    double lastLoad = 0.0;
    double lastTemp = 0.0;

    std::string fetchModelName();
    void readAllStats(CoreTime& total, std::vector<CoreTime>& cores);
    double calcUsage(const CoreTime& prev, const CoreTime& curr);
    std::vector<int> getCoreFrequencies();
    double getTemperature();
    double getPowerWatts();
    unsigned long long readEnergyUj();

public:
    CPUMonitor();
    void render();
    double getLastLoad() const { return lastLoad; }
    double getLastTemp() const { return lastTemp; }
};