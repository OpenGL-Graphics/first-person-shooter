#ifndef TARGET_HPP
#define TARGET_HPP

#include "render/model_renderer.hpp"
#include "physics/bounding_box.hpp"

/**
 * Target to destroy on intersection with mouse cursor
 * Multiple instance can have same renderer => use a pointer
 */
struct Target {
  /* kill target on intersection with mouse cursor */
  bool is_dead;

  /* Bounding box in local space (needed by LevelRenderer) */
  BoundingBox bounding_box;

  Target(Assimp::Importer& importer, const Program& program);
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();

private:
  Transformation m_transformation;

  /* delegate drawing to model renderer */
  AssimpUtil::Model m_model;
  ModelRenderer m_renderer;

  void calculate_bounding_box();
};

#endif // TARGET_HPP
