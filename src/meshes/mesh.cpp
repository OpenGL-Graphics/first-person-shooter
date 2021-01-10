#include <meshes/mesh.hpp>

Mesh::Mesh(const std::vector<float>& vertexes, int n_vertexes):
  m_n_vertexes(n_vertexes)
{
  // initialize vbo buffer
  init_buffers(vertexes);
}

void Mesh::draw() {
  // draw all vertexes from cube triangles from bound buffers (vbo)
  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLES, 0, m_n_vertexes);
}

void Mesh::free() {
  // de-allocate GPU vertex array & buffer
  glDeleteVertexArrays(1, &m_vao);
  glDeleteBuffers(1, &m_vbo);
}

void Mesh::init_buffers(const std::vector<float>& vertexes) {
  // VAO holds vertex attributes (positions, colors, texture direction) in GPU
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  // VBO stores VERTEXES array in GPU
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);
}
