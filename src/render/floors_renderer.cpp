#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "render/floors_renderer.hpp"
#include "geometries/surface.hpp"

/**
 * @param n_cols Number of tiles on x-axis (floor width)
 * @param n_rows Number of tiles on z-axis (floor depth)
 */
FloorsRenderer::FloorsRenderer(const Program& program, const glm::vec2& size):
  m_size(size),
  m_renderer(program, Surface(size), {
    {0, "position", 2, 7, 0},
    {1, "normal", 3, 7, 2},
    {2, "texture_coord", 2, 7, 5},
  }),
  m_textures {
    {"floor_diffuse", Texture2D(Image("assets/images/level/floor_diffuse.jpg"), GL_TEXTURE0)},
    {"floor_normal", Texture2D(Image("assets/images/level/floor_normal.jpg"), GL_TEXTURE1)},
    {"ceiling_diffuse", Texture2D(Image("assets/images/level/ceiling_diffuse.jpg"), GL_TEXTURE0)},
    {"ceiling_normal", Texture2D(Image("assets/images/level/ceiling_normal.jpg"), GL_TEXTURE1)},
  }
{
}

void FloorsRenderer::set_transform(const Transformation& t) {
  m_transformation = t;
}

/**
 * Draw horizontal surface at given height (level position assumed to be at origin)
 * @param is_floor Needed when face culling enabled & same angle for floor/ceiling => hidden floor
 */
void FloorsRenderer::draw_horizontal_surface(const Uniforms& u, bool is_floor) {
  // floor rotated in opposite dir. (need to be shifted back to origin)
  float angle = (is_floor) ? glm::radians(-90.0f) : glm::radians(90.0f);
  const float y = (is_floor) ? 0 : m_height;
  glm::vec3 position = glm::vec3(0.0f, y, 0.0f);

  if (is_floor) {
    position += glm::vec3(0, 0, m_size.y);
  }

  // calculate normal matrix only once (instead of doing it in shader for every vertex)
  glm::mat4 model = glm::rotate(
    glm::translate(glm::mat4(1.0f), position),
    angle,
    glm::vec3(1.0f, 0.0f, 0.0f)
  );
  glm::mat4 normal_mat = glm::inverseTranspose(model);

  // xy scaling then rotation around x-axis then translation
  Uniforms uniforms = u;
  uniforms["normal_mat"] = normal_mat;
  m_renderer.set_transform({ model, m_transformation.view, m_transformation.projection });
  m_renderer.draw(uniforms);
}

/* Draw horizontal floor covering bottom of tilemap */
void FloorsRenderer::draw_floor(const Uniforms& u) {
  Uniforms uniforms = u;
  uniforms["texture_diffuse"] = m_textures["floor_diffuse"];
  uniforms["texture_normal"] = m_textures["floor_normal"];
  draw_horizontal_surface(uniforms, true);
}

/* Draw horizontal floor covering top of tilemap */
void FloorsRenderer::draw_ceiling(const Uniforms& u) {
  Uniforms uniforms = u;
  uniforms["texture_diffuse"] = m_textures["ceiling_diffuse"];
  uniforms["texture_normal"] = m_textures["ceiling_normal"];
  draw_horizontal_surface(uniforms, false);
}

void FloorsRenderer::draw(const Uniforms& uniforms) {
  draw_floor(uniforms);
  draw_ceiling(uniforms);
}

void FloorsRenderer::free() {
  m_renderer.free();

  // textures
  for (auto& item : m_textures) {
    item.second.free();
  }
}