#ifndef WINDOW_HPP
#define WINDOW_HPP

// prevent glfw from including glad (opengl loading library)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>

#include "gui/monitor.hpp"
#include "navigation/camera.hpp"

/* Wrapper struct for GLFW windows */
struct Window {
  int width;
  int height;
  GLFWwindow* w;

  Window(const std::string& title);
  void make_context();
  void render();
  void process_events();
  bool is_null();
  bool is_closed();
  void destroy();
  void attach_mouse_listeners(GLFWcursorposfun on_mouse_move, GLFWmousebuttonfun on_mouse_click, GLFWscrollfun on_mouse_scroll);
  bool is_key_pressed(int key) const;
  void close() const;

  glm::mat4 get_projection2d();
  glm::mat4 get_projection3d(const Camera& camera);
};

#endif // WINDOW_HPP
