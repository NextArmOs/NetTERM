#pragma once
#include <string>

extern const std::string BG;
extern const std::string FG;
extern const std::string BORDER;
extern const std::string TITLE;
extern const std::string ACCENT;
extern const std::string GREEN;
extern const std::string YELLOW;
extern const std::string RED;
extern const std::string MUTED;
extern const std::string RESET;

extern const int BOX_WIDTH;

int visualLength(const std::string& str);
void drawBoxTop(const std::string& title);
void drawBoxBottom();
void drawRow(const std::string& label, const std::string& value, const std::string& valColor = FG);
void drawBarRow(const std::string& label, double percentage, const std::string& barColor);
std::string formatNumber(unsigned long long num);
std::string formatNumberStr(const std::string& str);
