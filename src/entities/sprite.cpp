#include <glm/gtc/matrix_transform.hpp>

#include "entities/sprite.hpp"
#include "geometries/surface.hpp"

/**
 * 2D sprite for props
 * Grass prop instances generated inside level accord. to tilemap
 */
Sprite::Sprite(const Texture2D& texture, const Program& program):
  m_texture(texture),
  renderer(program, Surface(), {
    {0, "position", 2, 7, 0},
    {1, "normal", 3, 7, 2},
    {2, "texture_coord", 2, 7, 5},
  })
{
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

/* Free texture, renderer (vao/vbo buffers) */
void Sprite::free() {
  m_texture.free();
  renderer.free();
}
