#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "render/renderer.hpp"

Renderer::Renderer(const Program& program, const VBO& vbo, const std::vector<Attribute>& attributes):
  m_vao(),
  m_vbo(vbo),
  m_program(program)
{
  // create vertex attributes linking bound VAO and VBO (& EBO with it)
  m_vao.bind();
  m_vbo.bind();

  for (Attribute attribute : attributes) {
    attribute.id = m_program.define_attribute(attribute.name);
    m_vao.set_attribute(attribute);
  }

  m_vao.unbind();
  m_vbo.unbind();
}

/**
 * Sets initial transformation matrix for model (translation, rotation, scaling)
 * glm::mat4 used as certain objects in scene require a scaling (besides translation)
 * @param mat_model Model matrix (i.e. transformation matrix)
 */
void Renderer::set_transform(const glm::mat4& mat_model) {
  m_mat_model = mat_model;

  // calculate bounding box from positions in local coords in vbo
  // then update bounding box in world coords from model matrix (avoids incremental translation)
  bounding_box = BoundingBox(m_vbo.positions);
  bounding_box.transform(mat_model);
}

/**
 * Translate from current position (model matrix) by given offset
 * Note: bounding box is in world coords while vertexes are in local coords,
 *       hense the different transformation matrices.
 */
void Renderer::move(const glm::vec3& offset) {
  // translation vector at 4th column of transformation matrix (i.e. model matrix)
  // vertexes in local coords, hence new_position = old_position + offset
  glm::vec3 position = m_mat_model[3];
  position += offset;
  m_mat_model = glm::translate(glm::mat4(1.0f), position);

  // translate bbox instead of reclalculating it each time (like `set_transform()`)
  // update bounding box in world coords using offset
  bounding_box.transform(glm::translate(glm::mat4(1.0f), offset));
}

/**
 * Draw vertexes given to `m_vbo`
 * @param Uniforms Unordered map (key, values) of vars to pass to shader
 * @param mode GL_TRIANGLES for most meshes, GL_TRIANGLE_STRIP for terrains
 */
void Renderer::draw(Uniforms& uniforms, GLenum mode) {
  // 3d position of model
  uniforms["model"] = m_mat_model;

  // wireframe mode
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  m_vao.bind();
  m_program.use();

  // pass shaders uniforms & draw attributes in bound VAO (using EBO vertexes indices)
  m_program.set_uniforms(uniforms);
  glDrawElements(mode, m_vbo.n_elements, GL_UNSIGNED_INT, 0);
  // glDrawArrays(GL_TRIANGLES, 0, m_vbo.n_vertexes);

  m_vao.unbind();
  m_program.unuse();
}

/* Free used vertex buffers (VAO & VBO lifecycles managed by renderer) */
void Renderer::free() {
  m_vao.free();
  m_vbo.free();
}
