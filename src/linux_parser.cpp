#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::stoi;
using std::stol;
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

float LinuxParser::MemoryUtilization() {
  float memTotal{0.0}, memFree{0.0};
  string line, mem, value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> mem >> value;
    memTotal = stof(value);

    std::getline(stream, line);
    linestream.str(line);
    linestream >> mem >> value;
    memFree = stof(value);
  }
  return (memTotal - memFree) / memTotal;
}

long LinuxParser::UpTime() {
  string line, uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return stol(uptime);
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

long LinuxParser::ActiveJiffies(int pid) {
  string line, word;
  long totalTime = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 1; i < 18; i++) {
      linestream >> word;
      if (i > 13) totalTime += stol(word);
    }
    return totalTime / sysconf(_SC_CLK_TCK);
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_str;
  string line, word;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> word;
    while (linestream >> word) cpu_str.push_back(word);
  }
  return cpu_str;
}

int LinuxParser::TotalProcesses() {
  string line, name, processes;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> name;
      if (name == "processes") {
        linestream >> processes;
        return stoi(processes);
      }
    }
  }
  return -1;
}

int LinuxParser::RunningProcesses() {
  string line, name, proc;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> name;
      if (name == "procs_running") {
        linestream >> proc;
        return stoi(proc);
      }
    }
  }
  return -1;
}

string LinuxParser::Command(int pid) {
  string line, cmd_str;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmd_str;
  }
  return cmd_str;
}

string LinuxParser::Ram(int pid) {
  string line, name, ram;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> name;
      if (name == "VmSize:") {
        linestream >> ram;
        ram = to_string(stoi(ram) / 1000);
        return ram;
      }
    }
  }
  return "0";
}

string LinuxParser::Uid(int pid) {
  string line, name, uid;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> name;
      if (name == "Uid:") {
        linestream >> uid;
        break;
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid) {
  string line, user, x_str, uid;
  string uid_target = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x_str >> uid;
      if (uid == uid_target) {
        break;
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) {
  string line, uptime;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 1; i < 23; i++) linestream >> uptime;
    return UpTime() - stol(uptime) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}
