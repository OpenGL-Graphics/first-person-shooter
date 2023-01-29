#ifndef TARGETS_RENDERER_HPP
#define TARGETS_RENDERER_HPP

#include "factories/shaders_factory.hpp"
#include "entries/target_entry.hpp"
#include "render/model_renderer.hpp"
#include "physics/bounding_box.hpp"

/* Target to destroy on intersection with mouse cursor */
class TargetsRenderer {
public:
  TargetsRenderer(const ShadersFactory& shaders_factory, Assimp::Importer& importer);
  void calculate_uniforms();
  void set_transform(const Transformation& t);
  void draw(const Uniforms& uniforms);
  void free();

private:
  /* Bounding box in local space (needed by LevelRenderer) */
  BoundingBox m_bounding_box;

  /* delegate drawing to model renderer */
  AssimpUtil::Model m_model3d;
  ModelRenderer m_renderer;

  /* Uniform matrices for all targets (dead & alive) */
  std::vector<glm::mat4> m_models;
  std::vector<glm::mat4> m_normals_mats;

  void calculate_bounding_box();
  std::vector<glm::mat4> get_uniform_mats(const std::string& name);
};

#endif // TARGETS_RENDERER_HPP
