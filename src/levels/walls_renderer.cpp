#include <iostream>
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "levels/walls_renderer.hpp"
#include "geometries/cube.hpp"

/* Cubes used for walls as face culling hides back-face & lighting affect it similarly to front one */
WallsRenderer::WallsRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory):
  m_renderer(
    shaders_factory["texture_cube"],
    Cube(false, { m_wall_length, m_wall_height, m_wall_depth }),
    Attributes::get({"position", "normal", "texture_coord"})
  ),
  m_renderer_subwall(
    shaders_factory["texture_cube"],
    Cube(false, { m_wall_length, m_subwall_height, m_wall_depth }),
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
  const unsigned int N_WALLS = entries.size();
  m_models.resize(N_WALLS);

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
  std::vector<glm::mat4> models_bottom(N_WINDOWS);
  std::vector<glm::mat4> models_top(N_WINDOWS);

  // for (const glm::vec3 position_tile : positions_windows) {
  for (size_t i_window = 0; i_window < N_WINDOWS; ++i_window) {
    glm::vec3 position_tile = positions_windows[i_window];
    glm::vec3 position_bottom = position_tile + glm::vec3(m_wall_length / 2, m_subwall_height / 2, m_wall_depth / 2);
    glm::vec3 position_top = position_bottom + glm::vec3(0, m_subwall_height + m_window_height, 0);
    models_bottom[i_window] = glm::translate(glm::mat4(1.0f), position_bottom);
    models_top[i_window] = glm::translate(glm::mat4(1.0f), position_top);
  }

  m_models_around_windows = std::move(models_bottom);
  m_models_around_windows.insert(m_models_around_windows.end(), models_top.begin(), models_top.end());
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

/* Called each frame before draw() to set matrices uniforms */
void WallsRenderer::set_transform(const Transformation& t) {
  m_renderer.set_transform({ m_models, t.view, t.projection });
  m_renderer_subwall.set_transform({ m_models_around_windows, t.view, t.projection });
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
