#pragma once
#include <string>

struct GPUInfo {
    bool available = false;
    std::string name = "Integrated / Unknown GPU";
    double loadPct = 0.0;
    double vramUsedGB = 0.0;
    double vramTotalGB = 0.0;
    double tempC = 0.0;
    double powerW = 0.0;
};

class GPUMonitor {
private:
    GPUInfo fetchNvidia();
    GPUInfo fetchAMD();

public:
    void render();
};