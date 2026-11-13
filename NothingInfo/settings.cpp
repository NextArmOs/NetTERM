#include "settings.hpp"
#include "ui.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>

AppSettings g_settings;

std::string formatSizeWithSettings(unsigned long long bytes, SizeUnit unit) {
    std::ostringstream ss;
    
    if (unit == SizeUnit::TB) {
        ss << std::fixed << std::setprecision(3) << (bytes / (1024.0 * 1024.0 * 1024.0 * 1024.0)) << " TB";
        return formatNumberStr(ss.str());
    }

    ss << std::fixed << std::setprecision(2);
    if (unit == SizeUnit::KB) {
        ss << (bytes / 1024.0) << " KB";
        return formatNumberStr(ss.str());
    }
    if (unit == SizeUnit::MB) {
        ss << (bytes / (1024.0 * 1024.0)) << " MB";
        return formatNumberStr(ss.str());
    }
    if (unit == SizeUnit::GB) {
        ss << (bytes / (1024.0 * 1024.0 * 1024.0)) << " GB";
        return formatNumberStr(ss.str());
    }

    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int idx = 0;
    double val = static_cast<double>(bytes);
    while (val >= 1024.0 && idx < 4) {
        val /= 1024.0;
        idx++;
    }
    ss << val << " " << units[idx];
    return formatNumberStr(ss.str());
}

void toggleSettingsMenu() {
    if (g_settings.memoryUnit == SizeUnit::AUTO) {
        g_settings.memoryUnit = SizeUnit::TB;
        g_settings.netUnit = SizeUnit::TB;
    } else if (g_settings.memoryUnit == SizeUnit::TB) {
        g_settings.memoryUnit = SizeUnit::GB;
        g_settings.netUnit = SizeUnit::GB;
    } else if (g_settings.memoryUnit == SizeUnit::GB) {
        g_settings.memoryUnit = SizeUnit::MB;
        g_settings.netUnit = SizeUnit::MB;
    } else if (g_settings.memoryUnit == SizeUnit::MB) {
        g_settings.memoryUnit = SizeUnit::KB;
        g_settings.netUnit = SizeUnit::KB;
    } else {
        g_settings.memoryUnit = SizeUnit::AUTO;
        g_settings.netUnit = SizeUnit::AUTO;
    }
}