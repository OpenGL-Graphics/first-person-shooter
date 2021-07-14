#ifndef MODEL_RENDERER_HPP
#define MODEL_RENDERER_HPP

#include <vector>

#include "shaders/program.hpp"
#include "shaders/uniforms.hpp"
#include "vertexes/attribute.hpp"
#include "vertexes/vao.hpp"
#include "vertexes/vbo.hpp"
#include "models/model.hpp"
#include "render/renderer.hpp"
#include "navigation/direction.hpp"

/**
 * Each mesh inside 3D model is rendered separately using `Renderer` class,
 * as vertexes indices start from 0 for all meshes (otherwise they would overwrite)
 */
struct ModelRenderer {
  BoundingBox bounding_box;

  ModelRenderer(const Program& program, const Model& model, const std::vector<Attribute>& attributes);
  void draw(Uniforms& uniforms);
  void free();

  void move(Direction direction);
  void set_transform(const glm::mat4& mat_model);
  void calculate_bounding_box();

private:
  std::vector<Renderer> m_renderers;
  Model m_model;
};

#endif // MODEL_RENDERER_HPP
