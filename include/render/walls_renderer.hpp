#ifndef WALLS_RENDERER_HPP
#define WALLS_RENDERER_HPP

#include <vector>

#include "entries/wall_entry.hpp"
#include "render/renderer.hpp"

/* Called from LevelRenderer to render walls */
class WallsRenderer {
public:
  WallsRenderer();
  void set_transform(const Transformation& t);
  void draw(const std::vector<WallEntry>& entries);
  void free();

private:
  Renderer m_renderer;
  Transformation m_transformation;

  /* Cubes used for walls as face culling hides back-face & lighting affect it similarly to front one */
  Texture3D m_texture;

  /* Height of walls & elevation of ceiling */
  const float m_height = 3.5;
  const float m_wall_thickness = 0.2;
  const float m_wall_tile_length = 1.0;

  void draw_wall(const WallEntry& wall);
  std::array<glm::vec3, 2> calculate_offsets(const WallEntry& entry);
  std::array<float, 2> calculate_angles(const WallEntry& entry);
};

#endif // WALLS_RENDERER_HPP
