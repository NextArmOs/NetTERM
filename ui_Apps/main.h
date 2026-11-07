#ifndef MAIN_H
#define MAIN_H

#include <vector>
#include <string>

void drawMenu(const std::vector<std::string>& options, int selected);
int getUserChoice(const std::vector<std::string>& options);

#endif // MAIN_H
