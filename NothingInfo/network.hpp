#pragma once
#include <string>

struct NetStats {
    unsigned long long rxBytes = 0;
    unsigned long long txBytes = 0;
    unsigned long long rxPackets = 0;
    unsigned long long txPackets = 0;
    std::string activeIface = "none";
};

class NetworkMonitor {
private:
    NetStats prevStats;
    NetStats readRawStats();
    std::string formatSpeed(double bytesPerSec);

public:
    NetworkMonitor();
    void render();
    unsigned long long getTotalRx() const { return prevStats.rxBytes; }
    unsigned long long getTotalTx() const { return prevStats.txBytes; }
};