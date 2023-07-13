#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds){
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":";
    oss << std::setw(2) << std::setfill('0') << minutes << ":";
    oss << std::setw(2) << std::setfill('0') << secs;

    return oss.str();
}
