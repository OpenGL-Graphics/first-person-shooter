#ifndef KEY_HANDLER_HPP
#define KEY_HANDLER_HPP

#include "window.hpp"
#include "navigation/camera_fps.hpp"

/**
 * Subject class in the Observer design pattern
 * receives input from keyboard and notifies (i.e. moves) camera
 */
class KeyHandler {
public:
  KeyHandler(const Window& window, CameraFPS& camera);
  void on_keypress();

private:
  Window m_window;

  /* Observers (references so they can be modified) */
  CameraFPS& m_camera;
};

#endif // KEY_HANDLER_HPP
