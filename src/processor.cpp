#include "processor.h"
#include "linux_parser.h"

//Return the aggregate CPU utilization
float Processor::Utilization(){
    long activeJiffies = LinuxParser::ActiveJiffies();
    long idleJiffies = LinuxParser::IdleJiffies();

    //Calculate total jiffies(active + idle)
    long totalJiffies = activeJiffies + idleJiffies;

    // Calculate CPU utilization as the ratio of active jiffies to total jiffies
    float utilization = static_cast<float>(activeJiffies) / totalJiffies;

    return utilization;
}