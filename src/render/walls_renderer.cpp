#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "render/walls_renderer.hpp"
#include "geometries/cube.hpp"

/* Cubes used for walls as face culling hides back-face & lighting affect it similarly to front one */
WallsRenderer::WallsRenderer(const TexturesFactory& textures_factory, const Program& program):
  m_renderer(
    program,
    Cube(false, { m_wall_length, m_wall_height, m_wall_depth }),
    Attributes::get({"position", "normal", "texture_coord"})
  ),
  m_renderer_subwall(
    program,
    Cube(false, { m_wall_length, m_subwall_height, m_wall_depth }),
    Attributes::get({"position", "normal", "texture_coord"})
  ),

  m_texture(textures_factory.get<Texture2D>("wall_diffuse"))
{
}

void WallsRenderer::set_transform(const Transformation& t) {
  m_transformation = t;
}

/* Draw all walls at positions given in constructor */
void WallsRenderer::draw(const std::vector<WallEntry>& entries) {
  for (const WallEntry& entry : entries) {
    draw_wall(entry);
  }
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

/**
 * Draw wall rotated by `angle` (in deg) around y-axis at entry's position
 * Actual position & angle calculated accord. to wall's orientation
 */
void WallsRenderer::draw_wall(const WallEntry& entry) {
  // L-shaped and inverse L-shaped wall composed of two wall tiles
  unsigned n_walls = (entry.orientation == WallOrientation::L_SHAPED || entry.orientation == WallOrientation::GAMMA_SHAPED) ? 2 : 1;
  std::array<glm::vec3, 2> offsets = calculate_offsets(entry);
  std::array<float, 2> angles = calculate_angles(entry);

  for (size_t i_wall = 0; i_wall < n_walls; i_wall++) {
    glm::vec3 offset = offsets[i_wall];
    glm::vec3 position = entry.position + offset;

    // calculate normal matrix only once (instead of doing it in shader for every vertex)
    float angle = angles[i_wall];
    glm::mat4 model = glm::rotate(
      glm::translate(glm::mat4(1.0f), position),
      glm::radians(angle),
      glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // TODO: inverse transpose calculated in every iteration :/
    glm::mat4 normal_mat = glm::inverseTranspose(model);
    Uniforms uniforms;
    uniforms["normal_mat"] = normal_mat;
    uniforms["texture2d"] = m_texture;

    // vertical scaling then rotation around y-axis then translation
    m_renderer.set_transform({ {model}, m_transformation.view, m_transformation.projection });
    m_renderer.draw(uniforms);
  }
}

/* Draw wall below window located at `position_tile` & another above it */
void WallsRenderer::draw_walls_around_window(const glm::vec3& position_tile) {
  Uniforms uniforms;
  uniforms["texture2d"] = m_texture;

  // wall below window
  glm::vec3 position_bottom = position_tile + glm::vec3(m_wall_length / 2, m_subwall_height / 2, m_wall_depth / 2);
  glm::mat4 model_bottom = glm::translate(glm::mat4(1.0f), position_bottom);
  uniforms["normal_mat"] = glm::inverseTranspose(model_bottom);
  m_renderer_subwall.set_transform({ {model_bottom}, m_transformation.view, m_transformation.projection });
  m_renderer_subwall.draw(uniforms);

  // wall above window
  glm::vec3 position_top = position_bottom + glm::vec3(0, m_subwall_height + m_window_height, 0);
  glm::mat4 model_top = glm::translate(glm::mat4(1.0f), position_top);
  uniforms["normal_mat"] = glm::inverseTranspose(model_top);
  m_renderer_subwall.set_transform({ {model_top}, m_transformation.view, m_transformation.projection });
  m_renderer_subwall.draw(uniforms);
}

void WallsRenderer::free() {
  m_renderer.free();
}
