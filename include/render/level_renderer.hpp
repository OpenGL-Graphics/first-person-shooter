#ifndef LEVEL_RENDERER_HPP
#define LEVEL_RENDERER_HPP

#include <unordered_map>

#include "render/renderer.hpp"
#include "render/walls_renderer.hpp"
#include "render/floors_renderer.hpp"

#include "levels/tilemap.hpp"
#include "program.hpp"

#include "entries/target_entry.hpp"
#include "entries/wall_orientation.hpp"
#include "entries/wall_entry.hpp"

#include "entities/target.hpp"
#include "entities/model.hpp"
#include "entities/sprite.hpp"

/**
 * Renderer for level items (e.g. walls, doors...)
 */
struct LevelRenderer {
  /* Used to block camera from going through walls */
  std::vector<glm::vec3> positions_walls;

  /**
   * Used to check for intersec. with mouse in `MouseHandler`
   * TODO: externalize in `globals/`
   */
  static std::vector<TargetEntry> targets;

  LevelRenderer(const Program& program_tile, const Tilemap& tilemap, const glm::vec3& position, Assimp::Importer& importer);
  void draw(const Uniforms& u={});
  void set_transform(const Transformation& t);
  void free();

protected:
  std::vector<Renderer> m_renderers;
private:
  /* Height of walls & elevation of ceiling */
  const float m_height = 3.5;

  /* declared before `m_renderer_floor` as nrows/ncols needed to avoid stretching texture */
  Tilemap m_tilemap;

  /**
   * Renderers for wall, window, & ceiling/floor tiles
   * Door & floor are both surfaces but with different uv-coords (to avoid stretching texture)
   */
  Renderer m_renderer_door;
  WallsRenderer m_renderer_walls;
  FloorsRenderer m_renderer_floors;

  /* tree props rendered multiple times */
  Model m_tree;

  /* Window & wall below/above it */
  Sprite m_window;
  Renderer m_renderer_wall_half;

  /* Target (enemy) to shoot */
  Target m_target;

  /* transformation matrixes (view, projection) passed to tiles renderers before `draw()` */
  Transformation m_transformation;

  /* position of level */
  glm::vec3 m_position;

  /* textures */
  std::unordered_map<std::string, Texture2D> m_textures;

  /* positions of tiles elements (parsed only once in constructor) */
  std::vector<glm::vec3> m_positions_doors;
  std::vector<glm::vec3> m_positions_trees;
  std::vector<glm::vec3> m_positions_windows;
  std::vector<WallEntry> m_walls;

  void draw_targets(const Uniforms& u);
  void draw_windows(const Uniforms& u);
  void draw_window(const Uniforms& u, const glm::vec3& position_tile);
  void draw_trees(const Uniforms& u);
  void draw_doors(const Uniforms& u);

  glm::mat4 get_model_target(const glm::vec3& position_target);
};

#endif // LEVEL_RENDERER_HPP
