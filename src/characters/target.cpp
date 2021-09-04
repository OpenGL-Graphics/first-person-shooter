#include "characters/target.hpp"

/**
 * TODO:
 *   1. pass ModelRenderer (like for Player)
 *   2. Both Target & Player inherit from Character class containing draw() & m_is_dead
 */
Target::Target(Renderer* r):
  renderer(r),
  is_dead(false)
{
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Target::draw(const Uniforms& uniforms) {
  if (is_dead) {
    return;
  }

  renderer->draw(uniforms);
}
