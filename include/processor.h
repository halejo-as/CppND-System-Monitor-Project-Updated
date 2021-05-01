#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

#include "linux_parser.h"

class Processor {
 public:
  float Utilization();
  std::vector<long long> parsingVector(std::vector<std::string>);
  long long getSum(std::string name, std::vector<int> kValues);

  // TODO: Declare any necessary private members
 private:
  std::vector<long long> cpu_states_prev_{0};
  std::vector<long long> cpu_states_{0};
};

#endif
