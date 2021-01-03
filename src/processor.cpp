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

    long diffTotal = newTotal - total_;
    long diffActive = newActive - active_;
    long diffIdle = newIdle - idle_;

    SetValues(newTotal, newActive, newIdle);

    return float(diffActive / diffTotal) * 100;
}

void Processor::SetValues(long total, long active, long idle) {
    total_ = total;
    active_ = active;
    idle_ = idle;
}