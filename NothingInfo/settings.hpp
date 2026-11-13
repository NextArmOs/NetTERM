#pragma once
#include <string>

enum class SizeUnit {
    AUTO,
    TB,   
    GB,   
    MB,   
    KB   
};

struct AppSettings {
    SizeUnit memoryUnit = SizeUnit::AUTO; 
    SizeUnit netUnit = SizeUnit::AUTO;    
    int logIntervalSec = 5;               
};

extern AppSettings g_settings;

std::string formatSizeWithSettings(unsigned long long bytes, SizeUnit unit);
void toggleSettingsMenu();