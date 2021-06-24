#include <render/renderer.hpp>

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

void Renderer::draw(const Uniforms& uniforms) {
  m_vao.bind();
  m_program.use();
  unsigned int n_vertexes = m_vbo.get_n_vertexes();

  // pass shaders uniforms & draw attributes in bound VAO (using EBO vertexes indices)
  m_program.set_uniforms(uniforms);
  glDrawElements(GL_TRIANGLES, n_vertexes, GL_UNSIGNED_INT, 0);
  // glDrawArrays(GL_TRIANGLES, 0, m_vbo.get_n_vertexes());

  m_vao.unbind();
  m_program.unuse();
}

void Renderer::free() {
  // VAO & VBO lifecycles managed by renderer
  m_vao.free();
  m_vbo.free();
}
