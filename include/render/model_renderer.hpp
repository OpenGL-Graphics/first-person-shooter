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

/**
 * Each mesh inside 3D model is rendered separately using `Renderer` class,
 * as vertexes indices start from 0 for all meshes (otherwise they would overwrite)
 */
class ModelRenderer {
public:
  ModelRenderer(const Program& program, const Model& model, const std::vector<Attribute>& attributes);
  void draw(const Uniforms& uniforms);
  void free();

private:
  std::vector<Renderer> m_renderers;
};

#endif // MODEL_RENDERER_HPP
