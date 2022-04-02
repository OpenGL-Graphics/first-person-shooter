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

  for (const Attribute& attribute : attributes) {
    m_vao.set_attribute(attribute);
  }

  m_vao.unbind();
  m_vbo.unbind();
}

/**
 * Sets initial transformation matrix for model (translation, rotation, scaling)
 * glm::mat4 used as certain objects in scene require a scaling (besides translation)
 * @param model Model matrix (i.e. transformation matrix)
 */
void Renderer::set_transform(const Transformation& t) {
  transformation = t;
  model_mat = transformation.model;

  // calculate bounding box from positions in local coords in vbo
  // then update bounding box in world coords from model matrix (avoids incremental translation)
  bounding_box = BoundingBox(m_vbo.positions);
  bounding_box.transform(model_mat);
}

/**
 * Translate from current position (model matrix) by given offset
 * Note: bounding box is in world coords while vertexes are in local coords,
 *       hence the different transformation matrices.
 */
void Renderer::move(const glm::vec3& offset) {
  // translation vector at 4th column of transformation matrix (i.e. model matrix)
  // vertexes in local coords, hence new_position = old_position + offset
  glm::vec3 position = model_mat[3];
  position += offset;
  model_mat = glm::translate(glm::mat4(1.0f), position);

  // translate bbox instead of reclalculating it each time (like `set_transform()`)
  // update bounding box in world coords using offset
  bounding_box.transform(glm::translate(glm::mat4(1.0f), offset));
}

/**
 * Draw vertexes given to `m_vbo`
 * @param Uniforms Unordered map (key, values) of vars to pass to shader
 *        Passed as const ref. as we cannot have default param values with refs.
 * @param mode GL_TRIANGLES for most meshes, GL_TRIANGLE_STRIP for grids (i.e. terrain & plane)
 *             GL_LINES to draw a line between each pair of successive vertexes
 * @param n_elements # of elements (i.e. indices) to draw (used only by `geometry/gizmo.cpp`)
 * @param offset Indice to start rendering from in EBO (used only by `geometry/gizmo.cpp`)
 */
void Renderer::draw(const Uniforms& u, GLenum mode, unsigned int count, size_t offset) {
  // 3d position of model
  Uniforms uniforms = u;
  uniforms["model"] = transformation.model;
  uniforms["view"] = transformation.view;
  uniforms["projection"] = transformation.projection;

  // wireframe mode
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  m_vao.bind();
  m_program.use();

  // pass shaders uniforms & draw attributes in bound VAO (using EBO vertexes indices)
  // offset given to `glDrawElements()` in bytes
  m_program.set_uniforms(uniforms);
  unsigned int n_elements = (count == 0) ? m_vbo.n_elements : count;
  glDrawElements(mode, n_elements, GL_UNSIGNED_INT, (GLvoid *) (offset * sizeof(GLuint)));

  m_vao.unbind();
  m_program.unuse();
}

/**
 * Draw vertexes with outlines using stencil buffer & multipass drawing
 * @param Uniforms Unordered map (key, values) of vars to pass to shader
 */
void Renderer::draw_with_outlines(const Uniforms& u) {
  // 1st render pass: always pass the stencil test & set ref=1 in stencil buffer for drawn fragments (pixels)
  glStencilFunc(GL_ALWAYS, 1, 0xff);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  Uniforms uniforms = u;
  draw(uniforms);

  // 2nd render pass: only fragments that weren't assigned 1s in previous step are rendered (pass the test)
  // white outlines & scaling
  glStencilFunc(GL_NOTEQUAL, 1, 0xff);
  // set_transform(glm::scale(model_mat, glm::vec3(1.1f, 1.1f, 1.1f)));
  set_transform({
    glm::scale(model_mat, glm::vec3(1.1f, 1.1f, 1.1f)),
    transformation.view,
    transformation.projection
  });
  uniforms["color"] = glm::vec3(1.0f, 1.0f, 1.0f);
  draw(uniforms);

  // reset stencil test to always pass (for further rendering in same frame)
  glStencilFunc(GL_ALWAYS, 1, 0xff);
}

/* Free used vertex buffers (VAO & VBO lifecycles managed by renderer) & shader program */
void Renderer::free() {
  m_vao.free();
  m_vbo.free();
}
