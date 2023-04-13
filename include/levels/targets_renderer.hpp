#ifndef TARGETS_RENDERER_HPP
#define TARGETS_RENDERER_HPP

#include "factories/shaders_factory.hpp"
#include "entries/target_entry.hpp"
#include "render/model_renderer.hpp"
#include "math/bounding_box.hpp"
#include "navigation/frustum.hpp"

/* Target to destroy on intersection with mouse cursor */
class TargetsRenderer {
public:
  TargetsRenderer(const ShadersFactory& shaders_factory, Assimp::Importer& importer);
  void calculate_bboxes();
  void calculate_uniforms();
  void set_transform(const Transformation& t, const Frustum& frustum);
  void draw(const Uniforms& uniforms);
  void free();

private:
  /* delegate drawing to model renderer */
  assimp_utils::Model m_model3d;
  ModelRenderer m_renderer;

  /* Bounding box in local space */
  BoundingBox m_bounding_box;

  /* Uniform matrices for all targets (dead & alive) */
  std::vector<glm::mat4> m_models;
  std::vector<glm::mat4> m_normals_mats;

  std::vector<glm::mat4> cull_dead(const std::vector<glm::mat4>& matrices, const std::vector<TargetEntry> targets);
};

#endif // TARGETS_RENDERER_HPP
