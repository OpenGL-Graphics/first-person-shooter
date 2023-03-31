#ifndef LEVEL_RENDERER_HPP
#define LEVEL_RENDERER_HPP

#include <unordered_map>

#include "render/renderer.hpp"

#include "levels/walls_renderer.hpp"
#include "levels/floors_renderer.hpp"
#include "levels/doors_renderer.hpp"
#include "levels/trees_renderer.hpp"
#include "levels/windows_renderer.hpp"
#include "levels/targets_renderer.hpp"

#include "levels/tilemap.hpp"
#include "program.hpp"

#include "entries/target_entry.hpp"
#include "entries/wall_orientation.hpp"
#include "entries/wall_entry.hpp"

#include "factories/shaders_factory.hpp"
#include "factories/textures_factory.hpp"

#include "navigation/frustum.hpp"

/**
 * Renderer for level items (e.g. walls, doors...)
 */
struct LevelRenderer {
  /* Used to block camera from going through walls */
  std::vector<glm::vec3> positions_walls;

  LevelRenderer(Assimp::Importer& importer, const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory);
  void draw(const Uniforms& u={});
  void set_transform(const Transformation& t, const Frustum& frustum);
  void free();

private:
  /* Height of walls & elevation of ceiling */
  const float m_height = 3.5;

  /* declared before `m_renderer_floor` as nrows/ncols needed to avoid stretching texture */
  Tilemap m_tilemap;

  /**
   * Renderers for wall, window, ceiling/floor tiles, & trees props
   * Door & floor are both surfaces but with different uv-coords (to avoid stretching texture)
   * with instancing
   */
  DoorsRenderer m_renderer_doors;
  WallsRenderer m_renderer_walls;
  FloorsRenderer m_renderer_floors;
  TreesRenderer m_renderer_trees;
  WindowsRenderer m_renderer_windows;
  TargetsRenderer m_renderer_targets;

  /* position of level */
  glm::vec3 m_position;

  /* positions of tiles elements (parsed only once in constructor) */
  std::vector<glm::vec3> m_positions_doors;
  std::vector<glm::vec3> m_positions_trees;
  std::vector<glm::vec3> m_positions_windows;
  std::vector<WallEntry> m_walls;

  void parse_tilemap();
  void calculate_uniforms();
};

#endif // LEVEL_RENDERER_HPP
