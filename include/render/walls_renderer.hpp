#ifndef WALLS_RENDERER_HPP
#define WALLS_RENDERER_HPP

#include <vector>

#include "entries/wall_entry.hpp"
#include "render/renderer.hpp"
#include "factories/textures_factory.hpp"

/* Called from LevelRenderer to render walls */
class WallsRenderer {
public:
  WallsRenderer(const TexturesFactory& textures_factory, const Program& program);
  void set_transform(const Transformation& t);
  void draw(const std::vector<WallEntry>& entries);
  void draw_walls_around_window(const glm::vec3& position_tile);
  void free();

private:
  /* Height of walls & elevation of ceiling */
  const float m_wall_height = 3.5;
  const float m_wall_length = 1.0;
  const float m_wall_depth = 0.2;

  /* Height of wall below/above window */
  const float m_window_height = 1.0f;
  const float m_subwall_height = (m_wall_height - m_window_height) / 2;

  /* TODO: same renderer (shader, vao attributes) but with updated vbo */
  Renderer m_renderer;
  Renderer m_renderer_subwall;
  Transformation m_transformation;

  /* Texture3D was stretching without repeat */
  Texture2D m_texture;

  void draw_wall(const WallEntry& wall);
  std::array<glm::vec3, 2> calculate_offsets(const WallEntry& entry);
  std::array<float, 2> calculate_angles(const WallEntry& entry);
};

#endif // WALLS_RENDERER_HPP
