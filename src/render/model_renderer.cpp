#include <glm/gtc/matrix_transform.hpp>

#include "render/model_renderer.hpp"

// not declared as private members as constants cause class's implicit copy-constructor to be deleted (prevents re-assignment)
// movement constants
const float SPEED = 0.1f;

ModelRenderer::ModelRenderer(const Program& program, const Model& model, const std::vector<Attribute>& attributes):
  m_model(model)
{
  // one renderer by mesh (to avoid mixing up meshes indices)
  for (const Mesh& mesh : m_model.meshes) {
    VBO vbo_mesh(Geometry(mesh.vertexes, mesh.indices, mesh.positions));
    Renderer renderer(program, vbo_mesh, attributes);
    m_renderers.push_back(renderer);
  }
}

/* Initial transformation (position) of 3D Object accord. to model matrix */
void ModelRenderer::set_transform(const glm::mat4& mat_model) {
  for (Renderer& renderer : m_renderers) {
    renderer.set_transform(mat_model);
  }
}

/* Rendering of model relies on `Renderer::draw() applied to each mesh */
void ModelRenderer::draw(Uniforms& uniforms, bool with_outlines) {
  for (size_t i_renderer = 0; i_renderer < m_renderers.size(); ++i_renderer) {
    // retrieve material color from mesh
    Mesh mesh = m_model.meshes[i_renderer];
    uniforms["color"] = mesh.color;
    uniforms["texture2d"] = mesh.texture;
    Renderer renderer = m_renderers[i_renderer];

    if (with_outlines) {
      renderer.draw_with_outlines(uniforms);
    } else {
      renderer.draw(uniforms);
    }
  }
}

void ModelRenderer::free() {
  for (Renderer& renderer : m_renderers) {
    renderer.free();
  }
}
