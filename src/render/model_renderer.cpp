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

/* Rendering of model relies on `Renderer::draw() applied to each mesh */
void ModelRenderer::draw(const Uniforms& u, bool with_outlines) {
  Uniforms uniforms = u;

  for (size_t i_renderer = 0; i_renderer < renderers.size(); ++i_renderer) {
    // retrieve material color from mesh
    AssimpUtil::Mesh mesh = m_model.meshes[i_renderer];
    uniforms["has_texture_diffuse"] = mesh.has_texture_diffuse;
    uniforms["has_texture_normal"] = mesh.has_texture_normal;
    uniforms["color"] = mesh.color;

    // no need to pass empty texture created (in `Mesh`) by default constructor
    if (mesh.has_texture_diffuse)
      uniforms["texture_diffuse"] = mesh.texture_diffuse;

    if (mesh.has_texture_normal)
      uniforms["texture_normal"] = mesh.texture_normal;

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
