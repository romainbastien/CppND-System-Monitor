#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <map>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::map;
using std::getline;
using std::ifstream;
using std::istringstream;


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


string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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

float LinuxParser::MemoryUtilization() {
  string line;
  string key, valueUnit;
  float value;
  std::ifstream fileStream(kProcDirectory + kMeminfoFilename);
  map<string, float> memUtilizationValues;
  if(fileStream.is_open()){
    for (int i = 0; i < 5; ++i) {
      std::getline(fileStream, line);
      std::istringstream lineStream(line);
      lineStream >> key >> value >> valueUnit;
      std::replace(key.begin(), key.end(), ':', ' ');
      memUtilizationValues.insert(std::pair<string, float>(key, value));
    }
  }
  float memUtilization = (memUtilizationValues["MemTotal"] - memUtilizationValues["MemFree"] - memUtilizationValues["MemBuffer"] - memUtilizationValues["MemCached"]) / memUtilizationValues["MemTotal"];
  return memUtilization;
}

long LinuxParser::UpTime() {
  string line;
  long value;
  std::ifstream fileStream(kProcDirectory + kUptimeFilename);
  if(fileStream.is_open()){
    getline(fileStream, line);
    istringstream lineStream(line);
    lineStream >> value;
  }
  return value;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

int LinuxParser::TotalProcesses() {
  string key, line;
  int value = 0.0;
  ifstream fileStream(kProcDirectory + kStatFilename);
  if(fileStream.is_open()){
    while (getline(fileStream, line)){
      istringstream lineStream(line);
      while (lineStream >> key >> value){
        if(key == "processes"){
          return value;
        }
      }
    }
  }
  return value;
}

int LinuxParser::RunningProcesses() {
  string key, line;
  int value = 0;
  ifstream fileStream(kProcDirectory + kStatFilename);
  if(fileStream.is_open()){
    while (getline(fileStream, line)){
      istringstream lineStream(line);
      while (lineStream >> key >> value){
        if(key == "procs_running"){
          return value;
        }
      }
    }
  }
  return value;
}


string LinuxParser::Command(int pid) {
  string line;
  ifstream fileStream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (fileStream.is_open()) {
    std::getline(fileStream, line);
    return line;
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, key;
  string value = "";
  ifstream fileStream(kPasswordPath);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      istringstream lineStream(line);
      while (lineStream >> value >> key) {
        if (key == uid) {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }