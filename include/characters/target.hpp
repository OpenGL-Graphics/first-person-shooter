#ifndef TARGET_HPP
#define TARGET_HPP

#include "render/renderer.hpp"

/**
 * Target to destroy on intersection with mouse cursor
 * Multiple instance can have same renderer => use a pointer
 */
struct Target {
/* kill target on intersection with mouse cursor */
  Renderer renderer;
  bool is_dead;

  Target(const Program& program);
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();
};

#endif // TARGET_HPP
