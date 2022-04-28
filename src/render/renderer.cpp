#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "render/renderer.hpp"

Renderer::Renderer(const Program& program, const VBO& vertex_buffer, const std::vector<Attribute>& attributes):
  m_vao(),
  vbo(vertex_buffer),
  m_program(program)
{
  // create vertex attributes linking bound VAO and VBO (& EBO with it)
  m_vao.bind();
  vbo.bind();

  for (const Attribute& attribute : attributes) {
    m_vao.set_attribute(attribute);
  }

  m_vao.unbind();
  vbo.unbind();
}

/**
 * Sets initial transformation matrix for model (translation, rotation, scaling)
 * glm::mat4 used as certain objects in scene require a scaling (besides translation)
 */
void Renderer::set_transform(const Transformation& t) {
  m_transformation = t;
}

/**
 * Draw vertexes given to `vbo`
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
  uniforms["model"] = m_transformation.model;
  uniforms["view"] = m_transformation.view;
  uniforms["projection"] = m_transformation.projection;

  // wireframe mode
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  m_vao.bind();
  m_program.use();

  // pass shaders uniforms & draw attributes in bound VAO (using EBO vertexes indices)
  // offset given to `glDrawElements()` in bytes
  m_program.set_uniforms(uniforms);
  unsigned int n_elements = (count == 0) ? vbo.n_elements : count;
  glDrawElements(mode, n_elements, GL_UNSIGNED_INT, (GLvoid *) (offset * sizeof(GLuint)));

  m_vao.unbind();
  m_program.unuse();
}

/**
 * Draw vertexes with outlines using stencil buffer & multipass drawing
 * @param Uniforms Unordered map (key, values) of vars to pass to shader
 */
void Renderer::draw_with_outlines(const Uniforms& u) {
  // clear stencil mask (so drawing doesn't have to happen at very beginning of main loop)
  glClear(GL_STENCIL_BUFFER_BIT);

  // 1st render pass: always pass the stencil test & set ref=1 in stencil buffer for drawn fragments (pixels)
  glStencilFunc(GL_ALWAYS, 1, 0xff);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  Uniforms uniforms = u;
  draw(uniforms);

  // 2nd render pass: only fragments that weren't assigned 1s in previous step are rendered (pass the test)
  // white outlines & scaling
  glStencilFunc(GL_NOTEQUAL, 1, 0xff);
  set_transform({
    glm::scale(m_transformation.model, glm::vec3(1.1f, 1.1f, 1.1f)),
    m_transformation.view,
    m_transformation.projection
  });
  uniforms["color"] = glm::vec3(1.0f, 1.0f, 1.0f);
  draw(uniforms);

  // reset stencil test to always pass (for further rendering in same frame)
  glStencilFunc(GL_ALWAYS, 1, 0xff);
}

/* Free used vertex buffers (VAO & VBO lifecycles managed by renderer) & shader program */
void Renderer::free() {
  m_vao.free();
  vbo.free();
}
