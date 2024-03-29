#include <iostream>
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "levels/walls_renderer.hpp"
#include "geometries/cube.hpp"

using namespace geometry;

/* Cubes used for walls as face culling hides back-face & lighting affect it similarly to front one */
WallsRenderer::WallsRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory):
  m_bbox(glm::vec3(0), m_size / 2.0f),
  m_bbox_around_windows(glm::vec3(0), m_size_around_window / 2.0f),

  m_renderer(
    shaders_factory["texture_cube"],
    Cube(false, m_size),
    Attributes::get({"position", "normal", "texture_coord"})
  ),
  m_renderer_subwall(
    shaders_factory["texture_cube"],
    Cube(false, m_size_around_window),
    Attributes::get({"position", "normal", "texture_coord"})
  ),

  m_texture(textures_factory.get<Texture2D>("wall_diffuse"))
{
}

/* Calculate offset rel. to tile map position in entry (unit cube geometry centered around origin) */
std::array<glm::vec3, 2> WallsRenderer::calculate_offsets(const WallEntry& entry) {
  std::array<glm::vec3, 2> offsets;

  switch (entry.orientation) {
    case WallOrientation::HORIZONTAL:
      offsets[0] = glm::vec3(m_wall_length / 2, m_wall_height / 2, m_wall_depth / 2);
      break;
    case WallOrientation::VERTICAL:
      offsets[0] = glm::vec3(m_wall_depth / 2, m_wall_height / 2, -m_wall_length / 2);
      break;
    case WallOrientation::L_SHAPED:
      offsets[0] = glm::vec3(m_wall_depth / 2, m_wall_height / 2, -m_wall_length / 2);
      offsets[1] = glm::vec3(m_wall_length / 2, m_wall_height / 2, m_wall_depth / 2);
      break;
    case WallOrientation::GAMMA_SHAPED:
      offsets[0] = glm::vec3(m_wall_length / 2, m_wall_height / 2, m_wall_depth / 2);
      offsets[1] = glm::vec3(m_wall_depth / 2, m_wall_height / 2, m_wall_length / 2);
  }

  return offsets;
}

/* Calculate rotation angle in degrees around y-axis */
std::array<float, 2> WallsRenderer::calculate_angles(const WallEntry& entry) {
  std::array<float, 2> angles;

  switch (entry.orientation) {
    case WallOrientation::HORIZONTAL:
      angles[0] = 0;
      break;
    case WallOrientation::VERTICAL:
      angles[0] = 90;
      break;
    case WallOrientation::L_SHAPED:
      angles[0] = 90;
      angles[1] = 0;
      break;
    case WallOrientation::GAMMA_SHAPED:
      angles[0] = 0;
      angles[1] = 90;
  }

  return angles;
}

/* Calculate model matrices for full walls */
void WallsRenderer::calculate_uniforms_full(const std::vector<WallEntry>& entries) {
  const size_t N_WALLS = entries.size();

  // Draw wall rotated by `angle` (in deg) around y-axis at entry's position
  // Actual position & angle calculated accord. to wall's orientation
  for (const WallEntry& entry : entries) {
    // L-shaped and inverse L-shaped wall composed of two wall tiles
    unsigned int n_walls = (entry.orientation == WallOrientation::L_SHAPED || entry.orientation == WallOrientation::GAMMA_SHAPED) ? 2 : 1;
    std::array<glm::vec3, 2> offsets = calculate_offsets(entry);
    std::array<float, 2> angles = calculate_angles(entry);

    for (size_t i_wall_piece = 0; i_wall_piece < n_walls; i_wall_piece++) {
      glm::vec3 offset = offsets[i_wall_piece];
      glm::vec3 position = entry.position + offset;

      // calculate normal matrix only once (instead of doing it in shader for every vertex)
      float angle = angles[i_wall_piece];
      glm::mat4 model = glm::rotate(
        glm::translate(glm::mat4(1.0f), position),
        glm::radians(angle),
        glm::vec3(0.0f, 1.0f, 0.0f)
      );

      m_models.push_back(model);
    } // END WALLS PIECES
  } // END WALLS
}

/* Calculate model matrices for two walls above & above each window */
void WallsRenderer::calculate_uniforms_around_window(const std::vector<glm::vec3>& positions_windows) {
  const unsigned int N_WINDOWS = positions_windows.size();

  for (size_t i_window = 0; i_window < N_WINDOWS; ++i_window) {
    glm::vec3 position_tile = positions_windows[i_window];
    glm::vec3 position_bottom = position_tile + glm::vec3(m_wall_length / 2, m_subwall_height / 2, m_wall_depth / 2);
    glm::vec3 position_top = position_bottom + glm::vec3(0, m_subwall_height + m_window_height, 0);
    glm::mat4 model_bottom = glm::translate(glm::mat4(1.0f), position_bottom);
    glm::mat4 model_top = glm::translate(glm::mat4(1.0f), position_top);

    m_models_around_windows.insert(m_models_around_windows.end(), { model_bottom, model_top });
  }
}

/**
 * Called in LevelRenderer's ctor to avoid inverting matrix every frame
 * @param entries Define positions/angles & # of walls for full walls
 * @param positions_windows Used to draw two walls below & above windows
 */
void WallsRenderer::calculate_uniforms(const std::vector<WallEntry>& entries, const std::vector<glm::vec3>& positions_windows) {
  calculate_uniforms_full(entries);
  calculate_uniforms_around_window(positions_windows);
}

/* Calculate bboxes for full walls OR for walls around windows (accod. to is_around_window) */
std::vector<BoundingBox> WallsRenderer::calculate_bboxes_for(bool is_around_window) {
  const unsigned int N_WALLS = (is_around_window ? m_models_around_windows.size() : m_models.size());
  std::vector<BoundingBox> bboxes(N_WALLS);

  // update cube (centered around origin) to world coords using model matrix
  for (size_t i_wall = 0; i_wall < N_WALLS; ++i_wall) {
    glm::mat4 model = (is_around_window ? m_models_around_windows[i_wall] : m_models[i_wall]);
    BoundingBox bbox = (is_around_window ? m_bbox_around_windows : m_bbox);
    bbox.transform(model);
    bboxes[i_wall] = bbox;
  }

  return bboxes;
}

/* Must be called after calculate_uniforms() */
void WallsRenderer::calculate_bboxes() {
  m_bboxes = calculate_bboxes_for(false);
  m_bboxes_around_windows = calculate_bboxes_for(true);
}

/* Called each frame before draw() to set matrices uniforms */
void WallsRenderer::set_transform(const Transformation& t, const Frustum& frustum) {
  std::vector<glm::mat4> models = frustum.cull(m_models, m_bboxes);
  std::vector<glm::mat4> models_around_windows = frustum.cull(m_models_around_windows, m_bboxes_around_windows);

  m_renderer.set_transform({ models, t.view, t.projection });
  m_renderer_subwall.set_transform({ models_around_windows, t.view, t.projection });
}

/**
 * Draw all walls at positions parsed from tilemap in LevelRenderer's ctor
 * Supports instancing
 */
void WallsRenderer::draw() {
  m_renderer.draw({ {"texture2d", m_texture} });
}

/**
 * Supports instancing
 */
void WallsRenderer::draw_walls_around_window() {
  m_renderer_subwall.draw({ {"texture2d", m_texture} });
}

void WallsRenderer::free() {
  m_renderer.free();
}
