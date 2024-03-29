#include <iostream>

#include "controls/mouse_handler.hpp"

#include "math/transformation.hpp"
#include "math/bounding_box.hpp"
#include "math/ray.hpp"

#include "entries/target_entry.hpp"

#include "globals/targets.hpp"
#include "globals/score.hpp"

/* Static class members require a declaration in *.cpp (to allocate space for them) */
CameraFPS* MouseHandler::m_camera;
Window* MouseHandler::m_window;
int MouseHandler::m_xmouse;
int MouseHandler::m_ymouse;
Audio* MouseHandler::m_audio;

/**
 * Initialize static members
 * @param window
 * @param camera Pointer to camera to control with mouse
 * @param cube Check for its intersection with camera's intersection
 * @param audio
 */
void MouseHandler::init(Window* window, CameraFPS* camera, Audio* audio) {
  // init static members: initial mouse's xy-coords at center of screen
  m_camera = camera;
  m_window = window;
  m_xmouse = m_window->width / 2;
  m_ymouse = m_window->height / 2;
  m_audio = audio;
}

/**
 * listener for click on mouse buttons
 * Check for intersection between camera's line of sight & cube
 */
void MouseHandler::on_mouse_click(GLFWwindow* window, int button, int action, int mods) {
  // callback called for all mouse buttons and on press & release
  if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS)
    return;

  // play gun shot sound
  m_audio->shot();
  Ray ray(m_camera->position, m_camera->direction);

  for (size_t i_target = 0; i_target < targets.size(); ++i_target) {
    TargetEntry& target_entry = targets[i_target];
    if (target_entry.is_dead)
      continue;

    // TODO: target2 would still be killed before target3 even if former in bg
    std::cout << "Target: " << i_target << '\n';
    BoundingBox bounding_box = target_entry.bounding_box;
    bool is_intersecting = bounding_box.intersects(ray);

    // remove target & increase score on intersection
    if (is_intersecting) {
      target_entry.is_dead = true;
      score++;
      std::cout << "Intersecting!" << '\n';
      return;
    } else {
      std::cout << "Not intersecting!" << '\n';
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
