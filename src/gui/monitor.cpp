#include <GLFW/glfw3.h>

#include "gui/monitor.hpp"

Monitor::Monitor() {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  width = mode->width;
  height = mode->height;
}
