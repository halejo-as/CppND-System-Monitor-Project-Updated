#include "processor.h"

using namespace LinuxParser;

std::vector<long long> Processor::parsingVector(
    std::vector<std::string> vec_str) {
  std::vector<long long> vec_int;
  for (std::string s : vec_str) vec_int.push_back(stoll(s));
  return vec_int;
}

long long Processor::getSum(std::string name, std::vector<int> kValues) {
  long long sum = 0;
  if (name == "prev") {
    for (int i : kValues) sum += cpu_states_prev_[i];
  } else if (name == "now") {
    for (int i : kValues) sum += cpu_states_[i];
  } else
    return -1;
  return sum;
}

float Processor::Utilization() {
  cpu_states_ = parsingVector(CpuUtilization());

  long long prevIdle = getSum("prev", {kIdle_, kIOwait_});
  long long idle = getSum("now", {kIdle_, kIOwait_});

  long long prevNonIdle =
      getSum("prev", {kUser_, kNice_, kSystem_, kIRQ_, kSoftIRQ_, kSteal_});
  long long nonIdle =
      getSum("now", {kUser_, kNice_, kSystem_, kIRQ_, kSoftIRQ_, kSteal_});

  long long prevTotal = prevIdle + prevNonIdle;
  long long total = idle + nonIdle;

  long long totald = total - prevTotal;
  long long idled = idle - prevIdle;

  cpu_states_prev_ = cpu_states_;

  return (totald - idled) / (float)totald;
}
