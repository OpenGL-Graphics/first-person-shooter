#include <vertexes/vbo.hpp>

VBO::VBO(const Geometry& geometry):
  m_vertexes(geometry.get_vertexes()),
  m_n_vertexes(geometry.get_n_vertexes())
{
  // transfer mesh vertexes to bound VBO
  glGenBuffers(1, &m_id);
  bind();
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertexes.size(), m_vertexes.data(), GL_STATIC_DRAW);
  unbind();
}

std::vector<float> VBO::get_vertexes() const {
  return m_vertexes;
}

int VBO::get_n_vertexes() const {
  // needed by Render::draw()
  return m_n_vertexes;
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
