#include <iostream>

#include "profiling/profiler.hpp"

void Profiler::start() {
  time_start = steady_clock::now();
}

void Profiler::stop() {
  time_stop = steady_clock::now();
  interval = time_stop - time_start;
}

void Profiler::print(const std::string& label) {
  // show duration time in seconds
  std::cout << "Duration " << label << ": " << interval.count() << "s" << std::endl;
}
