#include <meshes/pyramid.hpp>

Pyramid::Pyramid(const Program& program):
  Mesh(program)
{
  // initialize vbo buffer
  init_buffers();

  // get position attribute from shader
  set_attribute();
}

void Pyramid::set_attribute() {
  // position attribute
  GLuint attr_position = m_program.define_attribute("position");
  glVertexAttribPointer(attr_position, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(attr_position);

  // normal attribute
  GLuint attr_normal = m_program.define_attribute("normal");
  glVertexAttribPointer(attr_normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(attr_normal);
}

std::vector<float> Pyramid::get_vertexes() {
  return m_vertexes;
}

int Pyramid::get_n_vertexes() {
  return m_n_vertexes;
}
