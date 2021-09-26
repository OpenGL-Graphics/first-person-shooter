#include "entities/grass.hpp"
#include "geometries/surface.hpp"
#include "shaders/shader_exception.hpp"

/**
 * Grass prop instances generated inside level accord. to tilemap
 */
Grass::Grass():
  texture(Image("assets/images/surfaces/grass.png")),
  m_program("assets/shaders/texture_surface.vert", "assets/shaders/texture_surface.frag"),
  renderer(m_program, VBO(Surface()), {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}})  // render surface
{
  // vertex or fragment shaders failed to compile
  if (m_program.has_failed()) {
    throw ShaderException();
  }
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Grass::draw(const Uniforms& uniforms) {
  renderer.draw(uniforms);
}

/* delegate transform to renderer */
void Grass::set_transform(const Transformation& t) {
  renderer.set_transform(t);
}

/* Free texture, renderer (vao/vbo buffers), and shader program */
void Grass::free() {
  texture.free();
  renderer.free();
  m_program.free();
}
