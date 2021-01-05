#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>

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
  string line;
  string os, version, kernel;
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
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

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string memTotal, memAvailable = "1";
  ifstream fileStream(kProcDirectory + kMeminfoFilename);
  if(fileStream.is_open()){
    for (int i = 0; i < 5; ++i) {
      getline(fileStream, line);
      istringstream lineStream(line);
      lineStream >> key;
      if(key == "MemTotal:"){
        lineStream >> memTotal;
      }
      else if(key == "MemAvailable:"){
        lineStream >> memAvailable;
      }
    }
  }
  return (std::stof(memTotal) - std::stof(memAvailable)) / std::stof(memTotal);
}

long LinuxParser::UpTime() {
  string line, value;
  std::ifstream fileStream(kProcDirectory + kUptimeFilename);
  if(fileStream.is_open()){
    getline(fileStream, line);
    istringstream lineStream(line);
    lineStream >> value;
  }
  long uptime = std::stol(value);
  return uptime;
}

std::vector<long> LinuxParser::GetJiffies(){
    string line, dump, value;
    std::vector<string> jiffies{};
    ifstream fileStream(kProcDirectory + kStatFilename);
    if(fileStream.is_open()){
        getline(fileStream, line);
        std::istringstream linestream(line);
        linestream >> dump;
        while(linestream >> value){
            jiffies.push_back(value);
        }
    }
    vector<long> jiffiesInt{};
    for(string j: jiffies){
      jiffiesInt.push_back(stol(j));
    }
    return jiffiesInt;
}

long LinuxParser::Jiffies() {
    std::vector<long int> jiffies = GetJiffies();
    return std::accumulate(jiffies.begin(), jiffies.end(), 0);
}


long LinuxParser::ActiveJiffies(int pid) {
    string line, trash;
    long jiffies, totalJiffies{0};
    ifstream fileStream(kProcDirectory + to_string(pid) + kStatFilename);
    if(fileStream.is_open()){
        getline(fileStream, line);
        istringstream lineStream(line);
        for(int i = 0; i < 17; i++){
            if(i >= 13){
                lineStream >> jiffies;
                totalJiffies += jiffies;
            }
            else {
                lineStream >> trash;
            }
        }
    }
    return totalJiffies / sysconf(_SC_CLK_TCK);
}

long LinuxParser::ActiveJiffies() {
    std::vector<long int> jiffies = GetJiffies();
    return std::accumulate(jiffies.begin(), jiffies.end(), 0) - (jiffies.at(CPUStates::kIdle_) + jiffies.at(CPUStates::kIOwait_));
}

long LinuxParser::IdleJiffies() {
    std::vector<long int> jiffies = GetJiffies();
    return jiffies.at(CPUStates::kIdle_) + jiffies.at(CPUStates::kIOwait_);
}

vector<string> LinuxParser::CpuUtilization() {
    return vector<string> {
        to_string(Jiffies()),
        to_string(ActiveJiffies()),
        to_string(IdleJiffies())
    };
}

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

string LinuxParser::GetValueFromStatusFile(int pid, std::string keyName){
    string line, key, value = "0";
    ifstream fileStream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (fileStream.is_open()) {
        while (std::getline(fileStream, line)) {
            std::istringstream lineStream(line);
            while (lineStream >> key >> value) {
                if (key == keyName + ":") {
                    return value;
                }
            }
        }
    }
    return value;
}

string LinuxParser::Ram(int pid) {
    long ram = std::stol(GetValueFromStatusFile(pid, "VmSize"));
    return to_string(ram/1000);
}

string LinuxParser::Uid(int pid) {
    return GetValueFromStatusFile(pid, "Uid");
}

string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, key, value;
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

long LinuxParser::UpTime(int pid) {
    string line, trash;
    long time;
    ifstream fileStream(kProcDirectory + to_string(pid) + kStatFilename);
    if(fileStream.is_open()){
        getline(fileStream, line);
        istringstream lineStream(line);
        for(int i = 0; i < 22; i++){
            if(i == 21){
                lineStream >> time;
            }
            else {
                lineStream >> trash;
            }
        }
    }
    return time / sysconf(_SC_CLK_TCK);
}