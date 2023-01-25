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

  void draw_floor(const Uniforms& u);
  void draw_ceiling(const Uniforms& u);
  void draw_horizontal_surface(const Uniforms& u, bool is_floor);
};

#endif // FLOORS_RENDERER_HPP
