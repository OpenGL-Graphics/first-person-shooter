#ifndef LEVEL_RENDERER_HPP
#define LEVEL_RENDERER_HPP

#include <unordered_map>

#include "render/renderer.hpp"
#include "levels/tilemap.hpp"
#include "program.hpp"
#include "entries/target_entry.hpp"

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
  void draw_floor(const Uniforms& u);
  void draw_ceiling(const Uniforms& u);
  void draw_targets(const Uniforms& u);
  void set_transform(const Transformation& t);
  void free();

protected:
  std::vector<Renderer> m_renderers;
private:
  /* Height of walls & elevation of ceiling */
  const float m_height;

  /* declared before `m_renderer_floor` as nrows/ncols needed to avoid stretching texture */
  Tilemap m_tilemap;

  /**
   * One renderer for walls tiles and another one for ceiling/floor
   * wall tiles & ceiling/floor have different sizes => different uv-coords to avoid stretching texture
   */
  Renderer m_renderer_wall;
  Renderer m_renderer_floor;

  /* tree props rendered multiple times */
  Model m_tree;

  /* Window & wall below/above it (different uv from `m_renderer_wall`) */
  Sprite m_window;
  Renderer m_renderer_wall_half;

  /* Target (enemy) to shoot */
  Target m_target;

  /* transformation matrixes (view, projection) passed to tiles renderers before `draw()` */
  Transformation m_transformation;

  /* position of level */
  glm::vec3 m_position;
  std::unordered_map<std::string, Texture2D> m_textures;

  void draw_horizontal_surface(const Uniforms& u, bool is_floor);
  void draw_window(const Uniforms& u, const glm::vec3& position_tile);

  glm::mat4 get_model_target(const glm::vec3& position_target);
};

#endif // LEVEL_RENDERER_HPP
