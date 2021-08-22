#ifndef MONITOR_HPP
#define MONITOR_HPP

#include <GLFW/glfw3.h>

/* Wrapper struct for GLFWmonitor */
struct Monitor {
  Monitor();
  int width;
  int height;
  GLFWmonitor* m;
};

#endif // MONITOR_HPP
