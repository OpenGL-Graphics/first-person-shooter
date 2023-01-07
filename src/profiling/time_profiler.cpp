#include <iostream>

#include "profiling/time_profiler.hpp"

void TimeProfiler::start() {
  time_start = steady_clock::now();
}

void TimeProfiler::stop(const std::string& label) {
  steady_clock::time_point time_stop = steady_clock::now();
  duration<double> interval = time_stop - time_start;

  // show duration time in seconds
  std::cout << "Duration " << label << ": " << interval.count() << "s" << std::endl;
}
