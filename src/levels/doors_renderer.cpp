#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "levels/doors_renderer.hpp"
#include "geometries/surface.hpp"

using namespace geometry;

DoorsRenderer::DoorsRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory):
  m_renderer(shaders_factory["tile"], Surface(glm::vec2(1, m_height)), Attributes::get({"position", "normal", "texture_coord"}, 7, true)),

  // textures
  m_tex_diffuse(textures_factory.get<Texture2D>("door_diffuse")),
  m_tex_normal(textures_factory.get<Texture2D>("door_normal"))
{
}

/* Called in LevelRenderer's ctor to avoid inverting matrix every frame */
void DoorsRenderer::calculate_uniforms(const std::vector<glm::vec3>& positions) {
  const size_t N_DOORS = positions.size();
  m_models.resize(N_DOORS);
  m_normals_mats.resize(N_DOORS);
  m_textures_diffuse.resize(N_DOORS);
  m_textures_normal.resize(N_DOORS);

  for (size_t i_door = 0; i_door < N_DOORS; ++i_door) {
    glm::vec3 position_door = positions[i_door];
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position_door);
    glm::mat4 normal_mat = glm::inverseTranspose(model);
    m_models[i_door] = model;
    m_normals_mats[i_door] = normal_mat;

    m_textures_diffuse[i_door] = m_tex_diffuse;
    m_textures_normal[i_door] = m_tex_normal;
  }
}

/**
 * Keeps in uniform matrices only instances inside frustum
 * Note: textures not filtered out as all doors have the same textures
 */
std::vector<glm::mat4> DoorsRenderer::get_uniform_mats(const std::string& name, const Frustum& frustum) {
  std::vector<glm::mat4> matrices;

  for (size_t i_model = 0; i_model < m_models.size(); ++i_model) {
    glm::mat4 model = m_models[i_model];
    glm::vec3 position = model[3];

    // check if inside frustum
    if (frustum.is_inside(position)) {
      glm::mat4 mat = (name == "normal") ? m_normals_mats[i_model] : m_models[i_model];
      matrices.push_back(mat);
    }
  }

  return matrices;
}

void DoorsRenderer::set_transform(const Transformation& t, const Frustum& frustum) {
  std::vector<glm::mat4> models = get_uniform_mats("model", frustum);
  std::vector<glm::mat4> normals_mats = get_uniform_mats("normal", frustum);

  m_renderer.set_transform({ models, t.view, t.projection });
  m_renderer.set_uniform_arr("normals_mats", normals_mats);
}

void DoorsRenderer::draw(const Uniforms& u) {
  // with face culling enabled, one face (back) of doors surfaces not rendered
  m_renderer.set_uniform_arr("textures_diffuse", m_textures_diffuse);
  m_renderer.set_uniform_arr("textures_normal", m_textures_normal);
  glDisable(GL_CULL_FACE);
  m_renderer.draw(u);
  glEnable(GL_CULL_FACE);
}

void DoorsRenderer::free() {
  m_renderer.free();
}
