#include "processor.h"

Processor::Processor() {
    total_ = LinuxParser::Jiffies();
    active_ = LinuxParser::ActiveJiffies();
    idle_ = LinuxParser::IdleJiffies();
}

float Processor::Utilization() {
    long newTotal = LinuxParser::Jiffies();
    long newActive = LinuxParser::ActiveJiffies();
    long newIdle = LinuxParser::IdleJiffies();


    long diffTotal = newTotal > total_ ? newTotal - total_ : newTotal;
    long diffActive = newActive > active_ ? newActive - active_ : newActive;
    //long diffIdle = newIdle > idle_ ? newIdle - idle_ : newIdle;

    SetValues(newTotal, newActive, newIdle);

    auto utilization = float(diffActive) / float(diffTotal);

    return utilization;
}

void Processor::SetValues(long total, long active, long idle) {
    total_ = total;
    active_ = active;
    idle_ = idle;
}