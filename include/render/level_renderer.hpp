#ifndef LEVEL_RENDERER_HPP
#define LEVEL_RENDERER_HPP

#include <unordered_map>

#include "render/renderer.hpp"
#include "levels/tilemap.hpp"
#include "program.hpp"
#include "entities/target.hpp"
#include "entities/model.hpp"

/**
 * Renderer for level items (e.g. walls, doors...)
 */
struct LevelRenderer {
  /* Used to block camera from going through walls */
  std::vector<glm::vec3> positions_walls;

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

  /* same grass prop rendered multiple times */
  Model m_tree;

  /* transformation matrixes (view, projection) passed to tiles renderers before `draw()` */
  Transformation m_transformation;

  /* position of level */
  glm::vec3 m_position;
  std::unordered_map<std::string, Texture2D> m_textures;

  void draw_horizontal_surface(const Uniforms& u, float height);
};

#endif // LEVEL_RENDERER_HPP
