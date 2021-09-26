#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "render/renderer.hpp"

/**
 * 2D sprite having an image as a texture
 */
struct Sprite {
private:
  // program declared first (i.e. init first) as it's needed by renderer
  // https://stackoverflow.com/a/41886567/2228912
  Program m_program;
  Texture2D m_texture;

public:
  Renderer renderer;

  Sprite(const Texture2D& texture);
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();
};

#endif // SPRITE_HPP
