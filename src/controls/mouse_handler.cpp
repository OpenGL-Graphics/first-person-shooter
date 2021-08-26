#include <iostream>

#include "controls/mouse_handler.hpp"
#include "math/transformation.hpp"

/* Static class members require a declaration in *.cpp (to allocate space for them) */
Camera* MouseHandler::m_camera;
int MouseHandler::m_xmouse;
int MouseHandler::m_ymouse;

/**
 * Initialize static members
 * @param xmouse Inital mouse x-position
 * @param ymouse Inital mouse y-position
 * @param camera Pointer to camera to control with mouse
 */
void MouseHandler::init(int xmouse, int ymouse, const Window& window, Camera* camera) {
  // init static members: initial mouse's xy-coords at center of screen
  m_xmouse = xmouse;
  m_ymouse = ymouse;
  m_camera = camera;
}

/**
 * listener for click on mouse buttons
 * Raycasting theory: https://antongerdelan.net/opengl/raycasting.html
 */
void MouseHandler::on_mouse_click(GLFWwindow* window, int button, int action, int mods) {
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
    // gun crosshair at center of screen in clip space (NDC)
    glm::vec4 position_ndc(0.0f, 0.0f, -1.0f, 1.0f);
    // glm::vec4 position_world = transformation.transform_inv(position_ndc);

    std::cout << "LMB clicked! "
              << " xmouse: " << m_xmouse
              << " ymouse: " << m_ymouse
              << '\n';
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
