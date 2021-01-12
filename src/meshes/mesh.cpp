#include <meshes/mesh.hpp>
#include <iostream>

void Mesh::draw(GLenum render_type) {
  // set render type to (fill, wireframe or point)
  glPolygonMode(GL_FRONT_AND_BACK, render_type);

  // draw all vertexes from cube triangles from bound buffers (vbo)
  int n_vertexes(get_n_vertexes());
  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLES, 0, n_vertexes);
}

void Mesh::free() {
  // de-allocate GPU vertex array & buffer
  glDeleteVertexArrays(1, &m_vao);
  glDeleteBuffers(1, &m_vbo);
}

void Mesh::init_buffers() {
  // VAO holds vertex attributes (positions, colors, texture direction) in GPU
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  // VBO stores VERTEXES array in GPU
  std::vector<float> vertexes(get_vertexes());
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);
}
