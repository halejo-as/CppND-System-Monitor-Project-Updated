#include "process.h"
#include "linux_parser.h"

#include <cctype>
#include <sstream>
#include <string>
#include <vector>


using std::string;

Process::Process(int pid) : pid_(pid) {}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  long jiffiesTime = LinuxParser::ActiveJiffies(pid_);
  long startTime = LinuxParser::UpTime(pid_);
  cpu_utilization_ =  float(jiffiesTime) / startTime ;
  return cpu_utilization_;
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator>(Process const& a) const {
  return cpu_utilization_ > a.cpu_utilization_;
}
