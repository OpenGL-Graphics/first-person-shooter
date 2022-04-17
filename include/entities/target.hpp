#ifndef TARGET_HPP
#define TARGET_HPP

#include "render/renderer.hpp"
#include "physics/bounding_box.hpp"

/**
 * Target to destroy on intersection with mouse cursor
 * Multiple instance can have same renderer => use a pointer
 */
struct Target {
private:
  // program declared first (i.e. init first) as it's needed by renderer
  // https://stackoverflow.com/a/41886567/2228912
  Program m_program;
  glm::vec3 m_position;

public:
  Renderer renderer;

  /* for collision detection */
  BoundingBox bounding_box;

  /* kill target on intersection with mouse cursor */
  bool is_dead;

  Target(const glm::vec3& position);
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();
};

#endif // TARGET_HPP
