#include <vertexes/vao.hpp>

VAO::VAO() {
  // generate vao
  glGenVertexArrays(1, &m_id);
}

void VAO::bind() {
  glBindVertexArray(m_id);
}

void VAO::unbind() {
  glBindVertexArray(0);
}

void VAO::set_attribute(const Attribute& attribute) {
  // create a vertex attribute
  glVertexAttribPointer(attribute.id, attribute.size, GL_FLOAT, GL_FALSE, attribute.stride * sizeof(float), (void *) (attribute.offset * sizeof(float)));
  glEnableVertexAttribArray(attribute.id);
}

void VAO::free() {
  glDeleteVertexArrays(1, &m_id);
}
