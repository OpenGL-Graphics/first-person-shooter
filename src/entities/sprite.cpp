#include <glm/gtc/matrix_transform.hpp>

#include "entities/sprite.hpp"
#include "geometries/surface.hpp"
#include "shader_exception.hpp"

/**
 * 2D sprite for props
 * Grass prop instances generated inside level accord. to tilemap
 */
Sprite::Sprite(const Texture2D& texture):
  m_texture(texture),
  m_program("assets/shaders/texture_surface.vert", "assets/shaders/texture_surface.frag"),
  renderer(m_program, VBO(Surface()), {
    {0, "position", 2, 7, 0},
    {1, "texture_coord", 2, 7, 2},
    {2, "normal", 3, 7, 4}
  })
{
  // vertex or fragment shaders failed to compile
  if (m_program.has_failed()) {
    throw ShaderException();
  }
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Sprite::draw(const Uniforms& u) {
  Uniforms uniforms = u;
  uniforms["texture2d"] = m_texture;
  renderer.draw(uniforms);
}

/* delegate transform to renderer */
void Sprite::set_transform(const Transformation& t) {
  renderer.set_transform(t);
}

/* Free texture, renderer (vao/vbo buffers), and shader program */
void Sprite::free() {
  m_texture.free();
  renderer.free();
  m_program.free();
}
