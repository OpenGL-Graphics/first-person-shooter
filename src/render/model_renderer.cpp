#include <glm/gtc/matrix_transform.hpp>

#include "render/model_renderer.hpp"

// not declared as private members as constants cause class's implicit copy-constructor to be deleted (prevents re-assignment)
// movement constants
const float SPEED = 0.1f;

ModelRenderer::ModelRenderer(const Program& program, const Model& model, const std::vector<Attribute>& attributes):
  m_model(model),
  forward_dir(0.0f, 0.0f, -1.0f)
{
  // one renderer by mesh (to avoid mixing up meshes indices)
  for (const Mesh& mesh : m_model.meshes) {
    VBO vbo_mesh(Geometry(mesh.vertexes, mesh.indices, mesh.positions));
    Renderer renderer(program, vbo_mesh, attributes);
    m_renderers.push_back(renderer);
  }
}

/* Initial transformation (position) of 3D Object accord. to model matrix & calculate its bbox */
void ModelRenderer::set_transform(const glm::mat4& mat_model) {
  for (Renderer& renderer : m_renderers) {
    renderer.set_transform(mat_model);
  }

  calculate_bounding_box();
}

/* Calculate bounding box from bounding boxes of each mesh renderer */
void ModelRenderer::calculate_bounding_box() {
  std::vector<glm::vec3> renderers_bounds;
  for (Renderer& renderer : m_renderers) {
    renderers_bounds.push_back(renderer.bounding_box.min);
    renderers_bounds.push_back(renderer.bounding_box.max);
  }

  bounding_box = BoundingBox(renderers_bounds);
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

/* Move 3D model in either of the four directions accord to given `direction` */
void ModelRenderer::move(Direction direction) {
  glm::vec3 offset;
  glm::vec3 up_dir(0.0f, 1.0f, 0.0f);

  if (direction == Direction::FORWARD) {
    offset = SPEED * forward_dir;
  }
  if (direction == Direction::BACKWARD) {
    offset = SPEED * -forward_dir;
  }
  if (direction == Direction::LEFT) {
    glm::vec3 left_dir = glm::normalize(glm::cross(up_dir, forward_dir));
    offset = SPEED * left_dir;
  }
  if (direction == Direction::RIGHT) {
    glm::vec3 right_dir = glm::normalize(glm::cross(forward_dir, up_dir));
    offset = SPEED * right_dir;
  }

  // move meshes & recalculate model's bounding box
  for (Renderer& renderer : m_renderers) {
    renderer.move(offset);
  }
  calculate_bounding_box();
}
