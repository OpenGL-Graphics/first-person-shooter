#ifndef KEY_HANDLER_HPP
#define KEY_HANDLER_HPP

#include "gui/window.hpp"
#include "render/model_renderer.hpp"

/**
 * Subject class in the Observer design pattern
 * receives input from keyboard and notifies (i.e. moves) PC & camera respectively
 */
class KeyHandler {
public:
  KeyHandler(const Window& window, Camera& camera, ModelRenderer& pc);
  void on_keypress();

private:
  Window m_window;

  /* Observers (references so they can be modified) */
  Camera& m_camera;
  ModelRenderer& m_pc;
};

#endif // KEY_HANDLER_HPP
