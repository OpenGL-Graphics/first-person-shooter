#include <glm/gtc/matrix_transform.hpp>

#include "render/model_renderer.hpp"

// not declared as private members as constants cause class's implicit copy-constructor to be deleted (prevents re-assignment)
// movement constants
const float X_SPEED = 0.1f;
const float Z_SPEED = 0.1f;

ModelRenderer::ModelRenderer(const Program& program, const Model& model, const std::vector<Attribute>& attributes, const glm::vec3& position):
  m_model(model),
  m_position(position)
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

    // 3d position of model
    uniforms["model"] = glm::translate(glm::mat4(1.0f), m_position);

    Renderer renderer = m_renderers[i_renderer];
    renderer.draw(uniforms);
  }
}

void ModelRenderer::free() {
  for (Renderer& renderer : m_renderers) {
    renderer.free();
  }
}

/* Move 3D model in either of the four directions accord to given `direction` */
void ModelRenderer::move(Direction direction) {
  if (direction == Direction::FORWARD)
    m_position -= Z_SPEED * glm::vec3(0.0f, 0.0f, 1.0f);
  if (direction == Direction::BACKWARD)
    m_position += Z_SPEED * glm::vec3(0.0f, 0.0f, 1.0f);
  if (direction == Direction::LEFT)
    m_position -= X_SPEED * glm::vec3(1.0f, 0.0f, 0.0f);
  if (direction == Direction::RIGHT)
    m_position += X_SPEED * glm::vec3(1.0f, 0.0f, 0.0f);
}
