#include <vertexes/vbo.hpp>

VBO::VBO(const Geometry& geometry, bool is_empty, GLenum type):
  m_vertexes(geometry.get_vertexes()),
  m_n_vertexes(geometry.get_n_vertexes()),
  m_type(type)
{
  // transfer geometry vertexes to bound VBO if not is_empty (else only reserve space)
  const GLvoid* data = (!is_empty) ? m_vertexes.data() : NULL;
  glGenBuffers(1, &m_id);
  bind();
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertexes.size(), data, m_type);
  unbind();
}

std::vector<float> VBO::get_vertexes() const {
  return m_vertexes;
}

int VBO::get_n_vertexes() const {
  // needed by Render::draw()
  return m_n_vertexes;
}

void VBO::update(const Geometry& geometry) {
  // update vbo with vertexes from geometery (used for rendering glyphs)
  m_vertexes = geometry.get_vertexes();
  m_n_vertexes = geometry.get_n_vertexes();
  bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_vertexes.size(), m_vertexes.data());
  unbind();
}

void VBO::bind() {
  glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VBO::unbind() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::free() {
  glDeleteBuffers(1, &m_id);
}
