#ifndef GRASS_HPP
#define GRASS_HPP

#include "render/renderer.hpp"

/**
 * Target to destroy on intersection with mouse cursor
 * Multiple instance can have same renderer => use a pointer
 */
struct Grass {
// program declared first (i.e. init first) as it's needed by renderer
// https://stackoverflow.com/a/41886567/2228912
private:
  Program m_program;

public:
  Texture2D texture;  // passed in uniform inside `LevelRenderer`
  Renderer renderer;

  Grass();
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();
};

#endif // GRASS_HPP
