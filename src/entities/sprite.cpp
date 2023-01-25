#include <glm/gtc/matrix_transform.hpp>

#include "entities/sprite.hpp"
#include "geometries/surface.hpp"

/**
 * 2D sprite for props
 * Grass prop instances generated inside level accord. to tilemap
 */
Sprite::Sprite(const Texture2D& texture, const Program& program):
  m_texture(texture),
  renderer(program, Surface(), Attributes::get({"position", "normal", "texture_coord"}, 7, true))
{
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Sprite::draw(const Uniforms& u) {
  Uniforms uniforms = u;
  uniforms["texture2d"] = m_texture;
  renderer.draw(uniforms);
}

/**
 * Delegate transform to renderer
 * Supports instancing (multiple transparent windows)
 */
void Sprite::set_transform(const Transformation& t) {
  renderer.set_transform(t);
}

/* Free texture, renderer (vao/vbo buffers) */
void Sprite::free() {
  renderer.free();
}
