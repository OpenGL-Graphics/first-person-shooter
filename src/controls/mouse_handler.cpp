#include <iostream>

#include "controls/mouse_handler.hpp"
#include "math/transformation.hpp"

/* Static class members require a declaration in *.cpp (to allocate space for them) */
Camera* MouseHandler::m_camera;
Window* MouseHandler::m_window;
std::vector<Target *> MouseHandler::m_targets;
int MouseHandler::m_xmouse;
int MouseHandler::m_ymouse;

/**
 * Initialize static members
 * @param window
 * @param camera Pointer to camera to control with mouse
 * @param cube Check for its intersection with camera's intersection
 */
void MouseHandler::init(Window* window, Camera* camera, std::vector<Target *> targets) {
  // init static members: initial mouse's xy-coords at center of screen
  m_camera = camera;
  m_window = window;
  m_xmouse = m_window->width / 2;
  m_ymouse = m_window->height / 2;
  m_targets = targets;
}

/**
 * listener for click on mouse buttons
 * Check for intersection between camera's line of sight & cube
 */
void MouseHandler::on_mouse_click(GLFWwindow* window, int button, int action, int mods) {
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
    for (Target* target : m_targets) {
      BoundingBox bounding_box = target->renderer->bounding_box;
      bool is_intersecting = bounding_box.intersects(m_camera->position, m_camera->direction);

      if (is_intersecting) {
        target->is_dead = true;
        std::cout << "Intersecting!" << '\n';
      } else {
        std::cout << "Not intersecting!" << '\n';
      }
    }
  }
}

/* Respond to mouse inputs by notifying observer (i.e. by rotating camera) */
void MouseHandler::on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
  // see: https://www.reddit.com/r/opengl/comments/831vpb/
  // calculate offset in mouse cursor position
  float x_offset = -(xpos - m_xmouse);
  float y_offset = -(ypos - m_ymouse);
  m_xmouse = xpos;
  m_ymouse = ypos;

  // horizontal/vertical rotation around y-axis/x-axis accord. to offset
  m_camera->rotate(x_offset, y_offset);
}

/* Respond to mouse inputs by notifying observer (i.e. by zooming in/out using mouse wheel) */
void MouseHandler::on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
  if (yoffset == 1) {
    m_camera->zoom(Zoom::IN);
  } else if (yoffset == -1) {
    m_camera->zoom(Zoom::OUT);
  }
}
