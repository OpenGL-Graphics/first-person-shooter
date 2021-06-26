#include <render/model_renderer.hpp>

ModelRenderer::ModelRenderer(const Program& program, const Model& model, const std::vector<Attribute>& attributes) {
  for (const Mesh& mesh : model.meshes) {
    VBO vbo_mesh(Geometry(mesh.vertexes, mesh.indices));
    m_renderers.push_back(Renderer(program, vbo_mesh, attributes));
  }
}

void ModelRenderer::draw(const Uniforms& uniforms) {
  for (Renderer& renderer : m_renderers) {
    renderer.draw(uniforms);
  }
}

void ModelRenderer::free() {
  for (Renderer& renderer : m_renderers) {
    renderer.free();
  }
}
