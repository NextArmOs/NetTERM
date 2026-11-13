#include "cpu.hpp"
#include "ui.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

CPUMonitor::CPUMonitor() {
    modelName = fetchModelName();
    readAllStats(prevTotal, prevCores);
    prevEnergyUj = readEnergyUj();
    prevEnergyTime = std::chrono::steady_clock::now();
}

std::string CPUMonitor::fetchModelName() {
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("model name", 0) == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string model = line.substr(pos + 2);
                if (model.length() > 44) model = model.substr(0, 41) + "...";
                return model;
            }
        }
    }
    return "Unknown CPU";
}

void CPUMonitor::readAllStats(CoreTime& total, std::vector<CoreTime>& cores) {
    std::ifstream file("/proc/stat");
    std::string line;
    cores.clear();

    while (std::getline(file, line)) {
        if (line.rfind("cpu", 0) != 0) break;
        std::istringstream ss(line);
        std::string label;
        CoreTime ct;
        ss >> label >> ct.user >> ct.nice >> ct.system >> ct.idle 
           >> ct.iowait >> ct.irq >> ct.softirq >> ct.steal;

        if (label == "cpu") {
            total = ct;
        } else {
            cores.push_back(ct);
        }
    }
}

double CPUMonitor::calcUsage(const CoreTime& prev, const CoreTime& curr) {
    unsigned long long prevIdle = prev.idle + prev.iowait;
    unsigned long long currIdle = curr.idle + curr.iowait;
    unsigned long long prevNonIdle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    unsigned long long currNonIdle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    double totald = static_cast<double>((currIdle + currNonIdle) - (prevIdle + prevNonIdle));
    double idled = static_cast<double>(currIdle - prevIdle);
    if (totald <= 0.0) return 0.0;
    return (totald - idled) / totald * 100.0;
}

std::vector<int> CPUMonitor::getCoreFrequencies() {
    std::vector<int> freqs;
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("cpu MHz", 0) == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                freqs.push_back(static_cast<int>(std::stod(line.substr(pos + 2))));
            }
        }
    }
    return freqs;
}

double CPUMonitor::getTemperature() {
    if (!fs::exists("/sys/class/hwmon")) return 0.0;
    
    double fallbackTemp = 0.0;

    for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
        std::ifstream nameFile(entry.path() / "name");
        std::string name;
        if (nameFile >> name) {
            if (name == "k10temp" || name == "coretemp" || name == "zenpower" || name == "cpu_thermal") {
                for (int i = 1; i <= 5; ++i) {
                    fs::path tPath = entry.path() / ("temp" + std::to_string(i) + "_input");
                    if (!fs::exists(tPath)) continue;

                    std::ifstream tempFile(tPath);
                    long long tempRaw = 0;
                    if (tempFile >> tempRaw && tempRaw > 0) {
                        if (fallbackTemp == 0.0) {
                            fallbackTemp = tempRaw / 1000.0;
                        }
                        if (tempRaw % 1000 != 0) {
                            return tempRaw / 1000.0;
                        }
                    }
                }
            }
        }
    }
    return fallbackTemp;
}
unsigned long long CPUMonitor::readEnergyUj() {
    std::ifstream rapl("/sys/class/powercap/intel-rapl:0/energy_uj");
    unsigned long long uj = 0;
    if (rapl >> uj) return uj;
    return 0;
}

double CPUMonitor::getPowerWatts() {
    unsigned long long currUj = readEnergyUj();
    auto now = std::chrono::steady_clock::now();
    
    if (currUj > 0 && prevEnergyUj > 0 && currUj >= prevEnergyUj) {
        double dt = std::chrono::duration<double>(now - prevEnergyTime).count();
        double watts = (currUj - prevEnergyUj) / 1000000.0 / dt;
        prevEnergyUj = currUj;
        prevEnergyTime = now;
        return watts;
    }

    if (fs::exists("/sys/class/hwmon")) {
        for (const auto& entry : fs::directory_iterator("/sys/class/hwmon")) {
            std::ifstream powerFile(entry.path() / "power1_input");
            double microwatts = 0.0;
            if (powerFile >> microwatts) return microwatts / 1000000.0;
        }
    }
    return 0.0;
}

void CPUMonitor::render() {
    CoreTime currTotal;
    std::vector<CoreTime> currCores;
    readAllStats(currTotal, currCores);

    lastLoad = calcUsage(prevTotal, currTotal);
    lastTemp = getTemperature();
    double watts = getPowerWatts();
    std::vector<int> freqs = getCoreFrequencies();

    std::ostringstream statusLine;
    statusLine << std::fixed << std::setprecision(1) << lastTemp << " °C";
    if (watts > 0.0) {
        statusLine << "  |  " << std::fixed << std::setprecision(1) << watts << " W";
    }

    std::string tempColor = (lastTemp > 75.0) ? RED : (lastTemp > 55.0 ? YELLOW : GREEN);
    std::string loadColor = (lastLoad > 80.0) ? RED : (lastLoad > 50.0 ? YELLOW : GREEN);

    drawBoxTop("CPU");
    drawRow("Model", modelName);
    drawRow("Temp & Power", statusLine.str(), tempColor);
    drawBarRow("Total Load", lastLoad, loadColor);

    size_t numCores = currCores.size();
    for (size_t i = 0; i < numCores; i += 2) {
        std::string leftCell, rightCell;
        
        for (size_t col = 0; col < 2; ++col) {
            size_t idx = i + col;
            std::ostringstream cellStream;
            if (idx < numCores) {
                double u = (idx < prevCores.size()) ? calcUsage(prevCores[idx], currCores[idx]) : 0.0;
                int freq = (idx < freqs.size()) ? freqs[idx] : 0;
                
                std::string cColor = (u > 80.0) ? RED : (u > 50.0 ? YELLOW : ACCENT);
                int miniBar = 8;
                int filled = static_cast<int>(u / 100.0 * miniBar);
                if (filled > miniBar) filled = miniBar;

                cellStream << FG << "C" << std::setw(2) << std::left << idx << " " << cColor;
                for (int b = 0; b < filled; ++b) cellStream << "▰";
                cellStream << MUTED;
                for (int b = filled; b < miniBar; ++b) cellStream << "▱";

                cellStream << FG << std::setw(4) << std::right << static_cast<int>(u) << "% " 
                           << std::setw(4) << freq << "MHz";
            } else {
                cellStream << std::string(25, ' ');
            }
            if (col == 0) leftCell = cellStream.str();
            else rightCell = cellStream.str();
        }

        std::cout << BG << BORDER << "│   " << leftCell << BORDER << "    │    " << rightCell << BORDER << "  │\033[K\n";
    }
    drawBoxBottom();
    prevTotal = currTotal;
    prevCores = currCores;
}