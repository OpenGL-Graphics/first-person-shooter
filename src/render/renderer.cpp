#include <render/renderer.hpp>

Renderer::Renderer(const Program& program, const VBO& vbo, const std::vector<Attribute>& attributes):
  m_program(program),
  m_vbo(vbo),
  m_vao()
{
  // create vertex attributes linking bound VAO & VBO
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

  // pass shaders uniforms & draw attributes in bound VAO
  m_program.set_uniforms(uniforms);
  glDrawArrays(GL_TRIANGLES, 0, m_vbo.get_n_vertexes());

  m_vao.unbind();
  m_program.unuse();
}

void Renderer::free() {
  // VAO & VBO lifecycles managed by renderer
  m_vao.free();
  m_vbo.free();
}
