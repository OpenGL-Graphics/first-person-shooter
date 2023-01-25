#include <glm/gtc/matrix_transform.hpp>

#include "render/model_renderer.hpp"

// not declared as private members as constants cause class's implicit copy-constructor to be deleted (prevents re-assignment)
// movement constants
const float SPEED = 0.1f;

ModelRenderer::ModelRenderer(const Program& program, const AssimpUtil::Model& model, const std::vector<Attribute>& attributes):
  m_model(model)
{
  // one renderer by mesh (to avoid mixing up meshes indices)
  for (const AssimpUtil::Mesh& mesh : m_model.meshes) {
    Renderer renderer(program, Geometry(mesh.vertexes, mesh.indices, mesh.positions), attributes);
    renderers.push_back(renderer);
  }
}

/**
 * Initial transformation (position) of 3D Object accord. to model matrix
 * as well view & projection matrixes
 */
void ModelRenderer::set_transform(const Transformation& transformation) {
  for (Renderer& renderer : renderers) {
    renderer.set_transform(transformation);
  }
}

/* Needed to pass normal_mat to tree's shaders in LevelRenderer */
void ModelRenderer::set_uniform_arr(const std::string& name, const std::vector<glm::mat4>& u) {
  for (Renderer& renderer : renderers)
    renderer.set_uniform_arr(name, u);
}

/* Rendering of model relies on `Renderer::draw() applied to each mesh */
void ModelRenderer::draw(const Uniforms& u, bool with_outlines) {
  Uniforms uniforms = u;

  for (size_t i_renderer = 0; i_renderer < renderers.size(); ++i_renderer) {
    // retrieve materials/textures from mesh (get a ref. to avoid copying vec. members)
    AssimpUtil::Mesh& mesh = m_model.meshes[i_renderer];
    mesh.set_uniforms(uniforms);
    Renderer renderer = renderers[i_renderer];

    if (with_outlines) {
      renderer.draw_with_outlines(uniforms);
    } else {
      renderer.draw(uniforms);
    }
  }
}

/* Free loaded textures & vbo/vao buffers */
void ModelRenderer::free() {
  m_model.free();

  for (Renderer& renderer : renderers) {
    renderer.free();
  }
}
