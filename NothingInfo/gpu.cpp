#include "gpu.hpp"
#include "ui.hpp"
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <vector>     
#include <stdexcept> 

namespace fs = std::filesystem;
static double safeStod(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        return 0.0;
    }
}

GPUInfo GPUMonitor::fetchNvidia() {
    GPUInfo info;
    FILE* pipe = popen("nvidia-smi --query-gpu=name,utilization.gpu,memory.used,memory.total,temperature.gpu,power.draw --format=csv,noheader,nounits 2>/dev/null", "r");
    if (!pipe) return info;

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, item, ',')) {
            // Убираем пробелы в начале и конце строки
            size_t first = item.find_first_not_of(" \t\r\n");
            size_t last = item.find_last_not_of(" \t\r\n");
            if (first != std::string::npos && last != std::string::npos) {
                tokens.push_back(item.substr(first, last - first + 1));
            } else {
                tokens.push_back("");
            }
        }

        if (tokens.size() >= 6) {
            info.available = true;
            info.name = tokens[0];
            info.loadPct = safeStod(tokens[1]);
            info.vramUsedGB = safeStod(tokens[2]) / 1024.0;
            info.vramTotalGB = safeStod(tokens[3]) / 1024.0;
            info.tempC = safeStod(tokens[4]);
            info.powerW = safeStod(tokens[5]);
        }
    }
    pclose(pipe);
    return info;
}

GPUInfo GPUMonitor::fetchAMD() {
    GPUInfo info;
    std::string base = "/sys/class/drm/card0/device";
    if (!fs::exists(base + "/gpu_busy_percent")) return info;

    info.available = true;
    info.name = "AMD Radeon GPU";

    std::ifstream busy(base + "/gpu_busy_percent");
    busy >> info.loadPct;

    unsigned long long vramUsed = 0, vramTotal = 0;
    std::ifstream vu(base + "/mem_info_vram_used");
    std::ifstream vt(base + "/mem_info_vram_total");
    if ((vu >> vramUsed) && (vt >> vramTotal)) {
        info.vramUsedGB = vramUsed / (1024.0 * 1024.0 * 1024.0);
        info.vramTotalGB = vramTotal / (1024.0 * 1024.0 * 1024.0);
    }

    if (fs::exists(base + "/hwmon")) {
        for (const auto& entry : fs::directory_iterator(base + "/hwmon")) {
            std::ifstream tf(entry.path() / "temp1_input");
            double t = 0;
            if (tf >> t) info.tempC = t / 1000.0;

            std::ifstream pf(entry.path() / "power1_average");
            double p = 0;
            if (pf >> p) info.powerW = p / 1000000.0;
        }
    }
    return info;
}

void GPUMonitor::render() {
    GPUInfo gpu = fetchNvidia();
    if (!gpu.available) {
        gpu = fetchAMD();
    }

    drawBoxTop("GPU");
    if (!gpu.available) {
        drawRow("Status", "No discrete GPU telemetry found", MUTED);
        drawBoxBottom();
        return;
    }

    if (gpu.name.length() > 44) gpu.name = gpu.name.substr(0, 41) + "...";

    std::ostringstream vramStr, thermStr;
    vramStr << std::fixed << std::setprecision(2) << gpu.vramUsedGB << " / " << gpu.vramTotalGB << " GB";
    thermStr << std::fixed << std::setprecision(1) << gpu.tempC << " °C  |  " << gpu.powerW << " W";

    double vramPct = (gpu.vramTotalGB > 0) ? (gpu.vramUsedGB / gpu.vramTotalGB * 100.0) : 0.0;
    std::string tempColor = (gpu.tempC > 80.0) ? RED : (gpu.tempC > 60.0 ? YELLOW : GREEN);
    std::string loadColor = (gpu.loadPct > 80.0) ? RED : (gpu.loadPct > 50.0 ? YELLOW : GREEN);

    drawRow("Device", gpu.name);
    drawRow("Temp & Power", thermStr.str(), tempColor);
    drawBarRow("Core Load", gpu.loadPct, loadColor);
    drawRow("VRAM Used", vramStr.str(), ACCENT);
    drawBarRow("VRAM Load", vramPct, TITLE);
    drawBoxBottom();
}