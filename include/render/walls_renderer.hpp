#ifndef WALLS_RENDERER_HPP
#define WALLS_RENDERER_HPP

#include <vector>

#include "entries/wall_entry.hpp"
#include "render/renderer.hpp"
#include "factories/shaders_factory.hpp"
#include "factories/textures_factory.hpp"

/* Called from LevelRenderer to render walls */
class WallsRenderer {
public:
  WallsRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory);
  void set_transform(const Transformation& t);
  void calculate_uniforms(const std::vector<WallEntry>& entries, const std::vector<glm::vec3>& positions_windows);
  void draw();
  void draw_walls_around_window();
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

  /* Texture3D was stretching without repeat */
  Texture2D m_texture;

  std::vector<glm::mat4> m_models;
  std::vector<glm::mat4> m_models_around_windows;

  std::array<glm::vec3, 2> calculate_offsets(const WallEntry& entry);
  std::array<float, 2> calculate_angles(const WallEntry& entry);
  void calculate_uniforms_full(const std::vector<WallEntry>& entries);
  void calculate_uniforms_around_window(const std::vector<glm::vec3>& positions_windows);
};

#endif // WALLS_RENDERER_HPP
