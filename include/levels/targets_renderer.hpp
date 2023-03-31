#ifndef TARGETS_RENDERER_HPP
#define TARGETS_RENDERER_HPP

#include "factories/shaders_factory.hpp"
#include "entries/target_entry.hpp"
#include "render/model_renderer.hpp"
#include "physics/bounding_box.hpp"
#include "navigation/frustum.hpp"

/* Target to destroy on intersection with mouse cursor */
class TargetsRenderer {
public:
  TargetsRenderer(const ShadersFactory& shaders_factory, Assimp::Importer& importer);
  void calculate_uniforms();
  void set_transform(const Transformation& t, const Frustum& frustum);
  void draw(const Uniforms& uniforms);
  void free();

private:
  /* Bounding box in local space (needed by LevelRenderer) */
  BoundingBox m_bounding_box;

  /* delegate drawing to model renderer */
  assimp_utils::Model m_model3d;
  ModelRenderer m_renderer;

  /* Uniform matrices for all targets (dead & alive) */
  std::vector<glm::mat4> m_models;
  std::vector<glm::mat4> m_normals_mats;

  void calculate_bounding_box();
  std::vector<glm::mat4> get_uniform_mats(const std::string& name, const Frustum& frustum);
};

#endif // TARGETS_RENDERER_HPP
