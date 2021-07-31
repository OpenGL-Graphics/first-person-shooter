#include "controls/key_handler.hpp"

KeyHandler::KeyHandler(const Window& window, Camera& camera, Player& pc):
  m_window(window),
  m_camera(camera),
  m_pc(pc)
{
}

/**
 * Respond to keyboard inputs by notifying observers (iow. by moving camera/pc)
 * Listener for keypress events, called on every frame of mainloop
 */
void KeyHandler::on_keypress() {
  // close window on escape key press (by setting flag & check if set in `is_closed()`)
  if (m_window.is_key_pressed(GLFW_KEY_ESCAPE) || m_window.is_key_pressed(GLFW_KEY_Q)) {
    m_window.close();
  }

  // move camera
  if (m_window.is_key_pressed(GLFW_KEY_W))
    m_camera.move(Direction::FORWARD);
  if (m_window.is_key_pressed(GLFW_KEY_S))
    m_camera.move(Direction::BACKWARD);
  if (m_window.is_key_pressed(GLFW_KEY_A))
    m_camera.move(Direction::LEFT);
  if (m_window.is_key_pressed(GLFW_KEY_D))
    m_camera.move(Direction::RIGHT);

  // move pc (textured cube)
  if (m_window.is_key_pressed(GLFW_KEY_UP))
    m_pc.move(Direction::FORWARD);
  if (m_window.is_key_pressed(GLFW_KEY_DOWN))
    m_pc.move(Direction::BACKWARD);
  if (m_window.is_key_pressed(GLFW_KEY_LEFT))
    m_pc.move(Direction::LEFT);
  if (m_window.is_key_pressed(GLFW_KEY_RIGHT))
    m_pc.move(Direction::RIGHT);
}
