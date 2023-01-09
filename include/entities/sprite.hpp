#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "render/renderer.hpp"

/**
 * 2D sprite having an image as a texture
 */
struct Sprite {
private:
  Texture2D m_texture;

public:
  Renderer renderer;

  Sprite(const Texture2D& texture, const Program& program);
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();
};

#endif // SPRITE_HPP
