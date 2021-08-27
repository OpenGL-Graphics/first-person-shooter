#ifndef MOUSE_HANDLER_HPP
#define MOUSE_HANDLER_HPP

#include "gui/window.hpp"
#include "render/model_renderer.hpp"
#include "navigation/camera.hpp"

/**
 * Static class (all its members are static) because it contains only callbacks (function pointers)
 * representing the Subject class in the Observer design pattern
 * receives input from mouse and notifies (i.e. rotates) camera
 */
class MouseHandler {
public:
  /* No need for instance constructor to init static private members */
  static void init(Window* window, Camera* camera, glm::vec3* position_cube);

  /* static methods can be passed as function pointers callbacks (no `this` argument) */
  static void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
  static void on_mouse_click(GLFWwindow* window, int button, int action, int mods);
  static void on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);

private:
  /* Observers */
  /* declared as a pointer (not ref. as static members are defined in *.cpp), so camera can respond to mouse events (i.e. be modified) */
  static Camera* m_camera;

  /* Needed to get 3d projection matrix for raycasting (mouse position to 3d position) */
  static Window* m_window;

  /* Color cube positionned using camera's position & direction vector */
  static glm::vec3* m_postition_cube;

  /* previous mouse xy-coords to compare to when moving (modified inside listeners below) */
  static int m_xmouse;
  static int m_ymouse;
};

#endif // MOUSE_HANDLER_HPP
