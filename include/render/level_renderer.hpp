#ifndef LEVEL_RENDERER_HPP
#define LEVEL_RENDERER_HPP

#include <unordered_map>

#include "render/renderer.hpp"
#include "levels/tilemap.hpp"
#include "shaders/program.hpp"

/**
 * Renderer for level items (e.g. walls, doors...)
 */
struct LevelRenderer {
  LevelRenderer(const Program& program, const Tilemap& tilemap);
  void draw(Uniforms& uniforms);
  void draw_floor(Uniforms& uniforms);
  void draw_ceiling(Uniforms& uniforms);
  void set_transform(const Transformation& t);
  void free();

protected:
  std::vector<Renderer> m_renderers;
private:
  /* same renderer for all tiles */
  Renderer m_renderer;
  /* position of level */
  glm::vec3 m_position;
  /* Height of walls & elevation of ceiling */
  float m_height;
  Tilemap m_tilemap;
  std::unordered_map<std::string, Texture2D> m_textures;

  void draw_horizontal_surface(Uniforms& uniforms, const glm::vec2& size, float height);
};

#endif // LEVEL_RENDERER_HPP
