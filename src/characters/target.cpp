#include "characters/target.hpp"
#include "geometries/cube.hpp"
#include "shaders/shader_exception.hpp"

/**
 * Targets are generated inside level accord. to tilemap
 * TODO:
 *   1. pass ModelRenderer (like for Player)
 *   2. Both Target & Player inherit from Character class containing draw() & m_is_dead
 */
Target::Target():
  program("assets/shaders/color.vert", "assets/shaders/color.frag"),
  renderer(program, VBO(Cube{}), {{0, "position", 3, 12, 0}, {0, "color", 3, 12, 3}}),  // render colored cube
  is_dead(false)
{
  // vertex or fragment shaders failed to compile
  if (program.has_failed()) {
    throw ShaderException();
  }
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Target::draw(const Uniforms& uniforms) {
  if (is_dead) {
    return;
  }

  renderer.draw(uniforms);
}

/* delegate transform to renderer */
void Target::set_transform(const Transformation& t) {
  renderer.set_transform(t);
}

/* Free renderer (vao/vbo buffers) & shader program */
void Target::free() {
  renderer.free();
  program.free();
}
