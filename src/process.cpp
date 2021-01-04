#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  pid_ = pid;
  command_ = LinuxParser::Command(pid_);
  ram_ = LinuxParser::Ram(pid_);
  user_ = LinuxParser::User(pid_);
  uptime_ = LinuxParser::UpTime(pid_);
  long time = LinuxParser::ActiveJiffies(pid_);
  cpuUtilization_ = uptime_ > 0 ? float(time) / float(uptime_): 0;
  
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() const {
    return cpuUtilization_;
}


string Process::Command() {
  return command_;
}

string Process::Ram() {
    return ram_;
}


string Process::User() {
  return user_;
}


long int Process::UpTime() {
  return uptime_;
}

bool Process::operator<(Process const& a) const {
    return CpuUtilization() > a.CpuUtilization();
}