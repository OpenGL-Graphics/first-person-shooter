#ifndef LEVEL_RENDERER_HPP
#define LEVEL_RENDERER_HPP

#include <unordered_map>

#include "render/renderer.hpp"
#include "levels/tilemap.hpp"
#include "shaders/program.hpp"
#include "entities/target.hpp"
#include "entities/sprite.hpp"

/**
 * Renderer for level items (e.g. walls, doors...)
 */
struct LevelRenderer {
  /* Used to block camera from going through walls */
  std::vector<glm::vec3> positions_walls;
  /* Targets to kill by mouse cursor intersection */
  // std::vector<Target> targets;

  LevelRenderer(const Program& program_tile, const Tilemap& tilemap, const glm::vec3& position);
  void draw(const Uniforms& u={});
  void draw_floor(const Uniforms& u);
  void draw_ceiling(const Uniforms& u);
  void set_transform(const Transformation& t);
  void free();

protected:
  std::vector<Renderer> m_renderers;
private:
  /* same renderer for all ceiling/floor & walls */
  Renderer m_renderer;
  /* same target rendered multiple times */
  Target m_target;
  /* same grass prop rendered multiple times */
  Sprite m_grass;

  /* position of level */
  glm::vec3 m_position;
  /* Height of walls & elevation of ceiling */
  float m_height;
  Tilemap m_tilemap;
  std::unordered_map<std::string, Texture2D> m_textures;

  void draw_horizontal_surface(const Uniforms& u, const glm::vec2& size, float height);
};

#endif // LEVEL_RENDERER_HPP
