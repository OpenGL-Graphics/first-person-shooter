#ifndef TREES_RENDERER_HPP
#define TREES_RENDERER_HPP

#include "factories/shaders_factory.hpp"
#include "factories/textures_factory.hpp"
#include "render/model_renderer.hpp"
#include "shader/uniforms.hpp"
#include "navigation/frustum.hpp"

/* Called from LevelRenderer to render trees props */
class TreesRenderer {
public:
  TreesRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory, Assimp::Importer& importer);
  void calculate_uniforms(const std::vector<glm::vec3>& positions);
  void set_transform(const Transformation& t, const Frustum& frustum);
  void draw(const Uniforms& uniforms);
  void free();

private:
  ModelRenderer m_renderer;

  std::vector<glm::vec3> m_positions;
  std::vector<glm::mat4> m_models;
  std::vector<glm::mat4> m_normals_mats;
};

#endif // TREES_RENDERER_HPP
