#include <vertexes/vbo.hpp>

/* No polymorphism if `geometry` wasn't passed by ref */
VBO::VBO(const Geometry& geometry, bool is_empty, GLenum type):
  m_type(type),
  m_vertexes(geometry.get_vertexes()),
  m_indices(geometry.get_indices()),

  n_elements(geometry.get_n_elements()),
  positions(geometry.get_positions())
{
  generate();
  bind();

  // transfer geometry vertexes to bound VBO if not is_empty (else only reserve space)
  const GLvoid* data = (!is_empty) ? m_vertexes.data() : NULL;
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertexes.size(), data, m_type);

  // transfer vertexes indices if geometry has any
  if (!m_indices.empty()) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.size(), m_indices.data(), m_type);
  }

  unbind();
}

void VBO::generate() {
  glGenBuffers(1, &m_id_vbo);
  glGenBuffers(1, &m_id_ebo);
}

/* Update vbo with vertexes from geometery (used for rendering glyphs) */
void VBO::update(const Geometry& geometry) {
  m_vertexes = geometry.get_vertexes();
  n_elements = geometry.get_n_elements();
  bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_vertexes.size(), m_vertexes.data());
  unbind();
}

void VBO::bind() {
  glBindBuffer(GL_ARRAY_BUFFER, m_id_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id_ebo);
}

void VBO::unbind() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VBO::free() {
  glDeleteBuffers(1, &m_id_vbo);
  glDeleteBuffers(1, &m_id_ebo);
}
