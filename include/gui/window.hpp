#ifndef WINDOW_HPP
#define WINDOW_HPP

// prevent glfw from including glad (opengl loading library)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "gui/monitor.hpp"
#include "navigation/camera.hpp"

/* Wrapper struct for GLFW windows */
struct Window {
  int width;
  int height;

  Window(Camera* camera);
  void make_context();
  void render();
  void process_events();
  bool is_null();
  bool is_closed();
  void destroy();
  void attach_listeners();
  bool is_key_pressed(int key) const;
  void close() const;

private:
  GLFWwindow* m_window;

  /* declared as a pointer (not ref. as static members are defined in *.cpp), so camera can respond to mouse events (i.e. be modified) */
  static Camera* m_camera;

  /* previous mouse xy-coords to compare to when moving (modified inside listeners below) */
  static int m_xmouse;
  static int m_ymouse;

  /* static methods can be easily converted to function pointers callbacks (no `this` argument) */
  static void on_mouse_click(GLFWwindow* window, int button, int action, int mods);
  static void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
  static void on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);
};

#endif // WINDOW_HPP
