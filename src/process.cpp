#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h" 

using std::string;
using std::to_string;
using std::vector;


Process::Process(int pid) {
  pid_ = pid;
}

// Return this process's ID
int Process::Pid() { 
    return pid_; 
}


// Return this process's CPU utilization
float Process::CpuUtilization() const {
    long int processStartTime = LinuxParser::UpTime(pid_);
  
    

    // Calculate the process's total time
    long totalTime = LinuxParser::UpTime() - (processStartTime / sysconf(_SC_CLK_TCK));

   // Calculate the CPU utilization as the ratio of active jiffies to total time
    long int activeJiffies = LinuxParser::ActiveJiffies(pid_);
    float utilization = static_cast<float>(activeJiffies) / totalTime;
    return utilization;
}

// Return the command that generated this process
string Process::Command() {
    return LinuxParser::Command(pid_);
}

string Process::Ram() {
    return LinuxParser::Ram(pid_);
}

// Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(pid_);
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
    long int processStartTime = LinuxParser::UpTime(pid_);
    long int systemUptime = LinuxParser::UpTime();
    long int processUptime = systemUptime - processStartTime;
    return processUptime;
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& other) const {
  return CpuUtilization() < other.CpuUtilization();
}