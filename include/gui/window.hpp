#ifndef WINDOW_HPP
#define WINDOW_HPP

// prevent glfw from including glad (opengl loading library)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>

#include "gui/monitor.hpp"

/* Wrapper struct for GLFW windows */
struct Window {
  int width;
  int height;

  Window(const std::string& title);
  void make_context();
  void render();
  void process_events();
  bool is_null();
  bool is_closed();
  void destroy();
  void attach_mouse_listener(GLFWmousebuttonfun	on_mouse_click);
  bool is_key_pressed(int key) const;
  void close() const;

private:
  GLFWwindow* m_window;
};

#endif // WINDOW_HPP
