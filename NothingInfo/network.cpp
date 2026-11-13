#include "network.hpp"
#include "settings.hpp"
#include "ui.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

NetworkMonitor::NetworkMonitor() {
    prevStats = readRawStats();
}

NetStats NetworkMonitor::readRawStats() {
    std::ifstream file("/proc/net/dev");
    std::string line;
    NetStats total;
    
    std::getline(file, line);
    std::getline(file, line);

    unsigned long long maxTraffic = 0;

    while (std::getline(file, line)) {
        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;
        
        std::string iface = line.substr(0, colon);
        iface.erase(0, iface.find_first_not_of(" \t"));
        if (iface == "lo") continue;

        std::istringstream ss(line.substr(colon + 1));
        unsigned long long rx, rPackets, rErrs, rDrop, rFifo, rFrame, rComp, rMulti;
        unsigned long long tx, tPackets;
        
        ss >> rx >> rPackets >> rErrs >> rDrop >> rFifo >> rFrame >> rComp >> rMulti >> tx >> tPackets;
        
        total.rxBytes += rx;
        total.txBytes += tx;
        total.rxPackets += rPackets;
        total.txPackets += tPackets;

        if (rx + tx > maxTraffic) {
            maxTraffic = rx + tx;
            total.activeIface = iface;
        }
    }
    return total;
}

std::string NetworkMonitor::formatSpeed(double bytesPerSec) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1);
    if (bytesPerSec >= 1024.0 * 1024.0) {
        ss << (bytesPerSec / (1024.0 * 1024.0)) << " MB/s";
    } else {
        ss << (bytesPerSec / 1024.0) << " KB/s";
    }
    return ss.str();
}

void NetworkMonitor::render() {
    NetStats curr = readRawStats();
    double rxSpeed = static_cast<double>(curr.rxBytes - prevStats.rxBytes);
    double txSpeed = static_cast<double>(curr.txBytes - prevStats.txBytes);
    prevStats = curr;

    drawBoxTop("NETWORK (" + curr.activeIface + ")");
    drawRow("Download Speed (RX)", formatSpeed(rxSpeed), GREEN);
    drawRow("Upload Speed   (TX)", formatSpeed(txSpeed), YELLOW);
    drawRow("Total Downloaded", formatSizeWithSettings(curr.rxBytes, g_settings.netUnit), ACCENT);
    drawRow("Total Uploaded", formatSizeWithSettings(curr.txBytes, g_settings.netUnit), TITLE);
    drawBoxBottom();
}