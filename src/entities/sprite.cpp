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
template <size_t N_INSTANCES>
void Sprite::set_transform(const Transformation<N_INSTANCES>& t) {
  renderer.set_transform(t);
}

/* Free texture, renderer (vao/vbo buffers) */
void Sprite::free() {
  m_texture.free();
  renderer.free();
}

// template instantiation to avoid linking error
template void Sprite::set_transform<1>(const Transformation<1>& transform);
template void Sprite::set_transform<2>(const Transformation<2>& transform);
