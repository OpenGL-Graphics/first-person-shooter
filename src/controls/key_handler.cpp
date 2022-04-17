#include "controls/key_handler.hpp"

KeyHandler::KeyHandler(const Window& window, Camera& camera):
  m_window(window),
  m_camera(camera)
{
}

/**
 * Respond to keyboard inputs by notifying observers (iow. by moving camera)
 * Listener for keypress events, called on every frame of mainloop
 */
void KeyHandler::on_keypress() {
  // close window on escape key press (by setting flag & check if set in `is_closed()`)
  if (m_window.is_key_pressed(GLFW_KEY_ESCAPE) || m_window.is_key_pressed(GLFW_KEY_Q)) {
    m_window.close();
  }

  // TODO: if <spacebar> is pressed while jumping, camera can stick to ceiling
  if (m_window.is_key_pressed(GLFW_KEY_SPACE)) {
    m_camera.is_jumping = true;
  } else {
    // move camera in 6 directions (no else to support oblique movement)
    if (m_window.is_key_pressed(GLFW_KEY_W))
      m_camera.move(Direction::FORWARD);
    if (m_window.is_key_pressed(GLFW_KEY_S))
      m_camera.move(Direction::BACKWARD);
    if (m_window.is_key_pressed(GLFW_KEY_A))
      m_camera.move(Direction::LEFT);
    if (m_window.is_key_pressed(GLFW_KEY_D))
      m_camera.move(Direction::RIGHT);
    if (m_window.is_key_pressed(GLFW_KEY_F))
      m_camera.move(Direction::DOWN);
    if (m_window.is_key_pressed(GLFW_KEY_R))
      m_camera.move(Direction::UP);
  }
}
