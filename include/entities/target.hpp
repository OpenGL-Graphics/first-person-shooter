#ifndef TARGET_HPP
#define TARGET_HPP

#include "render/model_renderer.hpp"
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
  Transformation m_transformation;

  /* delegate drawing to model renderer */
  AssimpUtil::Model m_model;
  ModelRenderer m_renderer;

  void calculate_bounding_box();

public:
  /* for collision detection */
  BoundingBox bounding_box;

  /* used to calculate normal mat in `LevelRenderer` */
  glm::vec3 position;

  /* kill target on intersection with mouse cursor */
  bool is_dead;

  Target(Assimp::Importer& importer, const glm::vec3& pos);
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();
};

#endif // TARGET_HPP
