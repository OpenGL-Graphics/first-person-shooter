#include <glm/gtc/matrix_transform.hpp>

#include "levels/windows_renderer.hpp"
#include "geometries/surface.hpp"

/**
 * 2D sprite for props
 * Grass prop instances generated inside level accord. to tilemap
 */
WindowsRenderer::WindowsRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory):
  m_texture(textures_factory.get<Texture2D>("window")),
  m_renderer(shaders_factory["texture_surface"], Surface(), Attributes::get({"position", "normal", "texture_coord"}, 7, true))
{
}

/**
 * Called in LevelRenderer's ctor to avoid inverting matrix every frame
 * Draw windows at mid y-coord of `position_tile`
 */
void WindowsRenderer::calculate_uniforms(const std::vector<glm::vec3>& positions) {
  const unsigned int N_WINDOWS = positions.size();
  m_models.resize(N_WINDOWS);

  for (size_t i_window = 0; i_window < N_WINDOWS; ++i_window) {
    glm::vec3 position_tile = positions[i_window];
    float y_window_bottom = m_height/2.0f - height_window/2.0f;
    glm::vec3 position_window(position_tile.x, y_window_bottom, position_tile.z);
    m_models[i_window] = glm::translate(glm::mat4(1.0f), position_window);
  }
}

/**
 * Delegate transform to renderer
 * Supports instancing (multiple transparent windows)
 */
void WindowsRenderer::set_transform(const Transformation& t) {
  m_renderer.set_transform({ m_models, t.view, t.projection });
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void WindowsRenderer::draw() {
  // with face culling enabled, one face (back) of windows surfaces not rendered
  glDisable(GL_CULL_FACE);
  m_renderer.draw({ {"texture2d", m_texture} });
  glEnable(GL_CULL_FACE);
}

/* Free texture, renderer (vao/vbo buffers) */
void WindowsRenderer::free() {
  m_renderer.free();
}
