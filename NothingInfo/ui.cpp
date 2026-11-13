#include "ui.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

const std::string BG = "\033[48;2;20;24;36m";
const std::string FG = "\033[38;2;205;214;244m";
const std::string BORDER = "\033[38;2;137;180;250m";
const std::string TITLE = "\033[1;38;2;245;194;231m";
const std::string ACCENT = "\033[38;2;148;226;213m";
const std::string GREEN = "\033[38;2;166;227;161m";
const std::string YELLOW = "\033[38;2;249;226;175m";
const std::string RED = "\033[38;2;243;139;168m";
const std::string MUTED = "\033[38;2;108;112;134m";
const std::string RESET = "\033[0m";

const int BOX_WIDTH = 66;

int visualLength(const std::string& str) {
    int len = 0;
    for (unsigned char c : str) {
        if ((c & 0xC0) != 0x80) {
            len++;
        }
    }
    return len;
}

void drawBoxTop(const std::string& title) {
    std::cout << BG << BORDER << "╭─ " << TITLE << title << BORDER << " ";
    int dashCount = BOX_WIDTH - 5 - visualLength(title);
    for (int i = 0; i < dashCount; ++i) std::cout << "─";
    std::cout << "╮\033[K\n";
}

void drawBoxBottom() {
    std::cout << BG << BORDER << "╰";
    for (int i = 0; i < BOX_WIDTH - 2; ++i) std::cout << "─";
    std::cout << "╯\033[K\n";
}

void drawRow(const std::string& label, const std::string& value, const std::string& valColor) {
    int innerWidth = BOX_WIDTH - 4;
    int spaces = innerWidth - visualLength(label) - visualLength(value);
    if (spaces < 1) spaces = 1;

    std::cout << BG << BORDER << "│ " << ACCENT << label 
              << std::string(spaces, ' ') << valColor << value << BORDER << " │\033[K\n";
}

void drawBarRow(const std::string& label, double percentage, const std::string& barColor) {
    int barWidth = 34;
    int filled = static_cast<int>(percentage / 100.0 * barWidth);
    if (filled > barWidth) filled = barWidth;
    if (filled < 0) filled = 0;

    std::ostringstream pctStream;
    pctStream << std::fixed << std::setprecision(1) << std::setw(5) << percentage << "%";
    std::string pctStr = pctStream.str();

    int innerWidth = BOX_WIDTH - 4;
    int spaces = innerWidth - visualLength(label) - barWidth - 1 - visualLength(pctStr);
    if (spaces < 1) spaces = 1;

    std::cout << BG << BORDER << "│ " << ACCENT << label << std::string(spaces, ' ') << barColor;
    for (int i = 0; i < filled; ++i) std::cout << "█";
    std::cout << MUTED;
    for (int i = filled; i < barWidth; ++i) std::cout << "░";
    std::cout << " " << FG << pctStr << BORDER << " │\033[K\n";
}

std::string formatNumber(unsigned long long num) {
    std::string s = std::to_string(num);
    int insertPos = static_cast<int>(s.length()) - 3;
    while (insertPos > 0) {
        s.insert(insertPos, " ");
        insertPos -= 3;
    }
    return s;
}

std::string formatNumberStr(const std::string& str) {
    size_t dotOrSpace = str.find_first_of(". ");
    if (dotOrSpace == std::string::npos) dotOrSpace = str.length();

    std::string intPart = str.substr(0, dotOrSpace);
    std::string rest = str.substr(dotOrSpace);

    int insertPos = static_cast<int>(intPart.length()) - 3;
    while (insertPos > 0) {
        intPart.insert(insertPos, " ");
        insertPos -= 3;
    }
    return intPart + rest;
}