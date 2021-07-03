#include "render/model_renderer.hpp"

ModelRenderer::ModelRenderer(const Program& program, const Model& model, const std::vector<Attribute>& attributes):
  m_model(model)
{
  // one renderer by mesh (to avoid mixing up meshes indices)
  for (const Mesh& mesh : m_model.meshes) {
    VBO vbo_mesh(Geometry(mesh.vertexes, mesh.indices));
    m_renderers.push_back(Renderer(program, vbo_mesh, attributes));
  }
}

void ModelRenderer::draw(Uniforms& uniforms) {
  for (size_t i_renderer = 0; i_renderer < m_renderers.size(); ++i_renderer) {
    // retrieve material color from mesh
    Mesh mesh = m_model.meshes[i_renderer];
    uniforms["color"] = mesh.color;
    uniforms["texture2d"] = mesh.texture;

    Renderer renderer = m_renderers[i_renderer];
    renderer.draw(uniforms);
  }
}

void ModelRenderer::free() {
  for (Renderer& renderer : m_renderers) {
    renderer.free();
  }
}
