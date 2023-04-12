#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "levels/windows_renderer.hpp"
#include "geometries/surface.hpp"
#include "math/bounding_box.hpp"

using namespace geometry;

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
void WindowsRenderer::calculate_uniforms(const std::vector<glm::vec3>& positions_tiles) {
  const unsigned int N_WINDOWS = positions_tiles.size();
  m_models.resize(N_WINDOWS);

  for (size_t i_window = 0; i_window < N_WINDOWS; ++i_window) {
    glm::vec3 position_tile = positions_tiles[i_window];
    float y_window_bottom = m_height/2.0f - m_size.y/2.0f;
    glm::vec3 position_window(position_tile.x, y_window_bottom, position_tile.z);
    m_models[i_window] = glm::translate(glm::mat4(1.0f), position_window);
  }
}

/* Bboxes needed for frustum culling (calculate in level ctor) */
void WindowsRenderer::calculate_bboxes(const std::vector<glm::vec3>& positions_tiles) {
  const unsigned int N_WINDOWS = positions_tiles.size();
  m_bboxes.resize(N_WINDOWS);

  for (size_t i_window = 0; i_window < N_WINDOWS; ++i_window) {
    // surface origin at lower-left corner
    glm::vec3 position_tile = positions_tiles[i_window];
    glm::vec3 position_center(position_tile.x + m_size.x/2, m_height/2.0f, position_tile.z);
    m_bboxes[i_window] = BoundingBox(position_center, m_size / 2.0f);
  }
}

/**
 * Delegate transform to renderer
 * Supports instancing (multiple transparent windows)
 */
void WindowsRenderer::set_transform(const Transformation& t, const Frustum& frustum) {
  std::vector<glm::mat4> models = frustum.cull(m_models, m_bboxes);
  m_renderer.set_transform({ models, t.view, t.projection });
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
