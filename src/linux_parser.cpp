#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <iterator>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
    std::ifstream file(kProcDirectory + kMeminfoFilename);
    std::string line;
    float totalMemory = 0.0;
    float freeMemory = 0.0;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            float value;
            if (iss >> key >> value) {
                if (key == "MemTotal:") {
                    totalMemory = value;
                } else if (key == "MemFree:") {
                    freeMemory = value;
                }
            }
        }
        file.close();
    }
    
    // Calculate memory utilization
    if (totalMemory > 0.0) {
        float utilization = (totalMemory - freeMemory) / totalMemory;
        return utilization;
    }

    return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
    std::ifstream file(kProcDirectory + kUptimeFilename);
    std::string line;
    long uptime = 0;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> uptime;
    }
    file.close();

    return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    std::ifstream file(kProcDirectory + kStatFilename);
    std::string line;
    long jiffies = 0;
    if (file.is_open()) {
        std::getline(file, line);
        std::istringstream iss(line);
        std::string cpuLabel;
        iss >> cpuLabel;
        if (cpuLabel == "cpu") {
            long jiffy;
            while (iss >> jiffy) {
                jiffies += jiffy;
            }
        }
    }
    file.close();

    return jiffies;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long totaltime;
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }

  // make sure parsing was correct and values was read
  long utime = 0, stime = 0 , cutime = 0, cstime = 0;
  if (std::all_of(values[13].begin(), values[13].end(), isdigit))
    utime = stol(values[13]);
  if (std::all_of(values[14].begin(), values[14].end(), isdigit))
    stime = stol(values[14]);
  if (std::all_of(values[15].begin(), values[15].end(), isdigit))
    cutime = stol(values[15]);
  if (std::all_of(values[16].begin(), values[16].end(), isdigit))
    cstime = stol(values[16]);

  totaltime = utime + stime + cutime + cstime;
  return totaltime / sysconf(_SC_CLK_TCK);
}


// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    std::ifstream file(kProcDirectory + kStatFilename);
    std::string line;
    long userJiffies = 0;
    long niceJiffies = 0;
    long systemJiffies = 0;
    long irqJiffies = 0;
    long softirqJiffies = 0;
    long stealJiffies = 0;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string cpuLabel;
            iss >> cpuLabel;
            if (cpuLabel == "cpu") {
                iss >> userJiffies >> niceJiffies >> systemJiffies >> irqJiffies >> softirqJiffies >> stealJiffies;
                break;
            }
        }
    }
    file.close();

    return userJiffies + niceJiffies + systemJiffies + irqJiffies + softirqJiffies + stealJiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    std::ifstream file(kProcDirectory + kStatFilename);
    std::string line;
    long idleJiffies = 0;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string cpuLabel;
            iss >> cpuLabel;
            if (cpuLabel == "cpu") {
                for (int i = 0; i < 4; ++i) {
                    iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
                }
                iss >> idleJiffies;
                break;
            }
        }
    }
    file.close();

    return idleJiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
    std::ifstream file(kProcDirectory + kStatFilename);
    std::string line;
    vector<string> cpuUtilization;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string cpuLabel;
            iss >> cpuLabel;
            if (cpuLabel.substr(0, 3) == "cpu") {
                cpuUtilization.push_back(line);
            }
        }
    }
    file.close();

    return cpuUtilization;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
    std::ifstream file(kProcDirectory + kStatFilename);
    std::string line;
    int totalProcesses = 0;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.substr(0, 10) == "processes ") {
                std::istringstream iss(line);
                std::string key;
                int value;
                iss >> key >> value;
                totalProcesses = value;
                break;
            }
        }
    }
    file.close();

    return totalProcesses;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    std::ifstream file(kProcDirectory + kStatFilename);
    std::string line;
    int runningProcesses = 0;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.substr(0, 12) == "procs_running") {
                std::istringstream iss(line);
                std::string key;
                int value;
                iss >> key >> value;
                runningProcesses = value;
                break;
            }
        }
    }
    file.close();

    return runningProcesses;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
    std::ifstream file(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
    std::string command;
    if (file.is_open()) {
        std::getline(file, command);
    }
    file.close();

    return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
    std::ifstream file(kProcDirectory + std::to_string(pid) + kStatusFilename);
    std::string line;
    std::string ram;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.substr(0, 6) == "VmSize") {
                std::istringstream iss(line);
                std::string key;
                iss >> key >> ram;
                break;
            }
        }
    }
    file.close();

    // Convert kilobytes to megabytes
    if (!ram.empty()) {
        long ram_kb = std::stol(ram);
        long ram_mb = ram_kb / 1024;
        ram = std::to_string(ram_mb);
    }

    return ram;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
    std::ifstream file(kProcDirectory + std::to_string(pid) + kStatusFilename);
    std::string line;
    std::string uid;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.substr(0, 4) == "Uid:") {
                std::istringstream iss(line);
                std::string key;
                iss >> key >> uid;
                break;
            }
        }
    }
    file.close();

    return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
    std::string uid = Uid(pid);
    std::ifstream file(kPasswordPath);
    std::string line;
    std::string user;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream iss(line);
            std::string username, x, uidFromFile;
            iss >> username >> x >> uidFromFile;
            if (uidFromFile == uid) {
                user = username;
                break;
            }
        }
    }
    file.close();

    return user;
}


// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> values;
  long starttime = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  try {
    starttime = stol(values[21]) / sysconf(_SC_CLK_TCK);
  } catch (...) {
    starttime = 0;
  }
  return starttime;
}
