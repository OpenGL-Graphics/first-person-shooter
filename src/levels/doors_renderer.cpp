#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "levels/doors_renderer.hpp"
#include "geometries/surface.hpp"

DoorsRenderer::DoorsRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory):
  m_renderer(shaders_factory["tile"], Surface(glm::vec2(1, m_height)), Attributes::get({"position", "normal", "texture_coord"}, 7, true)),

  // textures
  m_tex_diffuse(textures_factory.get<Texture2D>("door_diffuse")),
  m_tex_normal(textures_factory.get<Texture2D>("door_normal"))
{
}

/* Called in LevelRenderer's ctor to avoid inverting matrix every frame */
void DoorsRenderer::calculate_uniforms(const std::vector<glm::vec3>& positions) {
  m_positions = std::move(positions);
  m_n_doors = m_positions.size();

  m_models.resize(m_n_doors);
  m_normals_mats.resize(m_n_doors);
  m_textures_diffuse.resize(m_n_doors);
  m_textures_normal.resize(m_n_doors);

  for (size_t i_door = 0; i_door < m_n_doors; ++i_door) {
    glm::vec3 position_door = m_positions[i_door];
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position_door);
    glm::mat4 normal_mat = glm::inverseTranspose(model);
    m_models[i_door] = model;
    m_normals_mats[i_door] = normal_mat;

    m_textures_diffuse[i_door] = m_tex_diffuse;
    m_textures_normal[i_door] = m_tex_normal;
  }
}

void DoorsRenderer::set_transform(const Transformation& t) {
  m_renderer.set_transform({ m_models, t.view, t.projection });
}

void DoorsRenderer::draw(const Uniforms& u) {
  // with face culling enabled, one face (back) of doors surfaces not rendered
  m_renderer.set_uniform_arr("normals_mats", m_normals_mats);
  m_renderer.set_uniform_arr("textures_diffuse", m_textures_diffuse);
  m_renderer.set_uniform_arr("textures_normal", m_textures_normal);
  glDisable(GL_CULL_FACE);
  m_renderer.draw(u);
  glEnable(GL_CULL_FACE);
}

void DoorsRenderer::free() {
  m_renderer.free();
}
