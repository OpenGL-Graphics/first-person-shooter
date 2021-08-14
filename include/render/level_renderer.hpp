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
  void free();

protected:
  std::vector<Renderer> m_renderers;
private:
  /* same renderer for all tiles */
  Renderer m_renderer;
  Tilemap m_tilemap;
  std::unordered_map<std::string, Texture2D> m_textures;
};

#endif // LEVEL_RENDERER_HPP
