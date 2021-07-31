#ifndef MODEL_RENDERER_HPP
#define MODEL_RENDERER_HPP

#include "models/model.hpp"
#include "render/renderer.hpp"

/**
 * Each mesh inside 3D model is rendered separately using `Renderer` class,
 * as vertexes indices start from 0 for all meshes (otherwise they would overwrite)
 */
struct ModelRenderer {
  ModelRenderer(const Program& program, const Model& model, const std::vector<Attribute>& attributes);
  void draw(Uniforms& uniforms);
  void set_transform(const glm::mat4& mat_model);
  void free();

protected:
  std::vector<Renderer> m_renderers;
private:
  Model m_model;
};

#endif // MODEL_RENDERER_HPP
