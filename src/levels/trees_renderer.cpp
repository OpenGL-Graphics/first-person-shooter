#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "levels/trees_renderer.hpp"

TreesRenderer::TreesRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory, Assimp::Importer& importer):
  m_renderer(shaders_factory["texture"], assimp_utils::Model("assets/models/tree/tree.obj", importer), Attributes::get({"position", "normal", "texture_coord", "tangent"}))
{
}

/* Called in LevelRenderer's ctor to avoid inverting matrix every frame */
void TreesRenderer::calculate_uniforms(const std::vector<glm::vec3>& positions) {
  const unsigned int N_TREES = positions.size();
  m_models.resize(N_TREES);
  m_normals_mats.resize(N_TREES);

  for (size_t i_tree = 0; i_tree < N_TREES; ++i_tree) {
    glm::vec3 position = positions[i_tree];
    glm::mat4 model_tree = glm::translate(glm::mat4(1.0f), position);
    m_models[i_tree] = model_tree;
    m_normals_mats[i_tree] = glm::inverseTranspose(model_tree);
  }
}

/* Keeps in uniform matrices only instances inside frustum */
std::vector<glm::mat4> TreesRenderer::get_uniform_mats(const std::string& name, const Frustum& frustum) {
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

void TreesRenderer::set_transform(const Transformation& t, const Frustum& frustum) {
  std::vector<glm::mat4> models = get_uniform_mats("model", frustum);
  std::vector<glm::mat4> normals_mats = get_uniform_mats("normal", frustum);

  m_renderer.set_transform({ models, t.view, t.projection });
  m_renderer.set_uniform_arr("normals_mats", normals_mats);
}

void TreesRenderer::draw(const Uniforms& uniforms) {
  m_renderer.draw(uniforms);
}

void TreesRenderer::free() {
  m_renderer.free();
}
