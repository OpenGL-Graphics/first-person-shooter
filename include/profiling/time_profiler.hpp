#ifndef TIME_PROFILER_HPP
#define TIME_PROFILER_HPP

#include <chrono>
#include <string>

using namespace std::chrono;

class TimeProfiler {
public:
  void start();
  void stop(const std::string& label);

private:
  steady_clock::time_point time_start;
};

#endif // TIME_PROFILER_HPP
