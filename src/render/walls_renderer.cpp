#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "render/walls_renderer.hpp"
#include "geometries/cube.hpp"

WallsRenderer::WallsRenderer():
  m_renderer(Program("assets/shaders/texture_cube.vert", "assets/shaders/texture_cube.frag"), VBO(Cube()), {{0, "position", 3, 6, 0}}),
  m_texture(Image("assets/images/level/wall_diffuse.jpg"))
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
      offsets[0] = glm::vec3(m_wall_tile_length / 2, m_height / 2, m_wall_thickness / 2);
      break;
    case WallOrientation::VERTICAL:
      offsets[0] = glm::vec3(m_wall_thickness / 2, m_height / 2, -m_wall_tile_length / 2);
      break;
    case WallOrientation::L_SHAPED:
      offsets[0] = glm::vec3(m_wall_thickness / 2, m_height / 2, -m_wall_tile_length / 2);
      offsets[1] = glm::vec3(m_wall_tile_length / 2, m_height / 2, m_wall_thickness / 2);
      break;
    case WallOrientation::GAMMA_SHAPED:
      offsets[0] = glm::vec3(m_wall_tile_length / 2, m_height / 2, m_wall_thickness / 2);
      offsets[1] = glm::vec3(m_wall_thickness / 2, m_height / 2, m_wall_tile_length / 2);
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
    glm::mat4 model = glm::scale(
      glm::rotate(
        glm::translate(glm::mat4(1.0f), position),
        glm::radians(angle),
        glm::vec3(0.0f, 1.0f, 0.0f)
      ),
      glm::vec3(1.0f, m_height, m_wall_thickness)
    );

    glm::mat4 normal_mat = glm::inverseTranspose(model);
    Uniforms uniforms;
    uniforms["normal_mat"] = normal_mat;
    uniforms["texture3d"] = m_texture;

    // vertical scaling then rotation around y-axis then translation
    m_renderer.set_transform({ model, m_transformation.view, m_transformation.projection });
    m_renderer.draw(uniforms);
  }
}

/* Draw wall below window located at `position_tile` & another above it */
void WallsRenderer::draw_walls_around_window(const glm::vec3& position_tile) {
  float height_window = 1.0f;
  float height_wall = (m_height - height_window) / 2;
  glm::vec3 scale = glm::vec3(1, height_wall, m_wall_thickness);

  // wall below window
  Uniforms uniforms;
  uniforms["texture3d"] = m_texture;
  glm::vec3 position_bottom = position_tile + glm::vec3(m_wall_tile_length / 2, height_wall / 2, m_wall_thickness / 2);
  glm::mat4 model_bottom = glm::scale(
    glm::translate(glm::mat4(1.0f), position_bottom),
    scale 
  );
  uniforms["normal_mat"] = glm::inverseTranspose(model_bottom);
  m_renderer.set_transform({ model_bottom, m_transformation.view, m_transformation.projection });
  m_renderer.draw(uniforms);

  // wall above window
  glm::vec3 position_top = position_bottom + glm::vec3(0, height_wall + height_window, 0);
  glm::mat4 model_top = glm::scale(
    glm::translate(glm::mat4(1.0f), position_top),
    scale
  );
  uniforms["normal_mat"] = glm::inverseTranspose(model_top);
  m_renderer.set_transform({ model_top, m_transformation.view, m_transformation.projection });
  m_renderer.draw(uniforms);
}

void WallsRenderer::free() {
  m_renderer.free();
  m_texture.free();
}
