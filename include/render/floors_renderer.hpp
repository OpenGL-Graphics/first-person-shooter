#ifndef FLOORS_RENDERER_HPP
#define FLOORS_RENDERER_HPP

#include <vector>

#include "entries/wall_entry.hpp"
#include "factories/textures_factory.hpp"
#include "render/renderer.hpp"
#include "texture_2d.hpp"

/* Called from LevelRenderer to render floor & ceiling */
class FloorsRenderer {
public:
  FloorsRenderer(const TexturesFactory& textures_factory, const Program& program, const glm::vec2& size);
  void set_transform(const Transformation& t);
  void draw(const Uniforms& uniforms);
  void free();

private:
  /* Elevation of ceiling & # of tiles in each direction (x-axis & z-axis resp.) */
  const float m_height = 3.5;
  glm::vec2 m_size;

  Renderer m_renderer;
  Transformation m_transformation;

  /* textures (lifecycle managed by TexturesFactory) */
  Texture2D m_tex_floor_diffuse;
  Texture2D m_tex_floor_normal;
  Texture2D m_tex_ceiling_diffuse;
  Texture2D m_tex_ceiling_normal;

  /* uniforms set in ctor to avoid calling inverseTranspose in each frame */
  const unsigned int m_n_floors = 2;
  std::vector<glm::mat4> m_models_floors;
  std::vector<glm::mat4> m_normals_mats_floors;
  std::vector<Texture2D> m_textures_diffuse;
  std::vector<Texture2D> m_textures_normal;

  void calculate_uniforms();
};

#endif // FLOORS_RENDERER_HPP
