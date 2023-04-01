#ifndef DOORS_RENDERER_HPP
#define DOORS_RENDERER_HPP

#include "factories/shaders_factory.hpp"
#include "factories/textures_factory.hpp"
#include "render/renderer.hpp"
#include "navigation/frustum.hpp"

/* Called from LevelRenderer to render doors */
class DoorsRenderer {
public:
  DoorsRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory);
  void calculate_uniforms(const std::vector<glm::vec3>& positions);
  void set_transform(const Transformation& t, const Frustum& frustum);
  void draw(const Uniforms& u);
  void free();

private:
  const float m_height = 3.5;

  Renderer m_renderer;

  Texture2D m_tex_diffuse;
  Texture2D m_tex_normal;

  std::vector<glm::vec3> m_positions;
  std::vector<glm::mat4> m_models;
  std::vector<glm::mat4> m_normals_mats;
  std::vector<Texture2D> m_textures_diffuse;
  std::vector<Texture2D> m_textures_normal;
};

#endif // DOORS_RENDERER_HPP
