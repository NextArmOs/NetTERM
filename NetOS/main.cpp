#include "calc.hpp"
#include "Uptime.hpp"
#include "Taskbar.hpp"
#include "os.hpp"

#include "calc.cpp"
#include "Uptime.cpp"
#include "Taskbar.cpp"
#include "os.cpp"

int main() {
    AsciiOS os;
    os.run();
    return 0;
}
