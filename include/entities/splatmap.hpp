#ifndef SPLATMAP_HPP
#define SPLATMAP_HPP

#include "render/renderer.hpp"

/**
 * Encapsulates renderer for terrain geometry & its textures (splatmap)
 */
class Splatmap {
public:
  Splatmap();
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();

private:
  /* heightmap image (needs to be declared/init before vbo) */
  Image m_image;

  // program declared first (i.e. init first) as it's needed by renderer
  // https://stackoverflow.com/a/41886567/2228912
  Program m_program;
  VBO m_vbo;
  Renderer m_renderer;

  // terrain textures (used by same shader) need to be attached to different texture units
  Texture2D m_texture_terrain_water;
  Texture2D m_texture_terrain_grass;
  Texture2D m_texture_terrain_rock;
  Texture2D m_texture_terrain_splatmap;
};

#endif // SPLATMAP_HPP
