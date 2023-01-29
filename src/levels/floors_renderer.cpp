#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "levels/floors_renderer.hpp"
#include "geometries/surface.hpp"

/**
 * @param n_cols Number of tiles on x-axis (floor width)
 * @param n_rows Number of tiles on z-axis (floor depth)
 */
FloorsRenderer::FloorsRenderer(const TexturesFactory& textures_factory, const Program& program, const glm::vec2& size):
  m_size(size),
  m_renderer(program, Surface(size), Attributes::get({"position", "normal", "texture_coord"}, 7, true)),
  m_tex_floor_diffuse(textures_factory.get<Texture2D>("floor_diffuse")),
  m_tex_floor_normal(textures_factory.get<Texture2D>("floor_normal")),
  m_tex_ceiling_diffuse(textures_factory.get<Texture2D>("ceiling_diffuse")),
  m_tex_ceiling_normal(textures_factory.get<Texture2D>("ceiling_normal"))
{
  calculate_uniforms();
}

void FloorsRenderer::set_transform(const Transformation& t) {
  m_transformation = t;
}

/**
 * Draw two horizontal surfaces for floor & ceiling
 * Supports instancing
 */
void FloorsRenderer::draw(const Uniforms& uniforms) {
  m_renderer.set_transform({ m_models_floors, m_transformation.view, m_transformation.projection });
  m_renderer.set_uniform_arr("normals_mats", m_normals_mats_floors);
  m_renderer.set_uniform_arr("textures_diffuse", m_textures_diffuse);
  m_renderer.set_uniform_arr("textures_normal", m_textures_normal);
  m_renderer.draw(uniforms);
}

/**
 * Calculate uniforms for horizontal surfaces (floor & ceiling),
 * in ctor to avoid calling inverseTranspose() every frame
 */
void FloorsRenderer::calculate_uniforms() {
  bool are_floor[] = { true, false };
  m_models_floors.resize(m_n_floors);
  m_normals_mats_floors.resize(m_n_floors);
  m_textures_diffuse.resize(m_n_floors);
  m_textures_normal.resize(m_n_floors);

  for (size_t i_floor = 0; i_floor < m_n_floors; ++i_floor) {
    bool is_floor = are_floor[i_floor];

    // floor rotated in opposite dir. (need to be shifted back to origin)
    float angle = (is_floor) ? glm::radians(-90.0f) : glm::radians(90.0f);
    const float y = (is_floor) ? 0 : m_height;
    glm::vec3 position = glm::vec3(0.0f, y, 0.0f);

    if (is_floor)
      position += glm::vec3(0, 0, m_size.y);

    // calculate normal matrix only once (instead of doing it in shader for every vertex)
    // xy scaling then rotation around x-axis then translation
    glm::mat4 model = glm::rotate(
      glm::translate(glm::mat4(1.0f), position),
      angle,
      glm::vec3(1.0f, 0.0f, 0.0f)
    );
    glm::mat4 normal_mat = glm::inverseTranspose(model);

    m_models_floors[i_floor] = model;
    m_normals_mats_floors[i_floor] = normal_mat;
    m_textures_diffuse[i_floor] = is_floor ? m_tex_floor_diffuse : m_tex_ceiling_diffuse;
    m_textures_normal[i_floor] = is_floor ? m_tex_floor_normal : m_tex_ceiling_normal;
  }
}

void FloorsRenderer::free() {
  m_renderer.free();
}
