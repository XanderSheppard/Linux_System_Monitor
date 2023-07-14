#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <sys/utsname.h>
#include <cstring>   
#include <fstream>
#include <sstream>
#include <dirent.h>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { 
    return cpu_; 
}

// Return a container composed of the system's processes
std::vector<Process>& System::Processes() { 
    processes_.clear();
    std::vector<int> pids = LinuxParser::Pids();

    for(int pid : pids){
        Process process(pid);
        processes_.push_back(process);
    } 

    std::sort(processes_.begin(), processes_.end());
    
    return processes_; 
    }


// Return the system's kernel identifier (string)
std::string System::Kernel() {
    struct utsname utsnameData;
    if (uname(&utsnameData)!= -1){
        return std::string(utsnameData.release);
    }
    return "";
}

// Return the system's memory utilization
float System::MemoryUtilization(){
    std::ifstream file("/proc/meminfo");
    std::string line;
    long totalMemory = 0;
    long freeMemory = 0;
    if(file.is_open()) {
        while(std::getline(file, line)){
            std::istringstream iss(line);
            std::string key;
            long value;
            if (iss >> key >> value){
                if(key == "MemTotal:"){
                    totalMemory = value;
                }else if(key =="MemFree:"){
                    freeMemory = value;
                }
            } 
        }
        file.close();
    }

    //Calculate memory utilization
    if(totalMemory > 0){
        float utilization = 1.0 - static_cast<float>(freeMemory) / totalMemory;
        return utilization;
    }

    return 0.0;
}

// Return the operating system name
std::string System::OperatingSystem(){
    std::ifstream file("/etc/os-release");
    std::string line;
    std::string osName;
    if (file.is_open()) {
        while (std::getline(file, line)){
            std::istringstream iss(line);
            std::string key;
            std::string value;
            if(std::getline(iss,key, '=')){
                if(std::getline(iss, value)){
                    if(key == "PRETTY_NAME"){
                        osName = value;
                        break;
                    }
                }
            }
        }
        file.close();
    }

    return osName;
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses(){
    std::string path = "/proc";
    int runningProcesses = 0;

    DIR* directory = opendir(path.c_str());
    if(directory != nullptr) {
        while(dirent* entry = readdir(directory)){
            if(entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
                runningProcesses++;
            }
        }
        closedir(directory);
    }
    return runningProcesses;
}

// Return the total number of processes on the system
int System::TotalProcesses() {
    std::string path = "/proc";
    int totalProcesses = 0;
    
    DIR* directory = opendir(path.c_str());
    if (directory != nullptr) {
        while (dirent* entry = readdir(directory)) {
            // Check if the entry is a directory and its name is a number (process ID)
            if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
                totalProcesses++;
            }
        }
        closedir(directory);
    }
    
    return totalProcesses;
}

//  Return the number of seconds since the system started running
long int System::UpTime() { 
    return LinuxParser::UpTime(); 
    
}
