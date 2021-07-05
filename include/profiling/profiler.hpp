#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <chrono>
#include <string>

using namespace std::chrono;

class Profiler {
public:
  void start();
  void stop();
  void print(const std::string& label);

private:
  steady_clock::time_point time_start;
  steady_clock::time_point time_stop;
  duration<double> interval;
};

#endif // PROFILER_HPP
